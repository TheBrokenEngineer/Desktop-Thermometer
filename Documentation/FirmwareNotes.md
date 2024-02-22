# Introduction
The firmware for the desktop thermometer is written in C using the MPLAB X IDE from microchip. This IDE along with the compiler is available for free. There are paid versions of the XC-8 compiler however these are not required for this project.

Installation of both the IDE and associated compiler is covered in the documentation on the microchip web site.

* IDE - [MPLAB X](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide)
* PIC Compiler - [MPLAB XC8](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers/xc8)

## Code Structure
All code for the firmware is contained in a single file called `main.c`. This code is heavily commented with not only the descriptions of the various methods but also the variables that need to be set for the configuration of the microcontroller. Where applicable page numbers referring to the specific definition and value are included.

### Display Mapping
As mentioned in the hardware notes, the connection between the shift registers and displays vary between each individual 7-segment display. In order to make the display of each digit consistent across all display units a table is used with the mapping of display element segments and that of actual pinouts. This is heavily commented showing an ASCII diagram of the display unit, the segment numbers and associated mapping.

## Definitions
There are a number of definitions that can be changed or commented out to change the behaviour of the thermometer.