#! /bin/bash
if [ $(pgrep -f "camera_5") ]; then
    pkill --signal 2 -f "camera_5"
else
    gnome-terminal --tab --title="camera_arm" \
    -- ssh -t resqbots@lotti2-pc.local \
    "source /opt/ros/jazzy/setup.bash; \
    export ROS_DOMAIN_ID=17; \
    cd Lotti2.0; \
    source install/setup.bash; \
    ros2 run webcam_publisher ffmpeg_camera_node --ros-args  \
    -p device:='/dev/v4l/by-id/usb-Creative_Technology_Ltd._Live__Cam_Sync_HD_VF0770-video-index0' \
    -p camera_name:='camera_5'; \
    "
fi