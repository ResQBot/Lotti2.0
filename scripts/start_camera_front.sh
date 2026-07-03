#! /bin/bash
if [ $(pgrep -f "camera_1") ]; then
    pkill --signal 2 -f "camera_1"
else
    gnome-terminal --tab --title="camera_1" \
    -- ssh -t resqbots@lotti2-pc.local \
    "source /opt/ros/jazzy/setup.bash; \
    export ROS_DOMAIN_ID=17; \
    cd Lotti2.0/; \
    source install/setup.bash; \
    ros2 run webcam_publisher ffmpeg_camera_node --ros-args \
    -p device:="/dev/v4l/by-id/usb-PC-LM1E_PC-LM1E_PC-LM1E-video-index0" \
    -p camera_name:="camera_1"; \
    "
fi