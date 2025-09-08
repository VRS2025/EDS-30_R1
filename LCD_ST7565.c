#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>   
#include <pic18F46K22.h>
#include "user.h"
#include "logos.h"
#include "LCD_ST7565.h"
#include <stdbool.h>

// Constants
#define COMMAND_MODE 0
#define DATA_MODE 1
#define MAX_X 128
#define MAX_Y 8

uint8_t framebuffer[128][8]; // 128 columns (width), 8 pages (64 rows / 8 bits per page)



// LCD Initialization
void LCDInit(void) {
    LCD_RES = 0; // Reset the LCD
    __delay_ms(50); // Wait for reset
    LCD_RES = 1; // Release reset
    __delay_ms(50); // Wait for reset release
    
    // Display initialization sequence
    const uint8_t initCommands[] = {
        0xA0, 0xC0, 0xA3, 0x40, 0x1E, 
        0x18, 0x2F, 0xAF, 0xA4, 0xA6, 0xA9,
        0xA0, 0xC8
    };

    for (uint8_t i = 0; i < sizeof(initCommands); i++) {
        LCD_Command(initCommands[i]);
    }
    
    lcd_clear();
}

// Send Command to LCD
void LCD_Command(uint8_t command) {
    LCD_A0 = COMMAND_MODE; // Command mode
    for (uint8_t i = 0; i < 8; i++) {
        LCD_SCL = 0; // Clock low
        LCD_SI = (command & 0x80) ? 1 : 0; // Send MSB
        command <<= 1; // Shift left
        LCD_SCL = 1; // Clock high
    }
}

// Send Data to LCD
void LCD_Data(uint8_t data) {
    LCD_A0 = DATA_MODE; // Data mode
    for (uint8_t i = 0; i < 8; i++) {
        LCD_SCL = 0; // Clock low
        LCD_SI = (data & 0x80) ? 1 : 0; // Send MSB
        data <<= 1; // Shift left
        LCD_SCL = 1; // Clock high
    }
}

// Set cursor position
void lcd_gotoxy(uint8_t x, uint8_t y) {
    LCD_Command(0xB0 | y); // Set page
    LCD_Command(0x10 | (x >> 4)); // Column high
    LCD_Command(0x00 | (x & 0x0F)); // Column low
}

// Clear the entire LCD
void lcd_clear(void) {
    for (uint8_t i = 0; i < MAX_Y; i++) {
        lcd_gotoxy(0, i);
        for (uint8_t j = 0; j < MAX_X; j++) {
            LCD_Data(0x00); // Clear pixel
        }
    }
}

// Clear a specific line
void lcd_clear_line(uint8_t y) {
    lcd_gotoxy(0, y);
    for (uint8_t k = 0; k < MAX_X; k++) {
        LCD_Data(0x00); // Clear line
    }
}

void lcd_clear_pixels(unsigned char start_pixel, unsigned char end_pixel, unsigned char y) {
    lcd_gotoxy(start_pixel, y); // Set the cursor to the desired line
    for (unsigned char i = start_pixel; i <= end_pixel; i++) {
        LCD_Data(0x00); // Clear pixel
    }
}

void lcd_putch(uint8_t c, uint8_t invert) { // Change char to uint8_t
    if (c < 32 || c > 'z') return; // Invalid character
    for (uint8_t i = 0; i < 5; i++) {
        LCD_Data(invert ? ~lcd_font[c - 32][i] : lcd_font[c - 32][i]);
    }
    LCD_Data(invert ? 0xFF : 0x00); // Clear space
}

// Display a string
void lcd_putstr(const char *s, uint8_t invert) {
    while (*s) {
        lcd_putch(*s++, invert);
    }
}

// Display a digit
void display_digit(uint8_t x, uint8_t y, uint8_t digit, uint8_t inv) {
    if (y < 1 || y > 7 || x >= MAX_X || digit > 0x11) return;
    lcd_gotoxy(x, y - 1);
    for (uint8_t i = 0; i < 8; i++) {
        LCD_Data(inv ? (uint8_t)~digits_8x16[(digit << 4) + i] : (uint8_t)digits_8x16[(digit << 4) + i]);
    }
    lcd_gotoxy(x, y);
    for (uint8_t i = 8; i < 16; i++) {
        LCD_Data(inv ? (uint8_t)~digits_8x16[(digit << 4) + i] : (uint8_t)digits_8x16[(digit << 4) + i]);
    }
}

// Display an image
void img_view(uint8_t x, uint8_t y, uint8_t size, const uint8_t pic[], uint8_t inv) {
    lcd_gotoxy(x, y);
    for (uint8_t j = 0; j < size; j++) {
        LCD_Data(inv ? ~pic[j] : pic[j]);
    }
}



// Display a picture
void Display_Picture(const uint8_t pic[]) {
    for (uint8_t i = 0; i < MAX_Y; i++) {
        lcd_gotoxy(0, i);
        for (uint8_t j = 0; j < MAX_X; j++) {
            LCD_Data(pic[i * MAX_X + j]);
        }
    }
}



void rs_icon(void) {
    const uint8_t iconData[] = {
        0b01110000, 0b01010000, 0b00100000,
        0b00100111, 0b00111101, 0b00100111,
        0b00100000, 0b01010000, 0b01110000
    };
    for (uint8_t i = 0; i < sizeof(iconData); i++) {
        LCD_Data(iconData[i]);
    }
}

//void lcd_draw_icon(uint8_t x, uint8_t y, const uint8_t *icon) {
//    lcd_gotoxy(x, y); // Set cursor to starting position
//    for (uint8_t i = 0; i < 13; i++) { // Loop through each row of the icon
//        LCD_Data(icon[i]); // Send each row data to the LCD
//    }
//}

//void lcd_draw_icon_13x8(uint8_t x, uint8_t page, const uint8_t *icon_data, uint8_t width) {
//    lcd_gotoxy(x, page); // Set starting column (x) and page (y)
//    for (uint8_t i = 0; i < width; i++) {
//        LCD_Data(icon_data[i]); // Write each column of icon
//    }
//}


//// Function to load the logo into custom characters
//void load_logo(void) {
//    uint8_t char_index = 0;
//    for (uint8_t i = 0; i < 8; i++) {  // Iterate through 8 custom characters
//        uint8_t logo_char[8];
//        for (uint8_t j = 0; j < 8; j++) {
//            logo_char[j] = logo[char_index++];  // Get the logo data for each character
//        }
//        LCD_CreateChar(i, logo_char);  // Create custom character
//    }
//}
//
//// Function to create custom character
//void LCD_CreateChar(uint8_t location, uint8_t *charmap) {
//    location &= 0x07; // Only 8 locations available for custom characters
//    LCD_Command(0x40 | (location * 8)); // Set CGRAM address
//    for (uint8_t i = 0; i < 8; i++) {
//        LCD_Data(charmap[i]);  // Write custom character data
//    }
//}
//
//// Display logo on the screen
//void display_logo(void) {
//    for (uint8_t i = 0; i < 8; i++) {  // Iterate through the custom characters
//        lcd_gotoxy(i, 0);  // Set cursor to the first row
//        LCD_Data(i);       // Display the custom character
//    }
//}



//void LCDInit(void) {
//    LCD_RES = 0; // Reset the LCD
//    __delay_ms(50); // Wait for reset
//    LCD_RES = 1; // Release reset
//    __delay_ms(50); // Wait for reset release
//    
//         // Display initialization sequence
//    LCD_Command(0xA0); // Set SEG direction (0xA0: Normal, 0xA1: Reverse)
//    LCD_Command(0xC0); // Set COM direction (0xC0: Normal, 0xC8: Reverse)
//    LCD_Command(0xA3); // Set LCD bias (0xA2: 1/9 bias, 0xA3: 1/7 bias)
//    LCD_Command(0x40); // Set display start line (0x40 to 0x7F)
//    LCD_Command(0x81); // Set electronic volume (0x00 to 0x3F)
//   // LCD_Command(config.contrast); // 0x25 Set contrast control (0x00 to 0x3F) 0x1E
//    LCD_Command(0x1E); // 0x25 Set contrast control (0x00 to 0x3F) 0x1E
//    LCD_Command(0x18); // Set internal resistor ratio (0x04 to 0x17)
//    LCD_Command(0x2F); // Set power control (0x20 for power save mode)
//    LCD_Command(0xAF); // Set display enable (0xAF: On, 0xAE: Off)
//    LCD_Command(0xA4); // Set display output (0xA4: Normal, 0xA5: All on)
//    LCD_Command(0xA6); // Set display mode (0xA6: Normal, 0xA7: Invert)
//    LCD_Command(0xA9); // Set 1/64 duty (0xA8: 1/65 duty, 0xA9: 1/64 duty)
//    LCD_Command(0xA0); // Set SEG direction (0xA0: Normal, 0xA1: Reverse)
//    LCD_Command(0xC8); // Set COM direction (0xC0: Normal, 0xC8: Reverse)
//    lcd_clear();
//
////    LCD_Command(0xAE); // Display OFF
////    LCD_Command(0xA2); // Set bias ratio
////    LCD_Command(0xA0); // Set segment direction
////    LCD_Command(0xC8); // Set common output direction
////    LCD_Command(0x2F); // Set power control
////    LCD_Command(0x21); // Set resistor ratio
////    LCD_Command(0x81); // Set electronic volume
////    LCD_Command(0x3F); // Electronic volume value
////    LCD_Command(0xAF); // Display ON
////    //lcd_clear();
//}
//
//void LCD_Command(unsigned char command){
//    
//        LCD_A0 = 0; // Command mode
//    for (char i = 0; i < 8; i++) {
//        LCD_SCL = 0;
//        LCD_SI = (command & 0x80) ? 1 : 0;
//        command <<= 1;
//        LCD_SCL = 1;
//    } 
//};
//
//
//void LCD_Data(unsigned char data){
//    
//        LCD_A0 = 1; // Data mode
//    for (char i = 0; i < 8; i++) {
//        LCD_SCL = 0;
//        LCD_SI = (data & 0x80) ? 1 : 0;
//        data <<= 1;
//        LCD_SCL = 1;
//    }
//};
//
////void LCD_WriteByte(uint8_t byte) {
////    for (int i = 0; i < 8; i++) {
////        LCD_SCL = 0;  // Clock low
////        if (byte & 0x80) {
////            LCD_SI = 1;  // Data bit high
////        } else {
////            LCD_SI = 0;  // Data bit low
////        }
////        byte <<= 1;  // Shift left
////        LCD_SCL = 1;  // Clock high
////    }
////}
//
//void lcd_gotoxy(unsigned char x, unsigned char y) {
//    LCD_Command(0xB0 | y); // Page(Row)
//    LCD_Command(0x10 | ((x + 4) >> 4)); // Column high
//    LCD_Command((0x00 | (x + 4)) & 0x0f); // Column low
//}
//
//void Set_Page_Address(unsigned char y){
//    y=0xb0|y;
//    LCD_Command(y);
//	return;
//};
//
//void Set_Column_Address(unsigned char add){
//    LCD_Command((0x10|(add>>4)));
//	LCD_Command((0x0f&add));
//	return;
//};
//
//void Initial_Dispay_Line(unsigned char line) {
//    line |= 0x40;
//    LCD_Command(line);
//}
//
//void lcd_clear(void) {
//    unsigned char i, j;
//    for (i = 0; i < 9; i++) {
//        LCD_Command(0xb0 + i);
//        LCD_Command(0x10);
//        LCD_Command(0x00);
//        for (j = 0; j < 128; j++) {
//            LCD_Data(0x00);
//        }
//    }
//}
//
////void Clear_img_Line(uint8_t line) {
////    // Clear the specific line on the LCD
////    img_view(20, line, 89, NULL, 0);  // Assuming this clears the line
////}
//
//void lcd_clear_line(unsigned char y) {
//    unsigned char k;
//    for (k = 0; k < 128; ++k) {
//        lcd_gotoxy(k, y);
//        LCD_Data(0x00);
//    }
//}
//
//
//void lcd_clear_pixels(unsigned char start_pixel, unsigned char end_pixel, unsigned char y) {
//    unsigned char x = 0;
//    for (x = start_pixel; x < end_pixel; x++) {
//        lcd_gotoxy(x, y);
//        LCD_Data(0x00);
//    }
//}
//
//void lcd_putch(unsigned char c, unsigned char invert) {
//    unsigned char i = 0;
//    if (c > 'z' || c < 32) {
//        return;
//    }
//    for (i = 0; i < 5; i++) {
//        if (invert) {
//            LCD_Data(~lcd_font[c - 32][i]);
//        } else {
//            LCD_Data(lcd_font[c - 32][i]);
//        }
//    }
//    if (invert) {
//        LCD_Data((unsigned char)~0x00);
//    } else {
//        LCD_Data(0x00);
//    }
//}
//
//void lcd_putstr(const char *s, unsigned char invert) {
//    while (*s) {
//        lcd_putch(*s, invert);
//        s++;
//    }
//}
//
//void display_digit(unsigned char x, unsigned char y, unsigned char digit, char inv) {
//    unsigned char i;
//    if (y < 1 || y > 7) return;
//    if (x > 128) return;
//    if (digit > 0x11) return;
//    if (inv) {
//        lcd_gotoxy(x, y - 1);
//        for (i = 0; i < 8; i++) LCD_Data((unsigned char)~digits_8x16[(digit << 4) + i]);
//        lcd_gotoxy(x, y);
//        for (i = 8; i < 16; i++) LCD_Data((unsigned char)~digits_8x16[(digit << 4) + i]);
//    } else {
//        lcd_gotoxy(x, y - 1);
//        for (i = 0; i < 8; i++) LCD_Data((unsigned char)digits_8x16[(digit << 4) + i]);
//        lcd_gotoxy(x, y);
//        for (i = 8; i < 16; i++) LCD_Data((unsigned char)digits_8x16[(digit << 4) + i]);
//    }
//}
//
//void img_view(unsigned char x, unsigned char y, unsigned char size, const unsigned char pic[], unsigned char inv) {
//    unsigned char j;
//    Initial_Dispay_Line(0x40);
//    Set_Page_Address(y);
//	 Set_Column_Address(x);
//    for (j = 0; j < size; j++) {
//        if (inv == 0) {
//            LCD_Data(pic[j]);
//        } else {
//            LCD_Data(~pic[j]);
//        }
//    }
//}
//
//void Display_Picture(const unsigned char pic[]) {
//    unsigned char i, j;
//    Initial_Dispay_Line(0x40);
//    for (i = 0; i < 0x08; i++) {
//        Set_Page_Address(i);
//    Set_Column_Address(0x00);
//        for (j = 0; j < 0x80; j++) {
//            LCD_Data(pic[i * 0x80 + j]);
//        }
//    }
//}
//
//void rs_icon(unsigned char i) {
//
//    LCD_Data(0b01110000);
//    LCD_Data(0b01010000);
//    LCD_Data(0b00100000);
//    LCD_Data(0b00100111);
//    LCD_Data(0b00111101);
//    LCD_Data(0b00100111);
//    LCD_Data(0b00100000);
//    LCD_Data(0b01010000);
//    LCD_Data(0b01110000);
//
//}
//





