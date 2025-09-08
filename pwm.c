/*
 * File:   pwm.c
 * Author: romanvinokur
 *
 * Created on May 28, 2024, 9:07 PM
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

// CCP2 initialization for PWM
//void CCP2_Init(void) {
//    
//    CCP2CON = 0x0C; // Configure CCP2 in PWM mode
//
//    PR2 = 249; // Set PWM period (~1kHz if Fosc = 16MHz, prescaler = 4)
//
//    T2CON = 0x05; // Timer2 on, prescaler 1:4
//    TMR2 = 0; // Clear Timer2
//
//    CCPR2L = 124; // Initialize duty cycle to 50% (124/249 * 100)
//
//    T2CON |= 0x04; // Ensure Timer2 is ON
//}
////
////
//// Set PWM duty cycle for CCP2
//void PWM_Set_Duty(unsigned int duty) {
//    if (duty < 1024) {
//        CCPR2L = (char)duty >> 2; // Set the 8 MSBs of duty
//        CCP2CONbits.DC2B = duty & 0x03; // Set the 2 LSBs of duty
//    }
//}


