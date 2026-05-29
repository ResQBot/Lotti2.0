from setuptools import setup

package_name = 'webcam_publisher'

setup(
    name=package_name,
    version='0.1.0',
    packages=[package_name],
    install_requires=['setuptools'],
    entry_points={
        'console_scripts': [
            'ffmpeg_camera_node = webcam_publisher.ffmpeg_camera_node:main',
        ],
    },
)
