/*
 * File:   595.c
 * Author: romanvinokur
 *
 * Created on May 29, 2024, 4:53 PM
 */

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>   
#include <pic18F46K22.h>
#include "user.h"
#include "LCD_ST7565.h"
#include "logos.h"
#include "eng_font.h"
#include "font16x16.h"




//void SendShiftData595(unsigned char data)
//{
//        unsigned char i;
//    for (i = 0; i < 8; i++) {
//        DATA_PIN = (data & 0x80) ? 1 : 0; // Set data pin based on MSB
//        SRCLK_PIN = 1;                    // Pulse the SRCLK
//        __nop();                          // Small delay, if needed
//        SRCLK_PIN = 0;
//        data <<= 1;                       // Shift data left
//    }
//
//    // Pulse the RCLK to latch the data
//    RCLK_PIN = 1;
//    __nop();                              // Small delay, if needed
//    RCLK_PIN = 0;
//    
//};

void SendShiftData595(unsigned int data)
{
    unsigned char i;

    // Shift data into both shift registers
    for (i = 0; i < 16; i++) {
        // Send MSB of data to the data pin
        DATA_PIN = (data & 0x8000) ? 1 : 0; // Check MSB of the 16-bit data
        SRCLK_PIN = 1;                    // Pulse the SRCLK
        __nop();                          // Small delay, if needed
        SRCLK_PIN = 0;
        data <<= 1;                      // Shift data left to process the next bit
    }

    // Pulse the RCLK to latch the data
    RCLK_PIN = 1;
    __nop();                              // Small delay, if needed
    RCLK_PIN = 0;
}

//void updateBit(unsigned int *data, unsigned int bitPosition, char value) {
//    unsigned int mask = 1 << bitPosition;
//
//    if (value) {
//        *data |= mask;    // Set the bit
//    } else {
//        *data &= ~mask;   // Clear the bit
//    }
//}
//



