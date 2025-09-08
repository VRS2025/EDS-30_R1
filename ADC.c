
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


void ADC_init(void){
    
    ADCON2bits.ADFM = 1;         // Right justified result
    ADCON2bits.ACQT = 3;         // Acquisition time (number of TAD) = 4*TAD
    ADCON2bits.ADCS = 5;         // A/D Conversion Clock (TAD) = Fosc/16
    ADCON1bits.PVCFG = 0;       // VDD = +Vref
    ADCON1bits.NVCFG = 0;       // VSS = -Vref

    ADCON0bits.ADON = 1;         // ADC module is enabled
    PIE1bits.ADIE = 0;           // Enable ADC interrupt
}

// int readADC(unsigned char channel) {
//     
//    ADCON0bits.CHS = channel;    // Select the specified ADC channel    
//    ADCON0bits.GO_nDONE = 1;     // Start conversion   
//    while (ADCON0bits.GO_nDONE); // Wait for conversion to complete  
//    return ((ADRESH << 8) + ADRESL); // Combine the result from ADRESH and ADRESL
//}
 
//int readADC(unsigned char channel) {
//    ADCON0bits.CHS = channel;    // Select the specified ADC channel    
//    ADCON0bits.GO_nDONE = 1;     // Start conversion   
////while (ADCON0bits.GO_nDONE);{ // Wait until GO_nDONE is cleared  
//    //asm("nop");  // No operation, just wait
// //     }
//    //__delay_us(10);
//    // Wait for conversion to complete using inline assembly
//    __asm__ (
//        "WaitForConversion: \n"
//        "btfsc ADCON0, 1 \n"     // Check if GO_nDONE is still set
//        "goto WaitForConversion"  // Loop until conversion is complete
//    );
//
//    // Combine the result from ADRESH and ADRESL
//    return ((ADRESH << 8) + ADRESL); // Return combined result
//}

 int readADC(unsigned char channel) {
    ADCON0bits.CHS = channel;    // ??????? ?????
    __delay_us(5);               // ????? ???????, ????? ??????????? ?????????

    ADCON0bits.GO_nDONE = 1;     // ????? ??????????????
    while (ADCON0bits.GO_nDONE); // ????? ?????????

    return ((ADRESH << 8) | ADRESL);
}
 

 
 
 