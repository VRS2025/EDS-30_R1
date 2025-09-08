/*
 * File:   fonts16x16.c
 * Author: romanvinokur
 *
 * Created on May 21, 2024, 11:15 AM
 */

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>   
#include <pic18F46K22.h>
#include "user.h"
#include "logos.h"
#include "ST7565_small.h"
#include "eng_font.h"
#include "font16x16.h"

// Delay function
void delay_ms16x16(unsigned int ms) {
    while (ms--) {
        __delay_ms(1);
    }
}

// Function to send a command to the LCD
void LCD_Command_16x16(uint8_t cmd) {
    LCD_A0 = 0;  // Command mode
    for (uint8_t i = 0; i < 8; i++) {
        LCD_SCL = 0;
        LCD_SI = (cmd >> (7 - i)) & 0x01;
        LCD_SCL = 1;
    }
}

// Function to send data to the LCD
void LCD_Data_16x16(uint8_t data) {
    LCD_A0 = 1;  // Data mode
    for (uint8_t i = 0; i < 8; i++) {
        LCD_SCL = 0;
        LCD_SI = (data >> (7 - i)) & 0x01;
        LCD_SCL = 1;
    }
}

// Function to initialize the LCD
void LCD_Init_16x16(void) {
    LCD_RES = 0;
    delay_ms16x16(50);
    LCD_RES = 1;
    delay_ms16x16(50);

    LCD_Command_16x16(0xA0); // Set SEG direction (0xA0: Normal, 0xA1: Reverse)
    LCD_Command_16x16(0xC0); // Set COM direction (0xC0: Normal, 0xC8: Reverse)
    LCD_Command_16x16(0xA3); // Set LCD bias (0xA2: 1/9 bias, 0xA3: 1/7 bias)
    LCD_Command_16x16(0x40); // Set display start line (0x40 to 0x7F)
    LCD_Command_16x16(0x81); // Set electronic volume (0x00 to 0x3F)
    LCD_Command_16x16(config.contrast); // 0x25 Set contrast control (0x00 to 0x3F)
    LCD_Command_16x16(0x18); // Set internal resistor ratio (0x04 to 0x17)
    LCD_Command_16x16(0x2F); // Set power control (0x20 for power save mode)
    LCD_Command_16x16(0xAF); // Set display enable (0xAF: On, 0xAE: Off)
    LCD_Command_16x16(0xA4); // Set display output (0xA4: Normal, 0xA5: All on)
    LCD_Command_16x16(0xA6); // Set display mode (0xA6: Normal, 0xA7: Invert)
    LCD_Command_16x16(0xA9); // Set 1/64 duty (0xA8: 1/65 duty, 0xA9: 1/64 duty)
    LCD_Command_16x16(0xA0); // Set SEG direction (0xA0: Normal, 0xA1: Reverse)
    LCD_Command_16x16(0xC8); // Set COM direction (0xC0: Normal, 0xC8: Reverse)

}

// Function to clear the LCD
void LCD_Clear_16x16(void) {
    for (uint8_t page = 0; page < 8; page++) {
        LCD_Command_16x16(0xB0 | page);    // Set page address
        LCD_Command_16x16(0x10);           // Set column address high nibble
        LCD_Command_16x16(0x00);           // Set column address low nibble
        for (uint8_t col = 0; col < 128; col++) {
            LCD_Data_16x16(0x00);
        }
    }
}

// Function to set the page address
void LCD_SetPage_16x16(uint8_t page) {
    LCD_Command_16x16(0xB0 | page);
}

// Function to set the column address
void LCD_SetColumn_16x16(uint8_t column) {
    LCD_Command_16x16(0x10 | (column >> 4));  // Set column address high nibble
    LCD_Command_16x16(0x00 | (column & 0x0F));  // Set column address low nibble
}

// Function to draw a 16x16 icon at (x, y)
void LCD_DrawIcon16x16(uint8_t x, uint8_t y, const uint8_t *icon) {
    for (uint8_t i = 0; i < 16; i++) {
        LCD_SetPage_16x16(y + (i / 8));
        LCD_SetColumn_16x16(x);
        LCD_Data_16x16(icon[i * 2]);
        LCD_Data_16x16(icon[i * 2 + 1]);
    }
}



