# Based on:
 
# Copyright 2020 ros2_control Development Team
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Changes made to the example by the Res.Q Bots Austria Team 
# of Vienna University of Applied Sciences

import os
import yaml
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, RegisterEventHandler, IncludeLaunchDescription
from launch.conditions import IfCondition
from launch.event_handlers import OnProcessExit
from launch.substitutions import Command, FindExecutable, PathJoinSubstitution, LaunchConfiguration

from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare
from ament_index_python import get_package_share_directory
from moveit_configs_utils import MoveItConfigsBuilder


def load_yaml(package_name, file_path):
    package_path = get_package_share_directory(package_name)
    absolute_file_path = os.path.join(package_path, file_path)

    try:
        with open(absolute_file_path, "r") as file:
            return yaml.safe_load(file)
    except EnvironmentError:  # parent of IOError, OSError *and* WindowsError where available
        return None
    
def generate_launch_description():
    # Declare arguments
    declared_arguments = []
    declared_arguments.append(
        DeclareLaunchArgument(
            "gui",
            default_value="false",
            description="No RViz2 with this launch file.",
        )
    )

    # Initialize Arguments
    gui = LaunchConfiguration("gui")

    # Get URDF via xacro
    robot_description_content = Command([
        PathJoinSubstitution([FindExecutable(name="xacro")]),
            " ",
            PathJoinSubstitution([
                FindPackageShare("lotti2_control"),
                    "description/urdf",
                    "Lotti.urdf.xacro",
                ]
            ),
        ]
    )

    robot_description = {"robot_description": robot_description_content}

    lotti2_controllers = PathJoinSubstitution([
        FindPackageShare("lotti2_control"),
            "config",
            "lotti2_controllers.yaml",
        ]
    )

    control_node = Node(
        package="controller_manager",
        executable="ros2_control_node",
        parameters=[lotti2_controllers],
        output="both",
        remappings=[
            ("~/robot_description", "/robot_description"),
        ],
    )

    robot_state_pub_node = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        output="both",
        parameters=[robot_description],
    )

    joint_state_broadcaster_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["joint_state_broadcaster"],
    )

    """
    arm_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "lotti2_arm_controller",
            "--param-file",
            lotti2_controllers,
        ],
    )
    """
    drive_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "lotti2_drive_controller",
            "--param-file",
            lotti2_controllers,
            "--controller-ros-args",
            "-r /lotti2_drive_controller/cmd_vel:=/cmd/chains",
        ],
    )

    flipper_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=[
            "lotti2_flipper_controller",
            "--param-file",
            lotti2_controllers,
        ],
    )

    # Delay start of controllers after `joint_state_broadcaster`
    """
    delay_arm_controller = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[arm_controller_spawner],
        )
    )
    """
    
    delay_drive_controller = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[drive_controller_spawner],
        )
    )

    delay_flipper_controller = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[flipper_controller_spawner],
        )
    )

    """
    # Get parameters for the Servo node
    servo_yaml = load_yaml("lotti2_control", "config/lotti_servo_config.yaml")
    servo_params = {"moveit_servo": servo_yaml}

    moveit_config = (
        MoveItConfigsBuilder("lotti")
        .robot_description(file_path="config/Lotti.urdf.xacro")
        .to_moveit_configs()
    )

    servo_node = Node(
        package="moveit_servo",
        executable="servo_node_main",
        parameters=[
            servo_params,
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
        ],
        output="screen",
    )

    delay_servo_node = RegisterEventHandler(
        event_handler=OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[servo_node],
        )
    )
    """



    nodes = [
        control_node,
        robot_state_pub_node,
        joint_state_broadcaster_spawner,
        #delay_arm_controller,
        delay_drive_controller,
        delay_flipper_controller,
        #delay_servo_node,
    ]

    return LaunchDescription(declared_arguments + nodes)