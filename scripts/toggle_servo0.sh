#! /bin/bash
gnome-terminal --tab --title="call_servo" -- \
#source /opt/ros/jazzy/setup.bash \
#&& export ROS_DOMAIN_ID=17 \
ros2 service call /servo_node/switch_command_type moveit_msgs/srv/ServoCommandType "{command_type: 0}"
