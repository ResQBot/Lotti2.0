import os
from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    # Define your ideal mapping setup
    possible_cameras = [
        {'name': 'camera_1', 'device': '/dev/video1'},
        {'name': 'camera_2', 'device': '/dev/video3'},
        {'name': 'camera_3', 'device': '/dev/video5'},
        {'name': 'camera_4', 'device': '/dev/video9'}
    ]

    launch_nodes = []

    # Only spin up a publisher if the device is actually plugged in!
    for config in possible_cameras:
        if os.path.exists(config['device']):
            launch_nodes.append(
                Node(
                    package='webcam_publisher', 
                    executable='ffmpeg_camera_node', 
                    name=f"publisher_{config['name']}",
                    output='screen',
                    parameters=[{
                        'device': config['device'],
                        'camera_name': config['name'],
                        'width': 640,
                        'height': 480,
                        'fps': 30
                    }]
                )
            )
        else:
            print(f"--> [SKIPPED] {config['name']} skipped: Hardware target {config['device']} is not connected.")

    #launch the subscriber dashboard
    #launch_nodes.append(
    #       Node(
    #        package='lotti_vision', 
    #        executable='camera_dashboard',
    #        name='camera_dashboard',
    #         output='screen'
    #    )
    #)
    #return LaunchDescription(launch_nodes)