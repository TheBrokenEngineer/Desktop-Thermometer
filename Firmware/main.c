/*
* Desktop Thermometer Firmware
* Copyright (C) 2024 The Broken Engineer.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// PIC16F15214 Configuration Bit Settings
// Configuration 1 (CONFIG1) {Page 23}
#pragma config FEXTOSC = OFF            // External Oscillator Mode Selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_32MHZ  // Power-up Default Value for COSC bits (HFINTOSC (32 MHz))
#pragma config CLKOUTEN = OFF           // Clock Out Enable bit (CLKOUT function is disabled; I/O function on RA4)
#pragma config VDDAR = HI               // VDD Range Analog Calibration Selection bit (Internal analog systems are calibrated for operation between VDD = 2.3V - 5.5V)
// Configuration 2 (CONFIG2) {Page 24, 25}
#pragma config MCLRE = EXTMCLR          // Master Clear Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RA3 pin function is MCLR)
#pragma config PWRTS = PWRT_OFF         // Power-up Timer Selection bits (PWRT is disabled)
#pragma config WDTE = OFF               // WDT Operating Mode bits (WDT disabled; SEN is ignored)
#pragma config BOREN = OFF              // Brown-out Reset Enable bits (Brown-out Reset disabled)
#pragma config BORV = LO                // Brown-out Reset Voltage Selection bit (Brown-out Reset Voltage (VBOR) set to 1.9V)
#pragma config PPS1WAY = ON             // PPSLOCKED One-Way Set Enable bit (The PPSLOCKED bit can be set once after an unlocking sequence is executed; once PPSLOCKED is set, all future changes to PPS registers are prevented)
#pragma config STVREN = OFF             // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will not cause a reset)
// Configuration 4 (CONFIG4) {Page 27, 28}
#pragma config BBSIZE = BB512           // Boot Block Size Selection bits (512 words boot block size)
#pragma config BBEN = OFF               // Boot Block Enable bit (Boot Block is disabled)
#pragma config SAFEN = OFF              // SAF Enable bit (SAF is disabled)
#pragma config WRTAPP = OFF             // Application Block Write Protection bit (Application Block is not write-protected)
#pragma config WRTB = OFF               // Boot Block Write Protection bit (Boot Block is not write-protected)
#pragma config WRTC = OFF               // Configuration Registers Write Protection bit (Configuration Registers are not write-protected)
#pragma config WRTSAF = OFF             // Storage Area Flash (SAF) Write Protection bit (SAF is not write-protected)
#pragma config LVP = ON                 // Low Voltage Programming Enable bit (Low Voltage programming enabled. MCLR/Vpp pin function is MCLR. MCLRE Configuration bit is ignored.)
// Configuration 5 (CONFIG5) {Page 29}
#pragma config CP = OFF                 // User Program Flash Memory Code Protection bit (User Program Flash Memory code protection is disabled)

// The frequency we have assigned is 8MHz
#define _XTAL_FREQ 8000000 // Used for the delay functions

/**
 * The shift register clock hold time (Pulse duration - tw). This is specified in the 75HC595 datasheet (page 6).
 */
#define CLK_HOLD_TIME 40 // in uS, we add a little to the value to be safe from the datasheet

/**
 * The number of ADC conversions to sample and average.
 */
#define SAMPLE_COUNT 10

/**
 * The amount of time between display reporting updates
 */
#define REPORT_WAIT_SEC 30

// PIC Libraries
#include <xc.h>

// C Libraries
#include <math.h>
#include <stdbool.h>


//
// Display segments are as follows:
//
//      AAAAAA
//      F    B
//      F    B
//      F    B
//      GGGGGG
//      E    C
//      E    C
//      E    C
//      DDDDDD
//
// Therefore each display digit has the following segments:
//
//      Value | Segments
//      --------------------------
//         0  | A,B,C,D,E,F
//         1  | B,C
//         2  | A,B,D,E,G
//         3  | A,B,C,D,G
//         4  | B,C,F,G
//         5  | A,C,D,F,G
//         6  | A,C,D,E,F,G
//         7  | A,B,C
//         8  | A,B,C,D,E,F,G
//         9  | A,B,C,D,F,G
//
// The map value are as follows with the digits running left to right on the display:
//
//      Digit 1 Segment | Digit 2 Segment | Digit 3 Segment | Offset
//      ------------------------------------------------------------
//             C        |        B        |        A        |   0
//             D        |        DP       |        G        |   1
//             E        |        A        |        F        |   2
//             G        |        C        |        B        |   3
//             F        |        D        |        C        |   4
//             A        |        F        |        D        |   5
//             B        |        E        |        E        |   6
//             N/A      |        G        |        N/A      |   7
//
// The array is for three digits in the order [Digit3, Digit 2, Digit 1][0,1,2,3,4,5,6,7,8,9]
static const uint8_t digit_array[3][10] = {
    {0b10111110, 0b00011000, 0b11010110, 0b11011100, 0b01111000, 0b11101100, 0b11101110, 0b10011000, 0b11111110, 0b11111100}, // Digit 3 - 10ths
    {0b10111110, 0b10010000, 0b10101011, 0b10111001, 0b10010101, 0b00111101, 0b00111111, 0b10110000, 0b10111111, 0b10111101}, // Digit 2 - 1s
    {0b11101110, 0b10000010, 0b01110110, 0b11010110, 0b10011010, 0b11011100, 0b11111100, 0b10000110, 0b11111110, 0b11011110} // Digit 1 - 10s
};

/**
 * Contains all MCU initialization including clock speeds etc.
 */
void mcu_init() {
    OSCFRQbits.FRQ = 0b011; // 8MHz
}

/**
 * Contains the configuration for the digital I/O Ports.
 * 
 * For the thermometer we use the following:
 *      RA1 - RCLK Send the stored register value to the output pins (LED display).
 *      RA2 - SRCLR Clears the register stored value.
 *      RA4 - SRCLK The shift clock.
 *      RA5 - DATA The bit value that will be shifted into the register.
 */
void port_init() {
    ANSELA = 0;
    // Enable the pins as digital {Page 125}
    ANSELAbits.ANSA1 = 0;
    ANSELAbits.ANSA2 = 0;
    ANSELAbits.ANSA4 = 0;
    ANSELAbits.ANSA5 = 0;
    // Set the pins as output  {Page 125}
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;
    // Set the output all low (This will also reset the internal register buffers to 0) {Page 124}
    LATAbits.LATA1 = 0;
    LATAbits.LATA2 = 0;
    LATAbits.LATA4 = 0;
    LATAbits.LATA5 = 0;
}

/**
 * Contains the initialization for the analog to digital (ADC) conversion.
 * 
 * For the thermometer we use the following:
 *      RA0 - TEMP Connected to the sense pin on the TMP36.
 * 
 */
void adc_init() {
    // Set the pin as input {Page 125}
    TRISAbits.TRISA0 = 1;
    // Enable the input buffer  {Page 125}
    ANSELAbits.ANSA0 = 1;
    // Set up the fixed voltage reference.
    FVRCONbits.ADFVR = 0b10; // Set up an 2x gain or 2.048v We are assuming that we are running off 5 volts {Page 310}
    FVRCONbits.FVREN = 1; // Enable the fixed voltage reference {Page 310}
    // Set up the ADC parameters
    ADCON0bits.CHS = 0b000000; // Set up RA0 as our ADC port {Page 322}
    ADCON1bits.FM = 0b1; // Right justified as we want the entire value {Page 323}
    ADCON1bits.PREF = 0b11; // Fix the reference voltage to the internal reference {Page 323}
    ADCON1bits.CS = 0b110; // Set the divisor to a large value to get a better reading, in this case it is 64 {Page 314, 315, 323}
    // Ensure interrupts do not fire
    ADACTbits.ACT = 0b000;
    // Enable the ADC {Page 322}
    ADCON0bits.ON = 0b1;
    // Wait for the ADC to settle
    __delay_us(10); // 10uS
}

/**
 * Performs a number of ADC cycles specified by the SAMPLE_COUNT then averages them.
 * @return The sample reference voltage in millivolts.
 */
int adc_read() {
    long capture = 0;
    // Get the required number of samples
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        // Start a conversion cycle. {Page 322}
        ADCON0bits.GO = 1;
        while (ADCON0bits.GO); // Wait for it to complete, this bit will go to 0 (false) when done.
        capture += (ADRESH << 8) + ADRESL; // Get the result of the conversion {Page 325}
        // Wait for the ADC to settle
        __delay_us(500); // 500uS
    }
    // Calculate the average value then apply the scale from our voltage reference.
    // Calculate the return voltage by taking the value, dividing by the resolution (10 bits - 2^10) then multiplying by the reference voltage
    int voltage = (int) ((capture / SAMPLE_COUNT) * (2048 / 1024));
    return voltage;
}

/**
 * Sends the value out to the shift registers
 * @param value The binary value to be sent.
 */
void shift_value(const uint8_t value) {

    // Loop through the data
    for (int8_t index = 0; index < 8; index++) {
        // Test the bit and if high set it otherwise not
        if (value & (1 << index)) {
            LATAbits.LATA5 = 1;
        }
        else {
            LATAbits.LATA5 = 0;
        }
        LATAbits.LATA4 = 1;
        __delay_us(CLK_HOLD_TIME);
        LATAbits.LATA4 = 0;
    }
}

/**
 * Formats the binary value to one that is valid for display on the 7-segment display.
 * @param value The value to be sent to the shift register.
 * @param enable_decimal_point Enable the decimal point on the LED display.
 * @return 
 */
uint8_t process_digit_value(const int index, const int value, bool enable_decimal_point) {
    uint8_t display_value = digit_array[index][value];
    if (enable_decimal_point) {
        display_value |= 0b01000000; // Note this implies digit 2!!!!
    }
    return display_value;
}

void send_value(const int tens, const int ones, const int tenths) {
    // Ensure that the clear is set high for the registers
    LATAbits.LATA2 = 1;

    // We grab the bit value for each parameter and shift this to the display registers
    // If we have an invalid value then return as we can nothing with it!
    uint8_t tens_display = process_digit_value(2, tens, false);
    uint8_t ones_display = process_digit_value(1, ones, true);
    uint8_t tenths_display = process_digit_value(0, tenths, false);
    // Verify
    if (tens_display == 0 && ones_display == 0 && tenths_display == 0) {
        return;
    }

    // The order is important, tenths, ones then tens.
    shift_value(tenths_display);
    shift_value(ones_display);
    shift_value(tens_display);
    // We have the value in the registers ready for display, click them into the output stage now.
    __delay_us(CLK_HOLD_TIME);
    LATAbits.LATA1 = 1;
    __delay_us(CLK_HOLD_TIME);
    LATAbits.LATA1 = 0;
}

/**
 * Take the result from the ADC, and performs the following:
 *  - Converts the voltage to a temperature value Refer TMP36 data sheet Page 1.
 *  - Splits the temperature value into separate values for the 10s, 1s and 10ths
 *  - Display each value on the appropriate 7-segment display
 * @param voltage The raw voltage value received from the ADC. This value is in millivolts
 */
void process_result(int voltage) {

    // The voltage of the TMP36 is calibrated as 750mV @ 25 degrees C with 10mV per degree.
    // we need to set this value with the correct offset to 0.
    int temperature = voltage - 500;

    // Break down into the single digits, we assume that this value will be below 100
    // since 100c is way too hot for humans to survive!
    int tens = (temperature / 100) % 10;
    int ones = (temperature / 10) % 10;
    int tenths = temperature % 10;

    // We have all the values, next load each value into the appropriate register ready for the display.
    send_value(tens, ones, tenths);

}

/**
 * Mainline
 */
void main(void) {

    mcu_init();
    port_init();
    adc_init();

    // Main processing loop
    while (true) {
                int voltage = adc_read();
                process_result(voltage);
                // There is no such thing as waiting for seconds so we do this manually. This allows for us to set the delay to be a very large value.
                for (int secondCount = 0; secondCount < REPORT_WAIT_SEC; secondCount++) {
                    __delay_ms(1000);
                }
    }
    return;
}
