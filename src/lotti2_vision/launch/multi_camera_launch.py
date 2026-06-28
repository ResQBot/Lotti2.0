from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='lotti_vision',
            executable='camera_dashboard',
            name='camera_dashboard',
            output='screen',
            parameters=[{'enable_motion': False}]
        ),

        # CAMERA 1: Front Left
        Node(
            package='webcam_publisher',
            executable='ffmpeg_camera_node',
            name='camera_1_publisher',
            output='screen',
            parameters=[{
                'device': '/dev/video0',
                'camera_name': 'camera_1',
                'width': 640,
                'height': 480,
                'fps': 30
            }]
        ),

        # CAMERA 2: Front Right
        Node(
            package='webcam_publisher',
            executable='ffmpeg_camera_node',
            name='camera_2_publisher',
            output='screen',
            parameters=[{
                'device': '/dev/video5', 
                'camera_name': 'camera_2',
                'width': 640,
                'height': 480,
                'fps': 30
            }]
        ),

        # CAMERA 3: Back Left
        Node(
            package='webcam_publisher',
            executable='ffmpeg_camera_node',
            name='camera_3_publisher',
            output='screen',
            parameters=[{
                'device': '/dev/video9', 
                'camera_name': 'camera_3',
                'width': 640,
                'height': 480,
                'fps': 30
            }]
        ),

        # CAMERA 4: Back Right
        Node(
            package='webcam_publisher',
            executable='ffmpeg_camera_node',
            name='camera_4_publisher',
            output='screen',
            parameters=[{
                'device': '/dev/video12',
                'camera_name': 'camera_4',
                'width': 640,
                'height': 480,
                'fps': 30
            }]
        ),

        # CAMERA 5: Arm Camera (Designated AI Target for YOLO/QR)
        Node(
            package='webcam_publisher',
            executable='ffmpeg_camera_node',
            name='camera_5_publisher',
            output='screen',
            parameters=[{
                'device': '/dev/video15',
                'camera_name': 'camera_5',
                'width': 640,
                'height': 480,
                'fps': 30
            }]
        ),
    ])