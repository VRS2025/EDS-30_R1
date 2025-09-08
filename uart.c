/*
 * File:   uart.c
 * Author: romanvinokur
 *
 * Created on May 26, 2024, 9:28 AM
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


void uart1_init(unsigned long baudrate) {
    
    TRISCbits.TRISC7 = 1;  // RX pin (RC7) as input
    TRISCbits.TRISC6 = 0;  // TX pin (RC6) as output    
    
    unsigned long brg =(_XTAL_FREQ / (4 * baudrate)) - 1;

    // Ensure correct setting of the BRG registers
    SPBRG1 = brg & 0xFF;
    SPBRGH1 = (brg >> 8) & 0xFF;
    
    // Set up the transmit and receive control registers
    TXSTA1bits.BRGH = 1;  // High-speed mode
    BAUDCON1bits.BRG16 = 1;  // Use 16-bit baud rate generator
    
    TXSTA1bits.TX9 = 0;  // High-speed mode
    
    TXSTA1bits.SYNC = 0;  // Asynchronous mode
    TXSTA1bits.TXEN = 1;  // Enable transmitter
    
    RCSTA1bits.SPEN = 1;  // Enable serial port
    RCSTA1bits.CREN = 1;  // Enable continuous receive
    

    
    // Enable UART2 receive interrupt
    PIE1bits.RC1IE = 1;
    
    
}

void uart2_init(unsigned long baudrate) {
      
    unsigned long brg = (_XTAL_FREQ / (4 * baudrate)) - 1;

    // Ensure correct setting of the BRG registers
    SPBRG2 = brg & 0xFF;
    SPBRGH2 = (brg >> 8) & 0xFF;
    
    // Set up the transmit and receive control registers
    TXSTA2bits.BRGH = 1;  // High-speed mode
    TXSTA2bits.SYNC = 0;  // Asynchronous mode
    TXSTA2bits.TXEN = 1;  // Enable transmitter
    
    RCSTA2bits.SPEN = 1;  // Enable serial port
    RCSTA2bits.CREN = 1;  // Enable continuous receive
    
    // Set the baud rate control register
    BAUDCON2bits.BRG16 = 1;  // Use 16-bit baud rate generator
    
    // Enable UART2 receive interrupt
    PIE3bits.RC2IE = 0;
    


}