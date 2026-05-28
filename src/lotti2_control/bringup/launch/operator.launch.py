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
    
def generate_launch_description():
    # Declare arguments
    declared_arguments = []

    rviz_config_file = PathJoinSubstitution([
        FindPackageShare("lotti2_control"), "config", "view_lotti.rviz"]
    )

    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", rviz_config_file],
    )

    joy_node = Node(
        package='joy',
        executable='joy_node',
    )

    teleop_package = get_package_share_directory('lotti2_teleop')
    teleop_node = IncludeLaunchDescription(
        os.path.join(teleop_package, 'launch', 'teleop.launch.py'),
    )



    nodes = [
        rviz_node,
        joy_node,
        teleop_node,
    ]

    return LaunchDescription(declared_arguments + nodes)