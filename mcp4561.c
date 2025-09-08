/*
 * File:   mcp4561.c
 * Author: romanvinokur
 *
 * Created on August 31, 2024, 10:20 PM
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
#include "i2c1.h"
#include "mcp4561.h"

//void MCP4561_WriteWiper(uint8_t i2cAddress, uint8_t wiperValue)
//{
//    I2CStart(); // Start condition
//    I2CSend((unsigned char)(i2cAddress << 1)); // Send device address with write bit (0)
//    I2CSend(MCP4561_WRITE_CMD | MCP4561_VOLATILE_WIPER_0); // Send command to write to wiper register 0
//    I2CSend(wiperValue); // Send the wiper value
//    I2CStop(); // Stop condition
//}


//uint8_t MCP4561_ReadWiper(uint8_t i2cAddress)
//{
//    uint8_t readValue;
//    
//    I2CStart(); // Start condition
//    I2CSend((unsigned char)(i2cAddress << 1)); // Send device address with write bit (0)
//    I2CSend(MCP4561_READ_CMD | MCP4561_VOLATILE_WIPER_0); // Send command to read from wiper register 0
//    
//    I2CRestart(); // Send repeated start
//    I2CSend((unsigned char)(i2cAddress  << 1) | 1); // Send device address with read bit (1)
//    readValue = I2CRead(); // Read the wiper value
//    I2CNak(); // Send NAK after the last byte
//    I2CStop(); // Stop condition
//    
//    return readValue; // Return the read value
//}



void SetControlRegister(unsigned char potAddress, unsigned char value)
{
    MCP4561_Write(potAddress, 0x04, value); // Control Register
}




void MCP4561_Write(unsigned char address, unsigned char reg, unsigned char data)
{
    I2CStart();
    I2CSend((unsigned char)(address << 1));  // Send address with write bit (0)
    I2CSend(reg); // Send register address
    I2CSend(data); // Send data
    I2CStop();
}




void SetWiperValue(unsigned char potAddress, unsigned char wiperNumber, unsigned char value)
{
    // Ensure the wiper number is valid (0 or 1)
    if (wiperNumber > 1)
        return;

    unsigned char reg = (wiperNumber == 0) ? 0x00 : 0x01; // Wiper 0 or Wiper 1 register
    MCP4561_Write(potAddress, reg, value);
}



float ConvertVolumeToDB(unsigned char volume) {
    float normalizedValue = (255 - volume) / 255.0f;  // Normalize to 0-255 then invert
    float dbValue = (float)(MIN_DB + normalizedValue * (MAX_DB - MIN_DB));  // Explicitly cast to float
    
        // Clamp the dB value to ensure it stays within the range
    if (dbValue < MIN_DB) {
        dbValue = MIN_DB;
    } else if (dbValue > MAX_DB) {
        dbValue = MAX_DB;
    }
    
    return dbValue;
  
}



//unsigned char MCP4561_Read(unsigned char address, unsigned char reg)
//{
//    unsigned char data;
//    
//    I2CStart();
//    I2CSend((unsigned char)(address << 1));  // Send address with write bit (0)
//    I2CSend(reg); // Send register address
//    
//    I2CRestart();
//    I2CSend((unsigned char)(address << 1) | 0x01); // Send address with read bit (1)
//    
//    data = I2CRead(); // Read data from the device
//    
//    I2CNak(); // Send NACK after reading the data
//    I2CStop();
//    
//    return data;
//}

//unsigned char GetWiperValue(unsigned char potAddress, unsigned char wiperNumber)
//{
//    // Ensure the wiper number is valid (0 or 1)
//    if (wiperNumber > 1)
//        return 0xFF; // Return error value
//
//    unsigned char reg = (wiperNumber == 0) ? 0x00 : 0x01; // Wiper 0 or Wiper 1 register
//    return MCP4561_Read(potAddress, reg);
//}
