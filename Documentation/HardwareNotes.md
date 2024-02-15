# Hardware.

## Introduction
The desktop thermometer consists of four major blocks or modules. The first is a temperature sensor that outputs a voltage that changes with the change in temperature. Second, is the display that consists of 3 digits to display the current temperature. Next is the microcontroller that receives the voltage from the temperature module, converts the voltage to a display sequence that is then sent to the display. Finally, the power supply that provides power to all other modules.

## Temperature sensor module.
In order to measure the temperature a TMP33/36/37 IC from Analog Devices is used, shown as a TMP3x component on the schematic. The reference to the 3x denotes that either a TMP36 or TMP37 can be used.

The difference between the devices is small, the TMP36 has a 10 millivolt per degree change while the TMP37 has a 20 millivolt per degree change. This is important as the microcontroller used can only detect voltage changes with a resolution of 2 millivolts. 

This means that if the TMP36 is fitted to the device then the temperature measured will be to the nearest 1/5th of a degree rather than 1/10th of a degree. The reason that this device can use a TMP36 is because it may be easier to source that the TMP37 even though the resolution is coarser. The only other difference between the two components is that the TMP36 has an initial voltage offset of 0.5 volts.

In addition to the temperature sensing IC, a 100 nanofarad capacitor is used as recommended by the datasheet.

## Display module
The display module can be split into two parts, the actual 7-segment LED display and the registers that hold the current values for each of the segments in the display. 

### 7-Segment LED display
The display is made up of three identical 7-segment LED displays with segment that is used on the display having a current limiting resistor. These are numbered from R1 to R22 and have a value of 4.7 kiloohms. This value is suggested only for the Inolux INND-TS56B display. 

This does not mean that another type or colour of display cannot be used however the value of the current limiting resistors may need to be changed. When choosing a display, it must physically conform to the INND-TS56 package dimensions which is fairly common for the 0.56 digit height 7 segment displays and have a cathode common pin (commonly referred to as 'common cathode').

#### Calculating the current limiting resistor value
The 74HC595 shift registers serve two purposes, they latch in the display data from the microcontroller and also drive the display elements. The maximum current for the component is 50 milliamps (mA) with the available current that can be used to drive the displays being 40mA to allow for switching of the latches plus some derating.

A single 74HC595 drives a single display and each display has a total of 8 LED elements (7 for the digit and one for the decimal point) the total current available for the display is divided by 8 giving 5mA per element.

Next, the forward voltage for the LED display is required. This will be found in the datasheet for the display being substituted. Typically, this value will be somewhere in the range of 2 to 3.8 volts with the more exotic colours being in the higher range.

The formula for calculating the resistor is as follows:

$$
R = \frac{5 - vf}{5 \cdot 10^{-3}}
$$

Where:
$R$ - The required resistor in ohms
$vf$ - The Forward voltage of the LED display in volts


For example, the substitute display has a forward voltage of 2.2 volts. We substitute this into the above formula as follows:

$$
560 = \frac{5 - 2.2}{5 \cdot 10^{-3}}
$$

This gives a value of 560 ohms the smallest resistor that could be used however there is another thing to consider and that is the brightness of the display. 

Remember that this is an informational display and not a light and therefore something that is very bright might just end up burning the retinas out of anyone who looks at it. Therefore, it might be better to use a resistor that is larger than the calculated value, in this case 2.2K might be a better value.


### Display shift registers
The microcontroller has a limited number of pins which cannot address all elements of the display so there needs to be another device that takes the output from the display and assigns this information to the various LED segments.



## Microcontroller module



## Device Power

