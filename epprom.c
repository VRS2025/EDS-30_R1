/*
 * File:   epprom.c
 * Author: romanvinokur
 *
 * Created on May 18, 2024, 5:55 PM
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

void EEPROM_Write(unsigned char address, unsigned char data) {
    while (EECON1bits.WR);     // Wait for any previous write to complete

    EEADR = address;           // Load the address
    EEDATA = data;             // Load the data to be written
    EECON1bits.EEPGD = 0;      // Point to Data EEPROM memory
    EECON1bits.CFGS = 0;       // Access EEPROM, not Configuration registers
    EECON1bits.WREN = 1;       // Enable writes to EEPROM

    // Required sequence for write to internal EEPROM
    INTCONbits.GIE = 0;        // Disable interrupts
    EECON2 = 0x55;             // Required sequence
    EECON2 = 0xAA;             // Required sequence
    EECON1bits.WR = 1;         // Start EEPROM write
    INTCONbits.GIE = 1;        // Enable interrupts

    while (EECON1bits.WR);     // Wait for write to complete

    EECON1bits.WREN = 0;       // Disable writes to EEPROM
}

unsigned char EEPROM_Read(unsigned char address) {
    EEADR = address;           // Load the address
    EECON1bits.EEPGD = 0;      // Point to Data EEPROM memory
    EECON1bits.CFGS = 0;       // Access EEPROM, not Configuration registers
    EECON1bits.RD = 1;         // Start EEPROM read
    NOP();                     // NOPs may be required for timing
    NOP();

    return EEDATA;             // Return the data read from EEPROM
}

// ?????? 16-??????? ???????? ? EEPROM (2 ?????)
void EEPROM_Write16(unsigned char addr, unsigned int value) {
    EEPROM_Write(addr, value & 0xFF);     
    NOP();                     
    NOP();
    EEPROM_Write(addr + 1, (value >> 8) & 0xFF); 
}

// ?????? 16-??????? ???????? ?? EEPROM (2 ?????)
unsigned int EEPROM_Read16(unsigned char addr) {
    unsigned int val = EEPROM_Read(addr);          
    NOP();                                                                  // NOPs may be required for timing
    NOP();
    val |= ((unsigned int)EEPROM_Read(addr + 1)) << 8; 
    return val;
}


