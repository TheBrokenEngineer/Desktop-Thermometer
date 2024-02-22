# Introduction
This document details the schematic layout for the desktop thermometer. There are two schematics, one for USB-2 and the second for USB-C. The only difference between the two is the USB power section.

## Sections
The schematics are divided up into sections with each section aligning to the descriptions of the modules in the hardware notes document. All connections between each section is done with the use of net labels with only minimal components directly connected in each section.

### 7-Segment LED Display
The 7 segment displays have the current limiting resistors directly attached with the arrangement of the displays is the same as that on the physical PCB, left to right. On initial inspection the ordering of the resistor designators will seem odd, instead of running consecutively, that is R1, R2 etc, they are out of order. This is deliberate as a decision was made to have them run consecutively on the printed circuit board (PCB). The values for the resistors are for the Inolux INND-TS56B blue displays. This value will need to be revised if a different LED display if used. Again refer to the hardware notes for further information on how to calculate these values.

### Display Shift Registers
Like the displays, the arrangement of the shift registers is identical in order to that of the PCB. The order of the output pins may however look a little strange. This has been done to simplify the routing of the traces on the PCB. This does make it a little more challenging to associate the correct shift position to the correct segment and a table is used in the firmware that addresses this complexity. For further information refer to the software notes.

### Temperature Sensor
Two variants of the temperature sensor can be used, either the TMP36 or TMP37. Fitment of this component is identical as they share the same pin out. The differences between the components is catered for in the firmware.

## General Notes
All integrated circuits have 100nF decoupling capacitors connected between ground and 5 volts. These are fitted as per the manufacturers recommendations and are placed as close to the supply pin as possible on the PCB.