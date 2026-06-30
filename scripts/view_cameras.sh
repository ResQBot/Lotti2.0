#! /bin/bash
if [ $(pgrep -f "_dashboard") ]; then
    pkill --signal 2 -f "_dashboard"
else
    gnome-terminal --tab --title="view_cameras" --command \
    "bash -c 'source /opt/ros/jazzy/setup.bash; \
    export ROS_DOMAIN_ID=17; \
    cd ~/Lotti2.0; \
    source install/setup.bash; \
    ros2 run lotti_vision camera_dashboard \
    '"
fi
