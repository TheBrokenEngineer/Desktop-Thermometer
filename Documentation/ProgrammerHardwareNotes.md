# Introduction
The desktop thermometer has no programming header pins therefore there is no way of programming or debugging the microcontroller firmware. To address this shortcoming a separate programming device has been created. Depending on the components fitted to the PCB it can function as a programmer, debugger or both.

## Configurations

### Programmer
If the device is only going to be used as a programmer then the board is fitted with a 5-pin header, standard DIP-8 socket and two wire spring terminals for power. The microcontroller to be programmed is inserted into the socket, 5-volt DC power is supplied via the spring terminal connectors and the PICkit-4/MPLAB-Snap programmer plugged into the header. 

The chip is then programmed using the MPLAB X IDE. An additional feature is the reverse voltage protection for the power input terminals which is implemented using a P-Channel MOSFET. In this configuration ALL components in the schematic must be fitted.  

### In-Circuit Debugger
The second configuration is as an in-circuit debugger. This required the fitting of a wire wrap DIP 8 socket and 5-pin header. All other components can be omitted as the power supply for the microcontroller is provided by the desktop thermometer. The board is then plugged directly into the socket on the thermometer board and the thermometer is powered up. Once the PICkit-4/MPLAB-Snap is attached debugging and programming can be performed using the IDE.

### Programmer and In-Circuit Debugger
This configuration populates the board with all components like that of the programmer configuration replacing the standard DIP socket with the wire wrap socket. This allows for the power to be connected to the board and used as just a programmer. Additionally, the board can be plugged into the target thermometer device with the programmer board connected to a 5-volt supply. The advantage is the programmer powers the desktop thermometer removing the need to plug the thermometer into a USB power supply.

## Connecting the Programmer
The pin header has a marking for pin one that corresponds to the programmer pin one. Note that not all pins are used on the programmer as per the recommendation from the programmer manual. Both the PICkit-4 and MPLAB-Snap share identical pinouts.
