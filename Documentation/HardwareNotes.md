# Introduction
The desktop thermometer consists of four major blocks or modules. The first is a temperature sensor that outputs a voltage that changes with the change in temperature. Second, is the display that consists of 3 digits to display the current temperature. Next is the microcontroller that receives the voltage from the temperature module, converts the voltage to a display sequence that is then sent to the display. Finally, the power supply that provides power to all other modules.

# Temperature sensor module.
In order to measure the temperature a TMP33/36/37 IC from Analog Devices is used, shown as a TMP3x component on the schematic. The reference to the 3x denotes that either a TMP36 or TMP37 can be used.

The difference between the devices is small, the TMP36 has a 10 millivolt per degree change while the TMP37 has a 20 millivolt per degree change. This is important as the microcontroller used can only detect voltage changes with a resolution of 2 millivolts. 

This means that if the TMP36 is fitted to the device then the temperature measured will be to the nearest 1/5th of a degree rather than 1/10th of a degree. The reason that this device can use a TMP36 is because it may be easier to source that the TMP37 even though the resolution is coarser. The only other difference between the two components is that the TMP36 has an initial voltage offset of 0.5 volts.

In addition to the temperature sensing IC, a 100 nanofarad capacitor is used as recommended by the datasheet.

# Display module
The display module can be split into two parts, the actual 7-segment LED display elements and the registers that hold the current values for each of the segments in the display. 

## Display shift registers
The thermometer has a total of 3 displays with 7 pins per display plus an additional pin used for the decimal point bring the total number of pins that we need to control to 22. This is far greater than the 5 pins that are available on the microcontroller chip, in fact we only have 4 pins available for the display as we need one for reading the temperature sensor voltage.

The solution to this problem is to use a serial to parallel shift register to increase the number of output pins to drive the three LED displays, in this case 74HC595 shift register per display.

### How they work
To explain the basic operation of the shift register we will use the game of pass the parcel. The basic rules are simple, we have a line of people, each of which can only hold one parcel at a time and if given another parcel to hold, they must pass the parcel they are currently holding onto the next person in the line. We have two types of parcels that can be passed; one coloured white signifying 'HIGH' or 1 and one coloured black signifying 'LOW' or 0.

Our game begins with 8 people in the line, each holding a black parcel, this is the shift register. We also have an additional person at the start of the line acting as the microcontroller. As microcontroller person passes in either a black or white parcel into the line, the parcels held by those in the line are passed along with the parcel at the end of the line being tossed on the floor. Effectively we are shifting the parcels (or values 1 or 0) along the line. At any point in the game we can record the value and position of each parcel in the line that translate to which person is holding a 1 or 0. This record correlates to the output pins on the shift register. If there is no record made, then the values on the pins do not change regardless of how many new parcels and sent into the line of people.

### Pin Configuration
Looking at the datasheet from Texas Instruments (TI) for the 74HC595 we have supply pins along with a number of input and output pins. The supply pins are self-explanatory, one is for ground and the other is supplied with 5 volts.

#### Output pins
There are 9 output pins, 8 current register pins that stores the current value in the register (the record made of what parcels the 8 people in the line were holding) along with a pin that holds the overflow (the parcel that was just dropped on the floor). The 8 output pins are labelled as QA through QH with the overflow pin labelled QH'. The overflow pin is used to 'feed' another connected shift register further increasing the number of output pins. This is called daisy-chaining.

#### Input pins
We also have five input pins, <span style="text-decoration:overline">OE</span> (Output Enable), <span style="text-decoration:overline">SRCLR</span> (Shift Register Clear), SER (Serial input), SRCLK (Shift Register Clock) and RCLK (Register Clock/Latch). The line over the pin names signifies that this pin is active when the voltage is LOW, also called active low. Each of these pins can be 'pulled' high or low, 5 volts signifying the high and 0 volts or ground signifying low. This is done either by the microcontroller or a direct connection to 5 volts or ground.

<span style="text-decoration:overline">OE</span>, as you would expect, enables the output pins. <span style="text-decoration:overline">SRCLR</span> sets all the values of the output pins to 0 (think of the black parcels held by the line of people in the game).

The SER pin holds the value that will be 'shifted' into the register (the person in our game who is about to pass in the new parcel). The SRCLK pin controls when the value held by the SER pin is actually 'shifted' into the register. This is a rising edge signal which means that when the value of this pin changes from low to high the shift operation is performed (the act of the person actually passing in the parcel to the line of people). 

The final pin, RCLK controls when the internal values of the shift register are sent to the output pins. Again referring back to our games, if this pin is high then the values held by the line of people are recorded.

Of the 5 input pins only one, the <span style="text-decoration:overline">OE</span> pin is hard-wired. This is tied to ground (set active low) meaning that the output of the registers is permanently on.

## 7-Segment LED display
The display is made up of three identical 7-segment LED displays with segment that is used on the display having a current limiting resistor. These are numbered from R1 to R22 and have a value of 4.7 kiloohms. This value is suggested only for the Inolux INND-TS56B display. 

This does not mean that another type or colour of display cannot be used however the value of the current limiting resistors may need to be changed. When choosing a display, it must physically conform to the INND-TS56 package dimensions which is fairly common for the 0.56 digit height 7 segment displays and have a cathode common pin (commonly referred to as 'common cathode').

### Calculating the current limiting resistor value
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

This gives a value of 560 ohms as the smallest resistor that can be used however there is another thing to consider and that is the brightness of the display. 

Remember that this is an informational display and not a light or torch, therefore something that is very bright might just end up burning the retinas out of anyone who looks at it. As such using a resistor that is larger than the calculated value, for example 1.5K ohm might be a better value. This is a subjective decision where breadboarding and experimenting with different values for the current limiting resistor is recommended to assess level of brightness that is comfortable to the eye.

## Microcontroller module
The PIC16F15214 microcontroller forms the heart of the thermometer. It is an 8 pin IC with two pins for power, one for reset and 5 pins available for other uses. Of these remaining 5 pins, one is used for measuring the voltage from the TMP3x temperature sensor and the other 4 pins drive the shift registers which in turn drive the display.

The PIC chip has an internal oscillator which is used for controlling the clock speed of the microcontroller. Whilst using an external crystal would mean the microcontroller would run at a greater speed it is not feasible with this design as two pins are required to connect the crystal to the microcontroller.

Pins 1 and 8 are connected to 5 volts and ground respectively and a 100nF ceramic capacitor between 5 volts and ground is placed as close as possible to pin 1. This acts as a decoupling capacitor and is recommended in the datasheet. The <span style="text-decoration:overline">MCLR</span> or reset pin is 'tied' high via a 10K ohm resistor to ensure the device is not in a reset state. The value of the resistor is again as per the datasheet recommendation.

Pin 7 is used as an analogue input and is connected to the output of the temperature sensor. This pin is internally connected to a 10-bit analogue to digital converter that is set up in the firmware. The remaining pins are defined as digital output pins in the firmware and are connected to the shift registers.

## Device Power
Of all the modules that make up the thermometer, the device power is by far the simplest. In the case of the USB-2 version this is just taken directly from the USB VBUS (5 volt) and GND (ground) pins.

The USB-C version of the thermometer is only slightly more complicated. There are two 5.1K ohm resistors are connected to the CC1 and CC2 pins of the connector as per the USB-C power delivery specification.


