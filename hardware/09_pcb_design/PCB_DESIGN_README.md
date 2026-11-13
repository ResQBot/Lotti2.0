# PCB Desing section
This is the PCB Design section of the storage and Documentation area

# Power Delivery
The Power delivery is split into two section, one section gets the two battery inputs, merges them to a stable 24V for further use and also secures two individual 24V lines for driving the Motors. Power, Voltage and current measurement happens here.
- 24V secured from BAT1
- 24V secured from BAT2
- 24V Fused from BAT1 and BAT2 (BATSYNC)

The secured lines indicate that the EMERGENCY STOP Button (and software emergency stop (tbi)) switch the power off on these traces. The synced 24V are not secured, because the PC in Lotti2 should keep running, even if the EMERGENCY STOP is pushed or a Battery is removed.

The second PCB will handle interfacing, communication and measurement, as well as some 5V, 12V and 3V3 conversion.
The STM32 Nucleo is mounted there and it communicates with the measuremnt ICs on the PowerBoard.

## Implemented changes:
- Reworked PD with thicker traces, more cooling (4 Layer stackup) and 40A Capability across the PD Components.
- the 12V conversion is handled externaly and is not planned to be handled internally for the PC.
- Different current measurement is used. (now with shunts)


## Open Problems witht the PCB (ideas and stuff for v3)
- Rework PD Traces
- include relay for smooth change for the PC 12V line.