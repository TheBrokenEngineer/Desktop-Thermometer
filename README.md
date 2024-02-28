# Desktop-Thermometer

A small desktop thermometer that just sits there and works. It acquires the ambient air temperature and shows the current temperature on 3 7-segment LED displays.

Designed to be low cost (under USD20) and be powered by a computer USB port, USB wall wart or power bank.

At the heart is a Microchip PIC16 microcontroller that reads the voltage produced by the sense pin on the TMP36/TMP37 sensor and shifts the converted value out to the display using 74HC595 shift registers. For full details of the device please refer to the documentation contained in the Documentation directory.