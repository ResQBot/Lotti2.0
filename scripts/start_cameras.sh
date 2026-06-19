#! /bin/bash
gnome-terminal --tab --title="cameras"  -ssh -t resqbots@lotti2-pc.local \
"source ~/.bashrc && \
ros2 run webcam_publisher ffmpeg_camera_node "