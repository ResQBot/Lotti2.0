#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import CompressedImage
from cv_bridge import CvBridge
import cv2
import threading
import time
import os
from ament_index_python.packages import get_package_share_directory

try:
    from ultralytics import YOLO
except ImportError:
    YOLO = None

class CameraDashboard(Node):
    def __init__(self):
        super().__init__('camera_dashboard')
        self.bridge = CvBridge()

        # =====================================================================
        # CENTRAL CAMERA CONFIGURATION MATRIX
        # Dynamically scales to any number of cameras added or renamed here.
        # =====================================================================
        self.camera_config = [
            {'name': 'front_left',  'topic': '/camera_1/image_raw/compressed'},
            {'name': 'front_right', 'topic': '/camera_2/image_raw/compressed'},
            {'name': 'back_left',   'topic': '/camera_3/image_raw/compressed'},
            {'name': 'back_right',  'topic': '/camera_4/image_raw/compressed'},
            {'name': 'arm_camera',  'topic': '/camera_5/image_raw/compressed'}
        ]

        # Name of the specific camera designated for advanced AI analysis (YOLO + QR + Motion)
        self.ai_camera_name = 'arm_camera'

        # Dynamically build runtime tracking dictionaries from config matrix
        self.frames = {config['name']: None for config in self.camera_config}
        self.detections = {config['name']: "" for config in self.camera_config}
        
        self.ai_processed_frame = None
        self.yolo_model = None
        self.qr_detector = None
        self.lock = threading.Lock()

        # -------------------------------------------------------------
        # ROS 2 PARAMETER FOR SYSTEM CONFIGURATION
        # -------------------------------------------------------------
        # This registers 'enable_motion' as a parameter that can be changed on-the-fly
        self.declare_parameter('enable_motion', True)

        # -------------------------------------------------------------
        # STATEFUL MOTION DETECTION CONFIGURATION
        #change the lower varThreshold means more sensitive motion detection and vice versa.
        # -------------------------------------------------------------
        self.motion_subtractor = cv2.createBackgroundSubtractorMOG2(history=300, varThreshold=20, detectShadows=False)
        self.arm_moving = False 

        # Initialize AI Models (YOLO and WeChat QR)
        self._configure_ai_models()

        # Dynamically generate native ROS 2 subscriptions from config matrix
        for config in self.camera_config:
            cam_name = config['name']
            topic_str = config['topic']
            
            self.create_subscription(
                CompressedImage,
                topic_str,
                lambda msg, name=cam_name: self.image_callback(msg, name),
                qos_profile_sensor_data
            )
            self.get_logger().info(f"Dynamically subscribed: '{cam_name}' -> Listening on {topic_str}")

        # Start the background execution worker thread
        self.ai_thread = threading.Thread(target=self.ai_processing_worker, daemon=True)
        self.ai_thread.start()

    def _configure_ai_models(self):
        """Locates and loads local YOLO and WeChat QR model assets from shared index paths."""
        package_share = get_package_share_directory('lotti_vision')
        
        #  Configure YOLO
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

        #  Configure WeChat QR Code Engine
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

    def image_callback(self, msg, cam_name):
        """Unpacks high-frequency compressed image strings, dropping stale data to maintain zero-latency."""
        try:
            now = self.get_clock().now()
            msg_time = rclpy.time.Time.from_msg(msg.header.stamp)
            latency = (now - msg_time).nanoseconds / 1e9
            
            # Latency mitigation threshold gate (100ms)
            if latency > 0.1:
                return

            cv_image = self.bridge.compressed_imgmsg_to_cv2(msg, desired_encoding='bgr8')
            with self.lock:
                self.frames[cam_name] = cv_image
        except Exception as e:
            self.get_logger().error(f"Decoder error on camera [{cam_name}]: {e}")

    def detect_motion(self, frame, annotated_frame):
        """
        Isolated background subtraction pipeline. Cleans pixel variance, 
        filters structural noise, and tags true displacement targets.
        """
        # Fetch parameter state safely on every evaluation cycle
        motion_parameter_enabled = self.get_parameter('enable_motion').get_parameter_value().bool_value

        # Abort immediately if the parameter is false, the arm is moving, or subtractor is missing
        if not motion_parameter_enabled or self.arm_moving or self.motion_subtractor is None:
            return False

        # Preprocessing: Grayscale and blur
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        blurred = cv2.GaussianBlur(gray, (21, 21), 0)

        # Feed the frame into the Mixture of Gaussians background subtractor
        fg_mask = self.motion_subtractor.apply(blurred)

        # Morphological transformations to snap tiny loose pixels together
        kernel = cv2.getStructuringElement(cv2.MORPH_RECT, (5, 5))
        fg_mask = cv2.morphologyEx(fg_mask, cv2.MORPH_OPEN, kernel)
        fg_mask = cv2.morphologyEx(fg_mask, cv2.MORPH_CLOSE, kernel)

        # Extract boundaries around the isolated movement fields
        contours, _ = cv2.findContours(fg_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        motion_found = False

        for contour in contours:
            if cv2.contourArea(contour) < 900:
                continue

            motion_found = True
            (x, y, w, h) = cv2.boundingRect(contour)

            # Paint Motion Bounding Box (Distinctive Safety Orange)
            cv2.rectangle(annotated_frame, (x, y), (x + w, y + h), (0, 140, 255), 2)
            cv2.putText(annotated_frame, "MOTION DETECTED", (x, y - 8),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0, 140, 255), 1, cv2.LINE_AA)

        return motion_found

    def ai_processing_worker(self):
        """Isolated background pipeline handler. Focuses computational loops on the designated AI camera."""
        while rclpy.ok():
            time.sleep(0.04)  # Constrain AI loop processing to ~25 FPS

            working_frame = None
            with self.lock:
                if self.frames[self.ai_camera_name] is not None:
                    working_frame = self.frames[self.ai_camera_name].copy()

            if working_frame is not None:
                annotated_frame = working_frame.copy()
                status_messages = []

                # -------------------------------------------------------------
                # RUN YOLO OBJECT DETECTION
                # -------------------------------------------------------------
                if self.yolo_model is not None:
                    results = self.yolo_model.predict(source=working_frame, conf=0.5, verbose=False)
                    result = results[0]
                    detected_names = []

                    if len(result.boxes) > 0:
                        for box in result.boxes:
                            x1, y1, x2, y2 = map(int, box.xyxy[0])
                            conf = float(box.conf[0])
                            cls_idx = int(box.cls[0])
                            class_name = result.names[cls_idx]
                            detected_names.append(class_name)

                            cv2.rectangle(annotated_frame, (x1, y1), (x2, y2), (0, 255, 0), 2)
                            label = f"{class_name} {conf:.2f}"
                            (w, h), _ = cv2.getTextSize(label, cv2.FONT_HERSHEY_SIMPLEX, 0.5, 1)
                            cv2.rectangle(annotated_frame, (x1, y1 - h - 5), (x1 + w, y1), (0, 255, 0), -1)
                            cv2.putText(annotated_frame, label, (x1, y1 - 3),
                                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 0), 1, cv2.LINE_AA)

                    if detected_names:
                        status_messages.append(f"HAZARDS: {', '.join(set(detected_names))}")

                # -------------------------------------------------------------
                #  RUN WECHAT DEEP-LEARNING QR SCANNER
                # -------------------------------------------------------------
                if self.qr_detector is not None:
                    try:
                        decoded_info, points = self.qr_detector.detectAndDecode(working_frame)
                        
                        if decoded_info:
                            for info_text, box_points in zip(decoded_info, points):
                                if info_text and box_points is not None:
                                    pts = box_points.astype(int)
                                    cv2.polylines(annotated_frame, [pts], isClosed=True, color=(255, 0, 0), thickness=2)
                                    status_messages.append(f"QR: {info_text}")
                                    cv2.putText(annotated_frame, f"DATA: {info_text}", tuple(pts[0]),
                                                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 0), 1, cv2.LINE_AA)
                    except Exception as qr_err:
                        self.get_logger().error(f"WeChat QR Engine Error: {qr_err}")

                # -------------------------------------------------------------
                # RUN ISOLATED MOTION DETECTION (IF PARAMETER ALLOWS)
                # -------------------------------------------------------------
                motion_detected = self.detect_motion(working_frame, annotated_frame)
                if motion_detected:
                    status_messages.append("MOTION: ACTIVE")

                # If the feature is explicitly disabled, show it on the telemetry summary
                motion_enabled = self.get_parameter('enable_motion').get_parameter_value().bool_value
                if not motion_enabled:
                    status_messages.append("MOTION: DISABLED")

                # -------------------------------------------------------------
                #  RECONCILE TEXT SUMMARY PAYLOADS
                # -------------------------------------------------------------
                if status_messages:
                    status_summary = " | ".join(status_messages)
                else:
                    status_summary = "Scanning clear. No active tracking targets."

                with self.lock:
                    self.detections[self.ai_camera_name] = status_summary
                    self.ai_processed_frame = annotated_frame


def main(args=None):
    rclpy.init(args=args)
    dashboard_node = CameraDashboard()

    # Spin ROS 2 callback nodes in an isolated background thread context
    ros_thread = threading.Thread(target=rclpy.spin, args=(dashboard_node,), daemon=True)
    ros_thread.start()

    dashboard_node.get_logger().info("Dashboard operational. Press 'm' over feed window to toggle motion tracking.")

    try:
        while rclpy.ok():
            time.sleep(0.016)  # Cap high-level window paint draws at stable ~60 Hz

            with dashboard_node.lock:
                display_frames = {k: v.copy() if v is not None else None for k, v in dashboard_node.frames.items()}
                display_text = dashboard_node.detections.copy()
            
            if dashboard_node.ai_processed_frame is not None:
                display_frames[dashboard_node.ai_camera_name] = dashboard_node.ai_processed_frame.copy()
            
            for cam_name, frame in display_frames.items():
                if frame is not None:
                    text = display_text.get(cam_name, "")
                    if text: 
                        text_color = (0, 0, 255) if ("HAZARDS" in text or "MOTION: ACTIVE" in text) else (0, 255, 0)
                        cv2.putText(frame, text, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 
                                    0.5, text_color, 2, cv2.LINE_AA)
                    
                    window_title = cam_name.replace('_', ' ').upper() + " FEED"
                    cv2.imshow(window_title, frame)

            # Handle Keyboard Intercepts across UI Windows
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break
            elif key == ord('m'):
                # Read current ROS 2 parameter, invert it, and update it dynamically
                current_state = dashboard_node.get_parameter('enable_motion').get_parameter_value().bool_value
                new_state = not current_state
                
                new_param = rclpy.parameter.Parameter('enable_motion', rclpy.Parameter.Type.BOOL, new_state)
                dashboard_node.set_parameters([new_param])
                dashboard_node.get_logger().info(f"[TOGGLE] Motion Detection runtime state shifted to: {new_state}")

    except KeyboardInterrupt:
        pass
    finally:
        dashboard_node.destroy_node()
        rclpy.shutdown()
        cv2.destroyAllWindows()

if __name__ == '__main__':
    main()