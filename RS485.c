/*
 * File:   uart1.c
 * Author: romanvinokur
 *
 * Created on June 30, 2024, 9:45 PM
 */

#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <pic18F46K22.h>
#include "user.h"
#include "LCD_ST7565.h"
#include "logos.h"
#include <stdbool.h>

// UART1 send function
void UART1_Write(uint8_t data) {
    while(!TXSTAbits.TRMT); // Wait until transmit buffer is empty
    TXREG1 = data;         // Send data
}

// UART1 receive function
//uint8_t UART1_Read(void) {
//    return RCREG1;         // Return received data
//}

void RS485_Send(uint8_t address, uint8_t command, uint8_t data) {
    uint8_t checksum = Calculate_Checksum(address, command, data);
    
    RS485_TX_MODE;          // Enable transmit mode
    UART1_Write(0x55);      // Start byte
    UART1_Write(address);   // Send address
    UART1_Write(command);   // Send command
    UART1_Write(data);      // Send data
    UART1_Write(checksum);  // Send checksum
    __delay_ms(10);         // Short delay to ensure transmission completes
    RS485_RX_MODE;          // Enable receive mode
} // Send a message over RS485

// Receive a message from RS485
//uint8_t RS485_Receive(void) {
//    return UART1_Read();    // Return received data
//}

uint8_t Calculate_Checksum(uint8_t address, uint8_t command, uint8_t data) {
    return address ^ command ^ data; // XOR of address, command, and data
}// Calculate a simple checksum for error detection
// RS485 master task
void RS485_Master_Task(void) {
    
if(config._485 == 21) {

            if (data_ready) {
                data_ready = 0; // Clear flag  
            }
            if (success == 0) {
                success = 1;
                rs_tmr_on = 1;

                RS485_Send(current_slave, 0x01, 0x00); // Example command and data

                if (current_slave == config._485) {
                    current_slave = 1;
                } else {
                    current_slave++;
                }

            }
        }
else if(config._485 >1 || config._485 > 21)
            RS485_Slave_Task();
    }

void RS485_Slave_Task(void) {
    if (data_ready) {
        data_ready = 0; // Clear the data ready flag

        // Process received data
        if (rx_index >= BUFFER_SIZE) {
            // Assuming a complete message is received when BUFFER_SIZE is reached
            uint8_t start_byte = rx_buffer[0];
            uint8_t address = rx_buffer[1];
            uint8_t command = rx_buffer[2];
            uint8_t data = rx_buffer[3];
            uint8_t checksum = rx_buffer[4];

            // Verify start byte and checksum
            if (start_byte == 0x55 && Calculate_Checksum(address, command, data) == checksum) {
                // Handle the command based on address and command
                Handle_Command(address, command, data);
            }
            
            // Reset the index for the next message
            rx_index = 0;
        }
        
        // Optionally send a response
        if (!response_sent) {
            // Prepare a response (example response_data)
            uint8_t response_data = 0x00; // Modify as per your requirements
            RS485_Send_Response(current_slave, response_data);
            response_sent = 1; // Mark that the response has been sent
        }
    }
}
// Handle commands received from the master
void Handle_Command(uint8_t address, uint8_t command, uint8_t data) {
    // Implement your command handling logic here
    // Example: Process different commands based on the received command
    switch (command) {
        case 0x01:
            // Handle command 0x01
            break;
        case 0x02:
            // Handle command 0x02
            break;
        // Add more cases as needed
        default:
            // Handle unknown command
            break;
    }
}
// Send a response to the master
void RS485_Send_Response(uint8_t address, uint8_t response_data) {
    uint8_t checksum = Calculate_Checksum(address, 0x01, response_data); // Adjust command and data as needed
    
    RS485_TX_MODE;          // Enable transmit mode
    UART1_Write(0x55);      // Start byte
    UART1_Write(address);   // Send address
    UART1_Write(0x01);      // Example command (response command)
    UART1_Write(response_data); // Send response data
    UART1_Write(checksum);  // Send checksum
    __delay_ms(10);         // Short delay to ensure transmission completes
    RS485_RX_MODE;          // Enable receive mode
}


