import subprocess
import threading
import rclpy
from rclpy.node import Node
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
from sensor_msgs.msg import CompressedImage

class FFmpegCameraNode(Node):
    def __init__(self):
        super().__init__('ffmpeg_camera_node')

        # Declare dynamic hardware tracking parameters
        self.declare_parameter('device', '/dev/video0')
        self.declare_parameter('camera_name', 'camera_1')
        self.declare_parameter('width', 640)
        self.declare_parameter('height', 480)
        self.declare_parameter('fps', 20)

        device      = self.get_parameter('device').value
        camera_name = self.get_parameter('camera_name').value
        width       = self.get_parameter('width').value
        height      = self.get_parameter('height').value
        fps         = self.get_parameter('fps').value

        qos = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=1
        )
        
        # Dynamically maps the target topic name based on launch configuration input
        topic_name = f'/{camera_name}/image_raw/compressed'
        self.publisher_ = self.create_publisher(CompressedImage, topic_name, qos)

        ffmpeg_cmd = [
            'ffmpeg',
            '-fflags', 'nobuffer+discardcorrupt',  # Added discardcorrupt to prevent horizontal screen tearing
            '-flags', 'low_delay',
            '-probesize', '32',
            '-analyzeduration', '0',
            '-thread_queue_size', '512',
            '-f', 'v4l2',
            '-input_format', 'mjpeg',
            '-framerate', str(fps),
            '-video_size', f'{width}x{height}',
            '-i', device,
            '-err_detect', 'ignore_err',
            '-vcodec', 'copy',      
            '-f', 'mjpeg',          
            'pipe:1'
        ]

        self.process = subprocess.Popen(
            ffmpeg_cmd,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            bufsize=65536
        )

        self.thread = threading.Thread(target=self._capture_loop, daemon=True)
        self.thread.start()
        self.get_logger().info(f'Initialized {camera_name} on hardware {device} -> Publishing to {topic_name}')

    def _capture_loop(self):
        """ FIXED: Now correctly indented inside the FFmpegCameraNode class """
        buffer = bytearray()
        chunk_size = 8192 # Increased chunk size to pull complete packets faster

        while rclpy.ok():
            if self.process.poll() is not None:
                self.get_logger().error('FFmpeg process terminated unexpectedly.')
                break

            chunk = self.process.stdout.read(chunk_size)
            if not chunk:
                continue
            
            buffer.extend(chunk)

            while True:
                soi = buffer.find(b'\xff\xd8')
                eoi = buffer.find(b'\xff\xd9')

                if soi != -1 and eoi != -1:
                    if eoi > soi:
                        # Extract the complete frame
                        jpeg_data = buffer[soi:eoi+2]
                        
                        msg = CompressedImage()
                        msg.header.stamp = self.get_clock().now().to_msg()
                        msg.header.frame_id = 'camera_frame'
                        msg.format = 'jpeg'
                        msg.data = bytes(jpeg_data)
                        self.publisher_.publish(msg)
                        
                        # Remove the processed frame from the buffer
                        buffer = buffer[eoi+2:]
                    else:
                        # EOI came BEFORE SOI. This means we have a mangled partial frame.
                        # Drop the garbage leading up to the real frame start
                        buffer = buffer[soi:]
                else:
                    # If the buffer is getting too huge and has no valid frame, clear it
                    if len(buffer) > 500000:
                        buffer.clear()
                    break

    def destroy_node(self):
        self.process.terminate()
        super().destroy_node()

def main(args=None):
    rclpy.init(args=args)
    node = FFmpegCameraNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()