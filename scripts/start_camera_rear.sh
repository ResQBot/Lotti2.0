#! /bin/bash
if [ $(pgrep -f "camera_2") ]; then
    pkill --signal 2 -f "camera_2"
else
    gnome-terminal --tab --title="camera_2" \
    -- ssh -t resqbots@lotti2-pc.local \
    "source /opt/ros/jazzy/setup.bash; \
    export ROS_DOMAIN_ID=17; \
    cd Lotti2.0/; \
    source install/setup.bash; \
    ros2 run webcam_publisher ffmpeg_camera_node --ros-args \
    -p device:="/dev/v4l/by-id/usb-H264_USB_Camera_H264_USB_Camera_2020032801-video-index0" \
    -p camera_name:="camera_2"; \
    -p fps:=20; \
    "
fi