///*
// * File:   DY1703A.c
// * Author: romanvinokur
// *
// * Created on July 28, 2024, 9:42 AM
// */
//
///*
// * File:   fn16.c
// * Author: romanvinokur
// *
// * Created on May 26, 2024, 9:26 AM
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
#include "eng_font.h"
#include "font16x16.h"
 

void jq8900_init(void) {
    
    jq8900_setVolume(config.msg_volume);
    __delay_ms(100);
    uint8_t switch_to_flash[] = {0x02};
    JQ8900_SendCommand(0x0B, 1, switch_to_flash); 
    __delay_ms(500);
    jq8900_setVolume(config.msg_volume);
   // DY_SV17F_SendCommand(0x09,0,0);
}

void JQ8900_SendCommand(uint8_t cmd_type, uint8_t data_length, uint8_t* data) {

    uint8_t packet_length = 0;
    uint8_t crc = 0;

    // Start byte
    packet[packet_length++] = 0xAA;
    crc += 0xAA;

    // Command type
    packet[packet_length++] = cmd_type;
    crc += cmd_type;

    // Data length
    packet[packet_length++] = data_length;
    crc += data_length;

    // Data
    for (uint8_t i = 0; i < data_length; i++) {
        packet[packet_length++] = data[i];
        crc += data[i];
    }

    // CRC
    packet[packet_length++] = crc;

    // Send packet
    for (uint8_t i = 0; i < packet_length; i++) {
        while (!PIR3bits.TX2IF); // Wait until the transmit buffer is empty
        TXREG2 = packet[i];      // Transmit data
    }

}

uint8_t is_module_busy(void) {
    return FN_busy;  // Return the state of the busy pin
}

void jq8900_setVolume(uint8_t volume) {
    uint8_t data[1];
    data[0] = volume; // Volume level
    JQ8900_SendCommand(CMD_SET_VOLUME, 1, data);
}

void jq8900_playTrack(uint8_t trackNumber) {

    uint8_t data[2];
    data[0] = 0x00;  // Reserved or command-specific byte
    data[1] = trackNumber;  // Track number

    JQ8900_SendCommand(0x07, 2, data);  // Command to play a specific track

}

void jq8900_stop(void) {
    JQ8900_SendCommand(0x04, 0x00, 0x00); // Stop command
}








//void sv17f_init(void) {
//    
//    DY1703_SetVolume(config.msg_volume);
//    __delay_ms(100);
//    uint8_t switch_to_flash[] = {0x02};
//    DY_SV17F_SendCommand(0x0B, 1, switch_to_flash); 
//    __delay_ms(500);
//    DY1703_SetVolume(config.msg_volume);
//   // DY_SV17F_SendCommand(0x09,0,0);
//}
//
//void DY_SV17F_SendCommand(uint8_t cmd_type, uint8_t data_length, uint8_t* data) {
//
//    uint8_t packet_length = 0;
//    uint8_t crc = 0;
//
//    // Start byte
//    packet[packet_length++] = 0xAA;
//    crc += 0xAA;
//
//    // Command type
//    packet[packet_length++] = cmd_type;
//    crc += cmd_type;
//
//    // Data length
//    packet[packet_length++] = data_length;
//    crc += data_length;
//
//    // Data
//    for (uint8_t i = 0; i < data_length; i++) {
//        packet[packet_length++] = data[i];
//        crc += data[i];
//    }
//
//    // CRC
//    packet[packet_length++] = crc;
//
//    // Send packet
//    for (uint8_t i = 0; i < packet_length; i++) {
//        while (!PIR3bits.TX2IF); // Wait until the transmit buffer is empty
//        TXREG2 = packet[i];      // Transmit data
//    }
//
//}
//
//uint8_t is_module_busy(void) {
//    return FN_busy;  // Return the state of the busy pin
//}
//
//void DY1703_SetVolume(uint8_t volume) {
//    uint8_t data[1];
//    data[0] = volume; // Volume level
//    DY_SV17F_SendCommand(CMD_SET_VOLUME, 1, data);
//}
//
//void _play_track(uint8_t trackNumber) {
//
//    uint8_t data[2];
//    data[0] = 0x00;  // Reserved or command-specific byte
//    data[1] = trackNumber;  // Track number
//
//    DY_SV17F_SendCommand(0x07, 2, data);  // Command to play a specific track
//
//}
//
//void msg_stop(void) {
//    DY_SV17F_SendCommand(0x04, 0x00, 0x00); // Stop command
//}
//


