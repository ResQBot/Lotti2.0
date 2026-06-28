# PCB Troubleshooting Guide
What to do if the PCB stops delivering Power

## Step 1 - If it burns -> it's bad.
Disconnect everything and press the emergency Stop.
Look if anything is really burned or fried, or where the smoke came from.

### Very bad situation -> requires further troubleshooting.
Best to just abandon the PCB and use the backup one. 
See -> what is missing from the Backup.

## Step 2 - If it's just not properly providing power
Measure if the Batteries provide the Proper voltage.
If they do then check and replace the fuse.

You can check if the Relais click if you turn them on.

You can measure if you have a short between VCC and GND. (Beware of the proper direction of the Multimeter).

Check all of the Diodes with the Multimeter. There are 2 THT Diodes on the front and 2-4 SMD Diodes on the Back.

Check if the FETs (Q2 and Q3) are shorted somewhere. If they are you should desolder them and short Pin 3 and 2 (see schematic and Board file).

Alternativly you can use the backup board and solder the missing components (U2, U7, IC1, IC4) from the old board to the new. then it should work. Make sure the Extra Cables are still there. Maybe add the missing fusebox (See backup Components).




