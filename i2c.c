///*
// * File:   i2c.c
// * Author: romanvinokur
// *
// * Created on July 20, 2024, 10:01 PM
// */
//
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


void I2CInit(void)
{
    /* CKE disabled; SMP Standard Speed;  */
    SSP1STAT = 0x80;
    /* SSPM FOSC/4_SSPxADD_I2C; CKP disabled; SSPEN disabled; SSPOV no_overflow; WCOL no_collision;  */
    SSP1CON1 = 0x8;
    /* SEN disabled; RSEN disabled; PEN disabled; RCEN disabled; ACKEN disabled; ACKDT acknowledge; GCEN disabled;  */
    SSP1CON2 = 0x0;
    /* DHEN disabled; AHEN disabled; SBCDE disabled; SDAHT 100ns; BOEN disabled; SCIE disabled; PCIE disabled;  */
    SSP1CON3 = 0x0;
    /* SSPADD 39;  */
    SSP1ADD = 0x27;
//    I2C1_InterruptsEnable();
//    I2C1_CallbackRegister(I2C1_DefaultCallback);
    SSP1CON1bits.SSPEN = 1;
}



void I2CStart(void)
{
    SSP1CON2bits.SEN = 1; // Initiate Start condition
    while (SSP1CON2bits.SEN); // Wait until Start condition is completed
}

/*
Function: I2CStop
Description: Send a stop condition on I2C Bus
*/
void I2CStop(void)
{
    SSP1CON2bits.PEN = 1; // Initiate Stop condition
    while (SSP1CON2bits.PEN); // Wait until Stop condition is completed
}

/*
Function: I2CRestart
Description: Sends a repeated start condition on I2C Bus
*/
//void I2CRestart(void)
//{
//    SSP1CON2bits.RSEN = 1; // Initiate Repeated Start condition
//    while (SSP1CON2bits.RSEN); // Wait until Repeated Start condition is completed
//}

/*
Function: I2CAck
Description: Generates acknowledge for a transfer
*/
//void I2CAck(void)
//{
//    SSP1CON2bits.ACKDT = 0; // Acknowledge bit, 0 = ACK
//    SSP1CON2bits.ACKEN = 1; // Enable Acknowledge
//    while (SSP1CON2bits.ACKEN); // Wait until Acknowledge bit is transmitted
//}

/*
Function: I2CNak
Description: Generates Not-acknowledge for a transfer
*/
//void I2CNak(void)
//{
//    SSP1CON2bits.ACKDT = 1; // Acknowledge bit, 1 = NAK
//    SSP1CON2bits.ACKEN = 1; // Enable Acknowledge
//    while (SSP1CON2bits.ACKEN); // Wait until Not-Acknowledge bit is transmitted
//}

/*
Function: I2CWait
Description: Wait for transfer to finish
*/
void I2CWait(void)
{
    while ((SSP1CON2 & 0x1F) || (SSP1STAT & 0x04)); // Wait for any pending transfer
}

/*
Function: I2CSend
Arguments: dat - 8-bit data to be sent on bus
Description: Send 8-bit data on I2C bus
*/
void I2CSend(unsigned char dat)
{
    SSP1BUF = dat; // Load data into SSP1BUF
    while (SSP1STATbits.BF); // Wait until data is sent
    I2CWait(); // Wait for any pending transfer
}

/*
Function: I2CRead
Return: 8-bit data read from I2C bus
Description: Read 8-bit data from I2C bus
*/
//unsigned char I2CRead(void)
//{
//    unsigned char temp;
//    SSP1CON2bits.RCEN = 1; // Enable reception
//    while (!SSP1STATbits.BF); // Wait until data is received
//    temp = SSP1BUF; // Read data from SSP1BUF
//    I2CWait(); // Wait for any pending transfer
//    return temp; // Return the read data
//}



