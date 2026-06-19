#! /bin/bash
gnome-terminal --tab --title="robot"  -ssh -t resqbots@lotti2-pc.local \
"source ~/.bashrc && \
ros2 launch lotti2_control robot.launch.py"