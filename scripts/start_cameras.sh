#! /bin/bash
ssh resqbots@lotti2-pc.local 'cd Lotti2.0/scripts/; ./start_webcam_publisher.sh"'

# gnome-terminal --tab --title="camera1"  ssh -t resqbots@lotti2-pc.local \
# "source ~/.bashrc && \
# ros2 run webcam_publisher ffmpeg_camera_node --ros-args -p device:="/dev/video1""
# gnome-terminal --tab --title="camera2"  ssh -t resqbots@lotti2-pc.local \
# "source ~/.bashrc && \
# ros2 run webcam_publisher ffmpeg_camera_node --ros-args -p device:="/dev/video3""
# gnome-terminal --tab --title="camera3"  ssh -t resqbots@lotti2-pc.local \
# "source ~/.bashrc && \
# ros2 run webcam_publisher ffmpeg_camera_node --ros-args -p device:="/dev/video5""
# gnome-terminal --tab --title="camera4"  ssh -t resqbots@lotti2-pc.local \
# "source ~/.bashrc && \
# ros2 run webcam_publisher ffmpeg_camera_node --ros-args -p device:="/dev/video9""