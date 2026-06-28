#!/usr/bin/env python3
"""
ai_node.py
----------
Single-purpose, optimized ROS 2 node for the arm/AI camera ONLY.

This replaces the "process everything" approach of the original dashboard
with a node that does one job well:

    1. Subscribe to a single camera topic (no wasted CPU/bandwidth on the
       4 cameras nobody is running AI against).
    2. Decouple ROS callback -> decode -> inference -> display into
       separate stages connected by drop-oldest hand-offs, so a slow
       model (YOLO) can never cause the live feed to fall behind.
    3. Run three detectors on every processed frame:
         - Motion detection (background subtraction)
         - YOLO object detection
         - WeChat deep-learning QR code reading
"""

import os
import time
import threading
import queue

import numpy as np
import cv2

import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import CompressedImage
from ament_index_python.packages import get_package_share_directory

try:
    from ultralytics import YOLO
except ImportError:
    YOLO = None


class AINode(Node):
    """Decode -> motion detection -> YOLO -> QR -> annotate, for one camera."""

    CAMERA_NAME = 'arm_camera'
    CAMERA_TOPIC = '/camera_5/image_raw/compressed'

    # AI inference is capped independently of camera FPS so YOLO/QR never
    # monopolize a core. Raise/lower this to trade latency vs CPU.
    AI_LOOP_PERIOD = 0.04            # ~25 Hz ceiling
    MAX_FRAME_LATENCY_SEC = 0.1      # reject frames older than this pre-decode

    # Motion detection tunables
    MOTION_MIN_AREA = 800            # ignore contours smaller than this (px^2)
    MOTION_VAR_THRESHOLD = 16        # MOG2 sensitivity (lower = more sensitive)
    MOTION_HISTORY = 300             # frames the background model remembers

    def __init__(self):
        super().__init__('ai_node')

        self._raw_queue = queue.Queue(maxsize=1)

        # decode thread -> AI thread: latest decoded frame (+ a sequence
        # number so the AI thread never processes the same frame twice,
        # which would otherwise poison the motion-detection background model)
        self._latest_frame = None
        self._frame_seq = 0
        self._frame_lock = threading.Lock()

        # AI thread -> display loop: latest annotated frame + status text
        self._annotated_frame = None
        self._status_text = ""
        self._result_lock = threading.Lock()

        # --- Models -------------------------------------------------------
        self.yolo_model = None
        self.qr_detector = None
        self.bg_subtractor = cv2.createBackgroundSubtractorMOG2(
            history=self.MOTION_HISTORY,
            varThreshold=self.MOTION_VAR_THRESHOLD,
            detectShadows=False
        )
        self._load_models()

        # --- Subscription ---------------------------------------------
        # Best-effort + depth 1: this is a live feed, not a log. We WANT
        # to drop frames under load rather than buffer and add latency.
        low_latency_qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )
        self.create_subscription(
            CompressedImage,
            self.CAMERA_TOPIC,
            self._on_frame_received,
            low_latency_qos
        )
        self.get_logger().info(
            f"Subscribed ONLY to AI camera '{self.CAMERA_NAME}' @ {self.CAMERA_TOPIC}"
        )

        # --- Worker threads ----------------------------------------------
        self._decode_thread = threading.Thread(
            target=self._decode_worker, daemon=True, name="ai_decode"
        )
        self._decode_thread.start()

        if self.yolo_model is not None or self.qr_detector is not None:
            self._ai_thread = threading.Thread(
                target=self._ai_worker, daemon=True, name="ai_inference"
            )
            self._ai_thread.start()
        else:
            self.get_logger().warning(
                "No YOLO/QR models loaded — running motion detection only."
            )
            self._ai_thread = threading.Thread(
                target=self._ai_worker, daemon=True, name="ai_inference"
            )
            self._ai_thread.start()

    # ---------------------------------------------------------------- #
    # Model loading (same model assets/paths as the original dashboard)
    # ---------------------------------------------------------------- #
    def _load_models(self):
        package_share = get_package_share_directory('lotti_vision')

        if YOLO is None:
            self.get_logger().warning("Package 'ultralytics' not found. YOLO features offline.")
        else:
            model_path = os.path.join(package_share, 'models', 'best.onnx')
            if os.path.exists(model_path):
                try:
                    self.yolo_model = YOLO(model_path, task='detect')
                    self.get_logger().info("YOLOv8 ONNX model loaded successfully.")
                except Exception as e:
                    self.get_logger().error(f"Failed to initialize YOLO model: {e}")
            else:
                self.get_logger().warning(f"YOLO model file missing at: {model_path}")

        wechat_dir = os.path.join(package_share, 'models', 'wechat_qr')
        try:
            self.qr_detector = cv2.wechat_qrcode_WeChatQRCode(
                os.path.join(wechat_dir, "detect.prototxt"),
                os.path.join(wechat_dir, "detect.caffemodel"),
                os.path.join(wechat_dir, "sr.prototxt"),
                os.path.join(wechat_dir, "sr.caffemodel")
            )
            self.get_logger().info("WeChat Deep-Learning QR Engine loaded successfully.")
        except Exception as e:
            self.qr_detector = None
            self.get_logger().error(f"Failed to load WeChat QR engine models: {e}")

    # ---------------------------------------------------------------- #
    # ROS callback — kept deliberately tiny. No decode, no inference,
    # no cv2 calls. Just a non-blocking enqueue with drop-oldest semantics.
    # ---------------------------------------------------------------- #
    def _on_frame_received(self, msg):
        # Reject already-stale frames BEFORE paying any decode cost.
        now = self.get_clock().now()
        msg_time = rclpy.time.Time.from_msg(msg.header.stamp)
        latency = (now - msg_time).nanoseconds / 1e9
        if latency > self.MAX_FRAME_LATENCY_SEC:
            return

        try:
            self._raw_queue.put_nowait(msg.data)
        except queue.Full:
            try:
                self._raw_queue.get_nowait()  # drop the stale one
            except queue.Empty:
                pass
            try:
                self._raw_queue.put_nowait(msg.data)
            except queue.Full:
                pass

    # ---------------------------------------------------------------- #
    # Decode thread — full resolution, full color. This is the AI
    # camera, so detection accuracy matters more than it did for the
    # multi-camera display (which decoded at half-size to save CPU).
    # ---------------------------------------------------------------- #
    def _decode_worker(self):
        while rclpy.ok():
            try:
                raw_bytes = self._raw_queue.get(timeout=1.0)
            except queue.Empty:
                continue

            np_arr = np.frombuffer(raw_bytes, dtype=np.uint8)
            frame = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
            if frame is not None:
                with self._frame_lock:
                    self._latest_frame = frame
                    self._frame_seq += 1

    # ---------------------------------------------------------------- #
    # AI thread — motion detection + YOLO + QR, rate-capped, and
    # guaranteed to never run the same frame through twice.
    # ---------------------------------------------------------------- #
    def _ai_worker(self):
        last_seen_seq = -1

        while rclpy.ok():
            time.sleep(self.AI_LOOP_PERIOD)

            with self._frame_lock:
                if self._latest_frame is None or self._frame_seq == last_seen_seq:
                    continue
                working_frame = self._latest_frame.copy()
                last_seen_seq = self._frame_seq

            annotated = working_frame.copy()
            status_messages = []

            # ---- STEP 1: Motion detection (background subtraction) ----
            if self._run_motion_detection(working_frame, annotated):
                status_messages.append("MOTION DETECTED")

            # ---- STEP 2: YOLO object detection ----
            if self.yolo_model is not None:
                hazard_names = self._run_yolo(working_frame, annotated)
                if hazard_names:
                    status_messages.append(f"HAZARDS: {', '.join(hazard_names)}")

            # ---- STEP 3: WeChat QR decode ----
            if self.qr_detector is not None:
                for payload in self._run_qr(working_frame, annotated):
                    status_messages.append(f"QR: {payload}")

            status_summary = (
                " | ".join(status_messages) if status_messages
                else "Scanning clear. No active tracking targets."
            )

            with self._result_lock:
                self._annotated_frame = annotated
                self._status_text = status_summary

    def _run_motion_detection(self, frame, annotated):
        fg_mask = self.bg_subtractor.apply(frame)
        fg_mask = cv2.morphologyEx(fg_mask, cv2.MORPH_OPEN, np.ones((3, 3), np.uint8))
        fg_mask = cv2.dilate(fg_mask, np.ones((5, 5), np.uint8), iterations=2)

        contours, _ = cv2.findContours(fg_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        motion_found = False
        for c in contours:
            if cv2.contourArea(c) < self.MOTION_MIN_AREA:
                continue
            motion_found = True
            x, y, w, h = cv2.boundingRect(c)
            # Orange: visually distinct from YOLO's green and QR's blue
            cv2.rectangle(annotated, (x, y), (x + w, y + h), (0, 165, 255), 2)
            cv2.putText(annotated, "MOTION", (x, max(0, y - 5)),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 165, 255), 1, cv2.LINE_AA)
        return motion_found

    def _run_yolo(self, frame, annotated):
        results = self.yolo_model.predict(source=frame, conf=0.5, verbose=False)
        result = results[0]
        detected_names = []

        for box in result.boxes:
            x1, y1, x2, y2 = map(int, box.xyxy[0])
            conf = float(box.conf[0])
            cls_idx = int(box.cls[0])
            class_name = result.names[cls_idx]
            detected_names.append(class_name)

            cv2.rectangle(annotated, (x1, y1), (x2, y2), (0, 255, 0), 2)
            label = f"{class_name} {conf:.2f}"
            (w, h), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 1)
            cv2.rectangle(annotated, (x1, y1 - h - 5), (x1 + w, y1), (0, 255, 0), -1)
            cv2.putText(annotated, label, (x1, y1 - 3),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv2.LINE_AA)

        return list(set(detected_names))

    def _run_qr(self, frame, annotated):
        payloads = []
        try:
            decoded_info, points = self.qr_detector.detectAndDecode(frame)
            if decoded_info:
                for info_text, box_points in zip(decoded_info, points):
                    if info_text and box_points is not None:
                        pts = box_points.astype(int)
                        cv2.polylines(annotated, [pts], isClosed=True, color=(255, 0, 0), thickness=2)
                        cv2.putText(annotated, f"DATA: {info_text}", tuple(pts[0]),
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 1, cv2.LINE_AA)
                        payloads.append(info_text)
        except Exception as qr_err:
            self.get_logger().error(f"WeChat QR Engine Error: {qr_err}")
        return payloads

    # ---------------------------------------------------------------- #
    # Snapshot accessor for the display loop in main()
    # ---------------------------------------------------------------- #
    def get_display_frame(self):
        with self._result_lock:
            if self._annotated_frame is not None:
                return self._annotated_frame.copy(), self._status_text

        with self._frame_lock:
            if self._latest_frame is not None:
                return self._latest_frame.copy(), ""

        return None, ""


def main(args=None):
    rclpy.init(args=args)
    node = AINode()

    ros_thread = threading.Thread(target=rclpy.spin, args=(node,), daemon=True)
    ros_thread.start()

    window_title = node.CAMERA_NAME.replace('_', ' ').upper() + " FEED"

    try:
        while rclpy.ok():
            time.sleep(0.016)  # ~60 Hz display cap

            frame, status_text = node.get_display_frame()
            if frame is None:
                continue

            if status_text:
                text_color = (0, 0, 255) if "HAZARDS" in status_text else (0, 255, 0)
                cv2.putText(frame, status_text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, text_color, 2, cv2.LINE_AA)

            cv2.imshow(window_title, frame)

            if cv2.waitKey(1) & 0xFF == ord('q'):
                break

    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()
        cv2.destroyAllWindows()


if __name__ == '__main__':
    main()