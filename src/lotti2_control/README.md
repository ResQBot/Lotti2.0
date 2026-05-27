Unitree motors:
To avoid problems with `-ld` not finding your library, copy the `/lib/libUnitreeMotorSDK_Linux64.so` into your `/usr/local/lib` directory. Then run `sudo ldconfig`.

Need to instal ioport

get permissions for USB devices with `sudo usermod -a -G dialout $whoami`

Error IOPort /home/unitree/unitree_actuator_sdk-all/ ... usually means, you have specified the wrong USB device to connect to. 

if the motors throw an Error on data.merror the need to be reset by switching off the power.

After making all changes, reboot the pc.


CobeMars motors:
