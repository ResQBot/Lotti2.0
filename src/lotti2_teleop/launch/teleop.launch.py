from launch import LaunchDescription
from launch_ros.actions import Node

def generate_launch_description():
    return LaunchDescription([
        Node(
            package='lotti2_teleop',
            executable='teleop',
            output='screen',
            name='teleop',
            arguments=['--ros-args', '--log-level', 'info'],
        ),
    ])