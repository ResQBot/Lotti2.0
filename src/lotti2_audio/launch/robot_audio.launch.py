from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, ExecuteProcess
from launch.substitutions import LaunchConfiguration

CAPS = ("application/x-rtp,media=audio,clock-rate=48000,"
        "encoding-name=OPUS,payload=96")

# Built-in 4-channel microphone (digital mic array)
DEFAULT_MIC = ("alsa_input.pci-0000_00_1f.3-platform-skl_hda_dsp_generic"
               ".HiFi__hw_sofhdadsp_6__source")
# Headphone output
DEFAULT_SPEAKER = ("alsa_output.pci-0000_00_1f.3-platform-skl_hda_dsp_generic"
                   ".HiFi__hw_sofhdadsp__sink")


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument('operator_ip', default_value='192.168.1.50'),
        DeclareLaunchArgument('send_port', default_value='5000'),
        DeclareLaunchArgument('recv_port', default_value='5001'),
        DeclareLaunchArgument('mic', default_value=DEFAULT_MIC),
        DeclareLaunchArgument('speaker', default_value=DEFAULT_SPEAKER),

        # robot mic -> operator
        ExecuteProcess(
            cmd=['gst-launch-1.0', '-q', 'pulsesrc',
                 ['device=', LaunchConfiguration('mic')], '!',
                 'audioconvert', '!', 'audioresample', '!',
                 'audio/x-raw,rate=48000,channels=1', '!',
                 'opusenc', 'bitrate=32000', 'frame-size=20', '!',
                 'rtpopuspay', '!', 'udpsink',
                 ['host=', LaunchConfiguration('operator_ip')],
                 ['port=', LaunchConfiguration('send_port')]],
            name='audio_tx', output='screen', respawn=True),

        # operator -> robot speaker
        ExecuteProcess(
            cmd=['gst-launch-1.0', '-q', 'udpsrc',
                 ['port=', LaunchConfiguration('recv_port')],
                 ['caps=', CAPS], '!',
                 'rtpjitterbuffer', 'latency=50', '!', 'rtpopusdepay', '!',
                 'opusdec', '!', 'audioconvert', '!', 'pulsesink',
                 ['device=', LaunchConfiguration('speaker')], 'sync=false'],
            name='audio_rx', output='screen', respawn=True),
    ])