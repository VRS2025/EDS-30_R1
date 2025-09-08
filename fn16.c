/*
 * File:   fn16.c
 * Author: romanvinokur
 *
 * Created on May 26, 2024, 9:26 AM
 */

//#include <xc.h>
//#include <stdint.h>
//#include <string.h>
//#include <stdarg.h>
//#include <stdio.h>   
//#include <pic18F46K22.h>
//#include "user.h"
//#include "LCD_ST7565.h"
//#include "logos.h"
//#include "eng_font.h"
//#include "font16x16.h"

//
//void send_uart2(unsigned char len){
//	char i;
//	uart2_write(0x7E);
//	for(i=0;i<len;i++)
//		{	
// 			__delay_ms(1);
//			uart2_write(Sendbuff[i]);
//		}
//	uart2_write(0xEF);
//};
//
//void DoSum(unsigned char *Str,unsigned char len){
//	int xorsum = 0;
//	char i;
//	
//	for(i=0;i<len;i++)
//		{
//			xorsum = xorsum + Str[i];
//		}
//	xorsum = 0-xorsum;
//	*(Str+i) = (char)(xorsum>>8);
//	*(Str+i+1)=(char)(xorsum&0x00ff);
//};
//
//void send_fn_command(unsigned char CMD,unsigned char feedback,unsigned int dat){
//    mute_amp = 0;
//	Sendbuff[0]=0xff;                                                           //Version bytes
//	Sendbuff[1]=0x06;                                                           //Length
//	Sendbuff[2]= CMD;                                                           //Control instructions
//	Sendbuff[3]=feedback;                                                       //The need for feedback
//	Sendbuff[4]=(char)(dat >> 8);                                               //dataH
//	Sendbuff[5]=(char)(dat);                                                    //dataL
//	DoSum(&Sendbuff[0],6);                                                      //Check
//	send_uart2(8);                                                              //Send this frame data
//};
//
//unsigned char is_module_busy() {
//    return FN_busy;  // Assuming BUSY pin is connected to RA5
//}
//
////void play_first_track() {
////    send_fn_command(0x03,0,0x01);  // Command to play the first track
////}
//
//void msg_stop(void){
//    
//send_fn_command(0x16,1, 0x00);  // Command to play the first track
//}
//
//void set_volume(unsigned char volume) {
//    
//    if(volume >=31){
//        volume = 30;
//    }
//    send_fn_command(0x06, 0, volume);  // Command to set the volume
//}
//
//void _play_track(unsigned int track,unsigned char feetback){
//    
//if (FN_busy == 1) {
//
//    //set_volume(config.msg_volume);
//        send_fn_command(0x03,feetback,track);                                   // Command to play the first track
//    }     
//}

