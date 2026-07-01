#! /bin/bash
gnome-terminal --tab --title="robot" \
-- ssh -t resqbots@lotti2-pc.local \
"source /opt/ros/jazzy/setup.bash; \
export ROS_DOMAIN_ID=17; \
cd Lotti2.0; \
source install/setup.bash; \
ros2 launch lotti2_control robot.launch.py"; \