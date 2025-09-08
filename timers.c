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

void init_TMR0(unsigned char en) {

    // Configure Timer0
    T0CONbits.T08BIT = 0;   // Set Timer0 to 16-bit mode
    T0CONbits.T0CS = 0;     // Select internal instruction cycle clock (FOSC/4)
    T0CONbits.PSA = 0;      // Enable prescaler
    T0CONbits.T0PS = 0b111; // Set prescaler to 1:256
    
    // Load Timer0 value for 10ms interrupt
    TMR0 = 65536 - ((16000000 / 4) / 256 / 100); // Assuming 16MHz FOSC

    // Enable Timer0 interrupt
    INTCONbits.TMR0IE = 1;   // Enable Timer0 interrupt
    INTCONbits.TMR0IF = 0;   // Clear Timer0 interrupt flag
    INTCONbits.PEIE = 0;     // Enable peripheral interrupt
    INTCONbits.GIE = 0;      // Enable global interrupts

    // Start Timer0
    T0CONbits.TMR0ON = en;    // Turn on Timer0
}




void init_TMR5(unsigned char h ,unsigned char EN,unsigned char int_en){          // 1mSec
    
    T5CONbits.TMR5ON = 0;    // Turn off Timer5 for configuration
    T5CONbits.T5CKPS = 0b11; // 1:8 prescaler value
    T5CONbits.T5RD16 = 1;     // 16-bit operation mode
    
    // Set the initial value for a 1ms interrupt
    // (Note: The exact value may need adjustment based on the oscillator frequency)
    TMR5H = 0xfe; // 0xFC; // High byte
    TMR5L = 0x18; // 0x18; // Low byte
    
    T5CONbits.TMR5ON = EN;    // Turn on Timer5
    PIE5bits.TMR5IE = int_en;     // Enable Timer5 interrupt
          
}


