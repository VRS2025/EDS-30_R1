
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
#include "icons.h"
#include "font16x16.h"

// Define bit masks for SP1 and SP2 error bits
#define SP1_OPEN_BIT   (1 << 0)
#define SP1_SHORT_BIT  (1 << 1)
#define SP1_GND_BIT    (1 << 2)
#define SP1_ALL_MASK   (SP1_OPEN_BIT | SP1_SHORT_BIT | SP1_GND_BIT)

#define SP2_OPEN_BIT   (1 << 3)
#define SP2_SHORT_BIT  (1 << 4)
#define SP2_GND_BIT    (1 << 5)
#define SP2_ALL_MASK   (SP2_OPEN_BIT | SP2_SHORT_BIT | SP2_GND_BIT)

// Bit for overall error flag provided as parameter in handleState
// (e.g. 1 << 6 for SP1, 1 << 7 for SP2)

//-----------------------------------------------------//
//  Function: detectLineState()
//  Description: Reads ADC and returns a defined line state.
//-----------------------------------------------------//

char detectLineState(char channel) {
    int adcValue = readADC(channel);
    
     if (adcValue >= ADC_OK_MIN && adcValue <= ADC_OK_MAX)
        return LINE_STATE_OK;
    else if (adcValue >= ADC_OPEN_MIN && adcValue <= ADC_OPEN_MAX)
        return LINE_STATE_CUT;
    else if (adcValue >= ADC_SHORT_MIN && adcValue <= ADC_SHORT_MAX)
        return LINE_STATE_SHORT;
    else if (adcValue >= ADC_GROUND_FAULT_MIN && adcValue <= ADC_GROUND_FAULT_MAX)
        return LINE_STATE_GROUND_FAULT;
    
    // Return an explicit unknown state instead of plain 0.
    return LINE_STATE_UNKNOWN;
}
//-----------------------------------------------------//
//  Function: handleState()
//  Description: Updates the global error flag 'sp_errors'
//               based on the detected line state and the provided
//               bit offset and overall error bit.
//-----------------------------------------------------//
void handleState(unsigned char lineState, unsigned char lineOffset, unsigned char all_err_bit) {
    switch (lineState) {
        case LINE_STATE_OK:
            // Clear the three bits for this channel and the overall error bit
            sp_errors &= ~((1 << lineOffset) | (1 << (lineOffset + 1)) | (1 << (lineOffset + 2)) | (1 << all_err_bit));
            // If no errors remain, disable blink
            if (all_errors1 == 0 && sp_errors == 0)
                blink_en = 0;
            break;
        case LINE_STATE_CUT:
            sp_errors |= ((1 << lineOffset) | (1 << all_err_bit));
            blink_en = 1;
            break;
        case LINE_STATE_SHORT:
            sp_errors |= ((1 << (lineOffset + 1)) | (1 << all_err_bit));
            blink_en = 1;
            break;
        case LINE_STATE_GROUND_FAULT:
            sp_errors |= ((1 << (lineOffset + 2)) | (1 << all_err_bit));
            blink_en = 1;
            break;
        default:
            // For an unknown state, you might choose to log or set a default error.
            break;
    }
}

//-----------------------------------------------------//
//  Function: handleLineStates()
//  Description: Checks the line states for one or both channels
//               depending on the configuration mode and updates
//               display elements accordingly.
//-----------------------------------------------------//
void handleLineStates(unsigned char en) {
    if (en) {
        adc_en = 0;
        line_test_on = 1;  // Disable interrupts for line test

        // Process channel 0 (Line 1) always
        char line1State = detectLineState(0);
        handleState(line1State, 0, 6);  // Using bit 6 as overall error for SP1

        // Display SP1 errors or clear line if no error bits are set
        if (sp_errors & SP1_OPEN_BIT) {
            img_view(35, 4, 46, _sp1_line_open_HE, 0);
        } else if (sp_errors & SP1_SHORT_BIT) {
            img_view(35, 4, 46, _sp1_line_short_HE, 0);
        } else if (sp_errors & SP1_GND_BIT) {
            img_view(25, 4, 87, _sp1_Line_GND_HE, 0);
        } else {
            lcd_clear_line(4);
        }
        
        // In dual-channel mode, also process channel 1 (Line 2)
        if (config.mode == 2) {
            char line2State = detectLineState(1);
            handleState(line2State, 3, 7);  // Using bit 7 as overall error for SP2

            if (sp_errors & SP2_OPEN_BIT) {
                img_view(35, 5, 45, _sp2_line_open_HE, 0);
            } else if (sp_errors & SP2_SHORT_BIT) {
                img_view(35, 5, 46, _sp2_line_short_HE, 0);
            } else if (sp_errors & SP2_GND_BIT) {
                img_view(25, 5, 87, _sp2_Line_GND_HE, 0);
            } else {
                lcd_clear_line(5);
            }
        }
        else {
            // For single-channel mode, clear the second display line
            lcd_clear_line(5);
        }
        
        line_test_on = 0;  // Re-enable interrupts for line test
    }
}
















//char detectLineState(char channel) {
//    int adcValue = readADC(channel);
//    
//    if (adcValue >= ADC_OK_MIN && adcValue <= ADC_OK_MAX) {
//        return LINE_STATE_OK;
//    } else if (adcValue >= ADC_OPEN_MIN && adcValue <= ADC_OPEN_MAX) {
//        return LINE_STATE_CUT;
//    } else if (adcValue >= ADC_SHORT_MIN && adcValue <= ADC_SHORT_MAX) {
//        return LINE_STATE_SHORT;
//    } else if (adcValue >= ADC_GROUND_FAULT_MIN && adcValue <= ADC_GROUND_FAULT_MAX) {
//        return LINE_STATE_GROUND_FAULT;
//    }
//    return 0; // Default return value
//}
//
//void handleState(unsigned char lineState, unsigned char lineOffset, unsigned char all_err) {
//    switch (lineState) {
//        case LINE_STATE_OK:
//            sp_errors &= ~(1 << lineOffset);
//            sp_errors &= ~(1 << (lineOffset + 1));
//            sp_errors &= ~(1 << (lineOffset + 2));
//            sp_errors &= ~(1 << all_err);
//            if (all_errors1 == 0 && sp_errors == 0) {
//                blink_en = 0;
//            }
//            break;
//        case LINE_STATE_CUT:
//            sp_errors |= (1 << lineOffset);
//            sp_errors |= (1 << all_err);
//            blink_en = 1;
//            break;
//        case LINE_STATE_SHORT:
//            sp_errors |= (1 << (lineOffset + 1));
//            sp_errors |= (1 << all_err);
//            blink_en = 1;
//            break;
//        case LINE_STATE_GROUND_FAULT:
//            sp_errors |= (1 << (lineOffset + 2));
//            sp_errors |= (1 << all_err);
//            blink_en = 1;
//            break;
//        default:
//            break;
//    }
//}
//
//void handleLineStates(unsigned char en) {
//    if (en == 1) {
//        adc_en = 0;
//        line_test_on = 1;  // Start test (disable interrupt for line test)
//        
//        // Process line 1 state always
//        char line1State = detectLineState(0);
//        handleState(line1State, 0, 6);
//
//        // Display SP1 errors or clear line if no SP1 errors
//        if ((sp_errors >> 0) & 1) {
//            img_view(35, 4, 46, _sp1_line_open_HE, 0);
//        } else if ((sp_errors >> 1) & 1) {
//            img_view(35, 4, 46, _sp1_line_short_HE, 0);
//        } else if ((sp_errors >> 2) & 1) {
//            img_view(25, 4, 87, _sp1_Line_GND_HE, 0);
//        } else {
//            lcd_clear_line(4); // Clear line 4 if no SP1 errors
//        }
//        
//        // If dual?channel mode, process line 2 state as well.
//        if (config.mode == 2) {
//            char line2State = detectLineState(1);
//            handleState(line2State, 3, 7);
//            
//            // Display SP2 errors or clear line if no SP2 errors
//            if ((sp_errors >> 3) & 1) {
//                img_view(35, 5, 45, _sp2_line_open_HE, 0);
//            } else if ((sp_errors >> 4) & 1) {
//                img_view(35, 5, 46, _sp2_line_short_HE, 0);
//            } else if ((sp_errors >> 5) & 1) {
//                img_view(25, 5, 87, _sp2_Line_GND_HE, 0);
//            } else {
//                lcd_clear_line(5); // Clear line 5 if no SP2 errors
//            }
//        } else {
//            // In single?channel mode, ensure the second display line is cleared.
//            lcd_clear_line(5);
//        }
//        
//        line_test_on = 0;  // End test (enable interrupts for line test)
//    }
//}
//
















//img_view(0,0,32,_floor_,0);
   
//char detectLineState(char channel) {
//    int adcValue = readADC(channel);
//    
//  if (adcValue >= ADC_OK_MIN && adcValue <= ADC_OK_MAX) {
//        return LINE_STATE_OK;
//    } else if (adcValue >= ADC_OPEN_MIN && adcValue <= ADC_OPEN_MAX) {
//        return LINE_STATE_CUT;
//    } else if (adcValue >= ADC_SHORT_MIN && adcValue <= ADC_SHORT_MAX) {
//        return LINE_STATE_SHORT;
//    } else if (adcValue >= ADC_GROUND_FAULT_MIN && adcValue <= ADC_GROUND_FAULT_MAX) {
//        return LINE_STATE_GROUND_FAULT;
//    }
//    return 0; // Default return value to ensure function always returns an int
//}
//
//void handleState(unsigned char lineState,unsigned char lineOffset,unsigned char all_err) {
//    switch (lineState) {
//        case LINE_STATE_OK:
//            sp_errors &= ~(1 << lineOffset);
//            sp_errors &= ~(1 << (lineOffset + 1));
//            sp_errors &= ~(1 << (lineOffset + 2));
//            sp_errors &= ~(1 << (all_err));
//            if (all_errors1 == 0 && sp_errors == 0) {
//                blink_en = 0;
//            }
//            break;
//        case LINE_STATE_CUT:
//            sp_errors |= (1 << lineOffset);
//            sp_errors |= (1 << (all_err));
//            blink_en = 1;
//            break;
//        case LINE_STATE_SHORT:
//            sp_errors |= (1 << (lineOffset + 1));
//            sp_errors |= (1 << (all_err));
//            blink_en = 1;
//            break;
//        case LINE_STATE_GROUND_FAULT:
//            sp_errors |= (1 << (lineOffset + 2));
//            //all_errors1 |= (1 << (all_err));
//            sp_errors |= (1 << (all_err));
//            blink_en = 1;
//            break;
//        default:
//            break;
//    }
//}
//
//void handleLineStates(unsigned char en) {
//    if (en == 1) {
//        adc_en = 0;        
//        line_test_on = 1;                                                       //start test(no interrupt for line test)
//        
//        
//        char line1State = detectLineState(0);
//        char line2State = detectLineState(1);
//
//        handleState(line1State, 0,6);
//        handleState(line2State, 3,7);
//
//
//
//        // Print SP1 errors or clear line if no errors
//        if ((sp_errors >> 0) & 1) {
//            img_view(35, 4, 46, _sp1_line_open_HE, 0);
//        }
//        if ((sp_errors >> 1) & 1) {
//            img_view(35, 4, 46, _sp1_line_short_HE, 0);
//        }
//        if ((sp_errors >> 2) & 1) {
//            img_view(25, 4, 87, _sp1_Line_GND_HE, 0);
//        }
//        if (!((sp_errors >> 0) & 1) && !((sp_errors >> 1) & 1) && !((sp_errors >> 2) & 1)) {
//            lcd_clear_line(4); // Clear line 4 if no SP1 errors
//        }
//
//        // Print SP2 errors or clear line if no errors
//        if ((sp_errors >> 3) & 1) {
//            img_view(35, 5, 45, _sp2_line_open_HE, 0);
//        }
//        if ((sp_errors >> 4) & 1) {
//            img_view(35, 5, 46, _sp2_line_short_HE, 0);
//        }
//        if ((sp_errors >> 5) & 1) {
//            img_view(25, 5, 87, _sp2_Line_GND_HE, 0);
//        }
//        if (!((sp_errors >> 3) & 1) && !((sp_errors >> 4) & 1) && !((sp_errors >> 5) & 1)) {
//            lcd_clear_line(5); // Clear line 5 if no SP2 errors
//        }
//        
//        line_test_on = 0;                                                       //END test(YES interrupt for line test)
//    }
//}










//int detectLineState(char channel) {
//
//         int adcValue = readADC(channel);
//        
//    if (adcValue >= ADC_OK_MIN && adcValue <= ADC_OK_MAX) {
//        return LINE_STATE_OK;
//    } else if (adcValue >= ADC_OPEN_MIN && adcValue <= ADC_OPEN_MAX) {
//        return LINE_STATE_CUT;
//    } else if (adcValue >= ADC_SHORT_MIN && adcValue <= ADC_SHORT_MAX) {
//        return LINE_STATE_SHORT;
//    } else if (adcValue >= ADC_GROUND_FAULT_MIN && adcValue <= ADC_GROUND_FAULT_MAX) {
//        //return LINE_STATE_GROUND_FAULT;
//    } else {
//        //return LINE_STATE_CUT; // Default to Open if none of the above conditions match
//    }
//   return 0;                                                       // Default return value to ensure function always returns an unsigned int   
//}
//
//void handleState(int lineState, int lineOffset) {
//    switch (lineState) {
//        case LINE_STATE_OK:
//            sp_errors &= ~(1 << lineOffset);
//            sp_errors &= ~(1 << (lineOffset + 1));
//            sp_errors &= ~(1 << (lineOffset + 2));
//            all_errors1 &= ~(1 << (lineOffset + 3));
//            if (all_errors1 == 0 && sp_errors == 0) {
//                blink_en = 0;
//            }
//            break;
//        case LINE_STATE_CUT:
//            all_errors1 |= (1 << (lineOffset + 3));
//            sp_errors |= (1 << lineOffset);
//            blink_en = 1;
//            break;
//        case LINE_STATE_SHORT:
//            all_errors1 |= (1 << (lineOffset + 3));
//            sp_errors |= (1 << (lineOffset + 1));
//            blink_en = 1;
//            break;
//        case LINE_STATE_GROUND_FAULT:
//            all_errors1 |= (1 << (lineOffset + 3));
//            sp_errors |= (1 << (lineOffset + 2));
//            blink_en = 1;
//            break;
//        default:
//            break;
//    }
//}
//
//void handleLineStates(unsigned char en) {
//    if (en == 1) {
//        int line1State = detectLineState(0);
//        int line2State = detectLineState(1);
//
//        handleState(line1State, 0);
//        handleState(line2State, 3);
//
//        line_test_on = 1;
//        adc_en = 0;
//                // Print SP line errors or clear lines if no errors
//        if (((sp_errors >> 0) & 1)) {
//            img_view(35, 4, 53, _sp1_line_open_HE, 0);
//        } else if (((sp_errors >> 1) & 1)) {
//            img_view(35, 4, 54, _sp1_line_short_HE, 0);
//        } else if (((sp_errors >> 2) & 1)) {
//            img_view(25, 4, 85, _sp1_Line_GND_HE, 0);
//        } else {
//            lcd_clear_line(4); // Clear line 4 if no SP1 errors
//        }
//
//        if (((sp_errors >> 3) & 1)) {
//            img_view(35, 5, 53, _sp2_line_open_HE, 0);
//        } else if (((sp_errors >> 4) & 1)) {
//            img_view(35, 5, 54, _sp2_line_short_HE, 0);
//        } else if (((sp_errors >> 5) & 1)) {
//            img_view(25, 5, 85, _sp2_Line_GND_HE, 0);
//        } else {
//            lcd_clear_line(5); // Clear line 5 if no SP2 errors
//        }
//    }
//}



//int detectLine2State(int channel) {
//
//       int L2_adcValue = readADC(channel);
//        
//    if (L2_adcValue >= ADC_OK_MIN && L2_adcValue <= ADC_OK_MAX) {
//        return LINE_STATE_OK;
//    } else if (L2_adcValue >= ADC_OPEN_MIN && L2_adcValue <= ADC_OPEN_MAX) {
//        return LINE_STATE_CUT;
//    } else if (L2_adcValue >= ADC_SHORT_MIN && L2_adcValue <= ADC_SHORT_MAX) {
//        return LINE_STATE_SHORT;
//    } else if (L2_adcValue >= ADC_GROUND_FAULT_MIN && L2_adcValue <= ADC_GROUND_FAULT_MAX) {
//        //return LINE_STATE_GROUND_FAULT;
//    } else {
//        return LINE_STATE_CUT; // Default to Open if none of the above conditions match
//    }
//  return 0;      
//}
//
//void handleLineStates(unsigned char en) {
//    
//    if(en==1){
//       
//    int line1State = detectLine1State(0);
//    int line2State = detectLine2State(1);
//
//    switch (line1State) {
//        case LINE_STATE_OK:
//            clear_scree_();
//            sp_errors  &=~(1<<0);
//            sp_errors  &=~(1<<1);
//            sp_errors  &=~(1<<2);
//            all_errors1  &=~(1<<3);
//            if(all_errors1 == 0){ blink_en = 0;}               
//            break;
//        case LINE_STATE_CUT:
//            all_errors1 |=(1<<3);
//            sp_errors |=(1<<0);
//            blink_en = 1;
//            break;
//        case LINE_STATE_SHORT:
//            all_errors1 |=(1<<3);            
//            sp_errors |=(1<<1);
//            blink_en = 1;
//            break;
//        case LINE_STATE_GROUND_FAULT:
//            all_errors1 |=(1<<3);
//            sp_errors |=(1<<2);
//            blink_en = 1;
//            break;
//        default:
//            // Handle unexpected state for Line 1
//            break;
//    }
//
//  
//
//    switch (line2State) {
//        case LINE_STATE_OK:
//            clear_scree_();
//            sp_errors  &=~(1<<3);
//            sp_errors  &=~(1<<4);
//            sp_errors  &=~(1<<5);
//            all_errors1  &=~(1<<4);
//            if(all_errors1 == 0 && sp_errors == 0){ blink_en = 0;}  
//            break;
//        case LINE_STATE_CUT:
//            all_errors1 |=(1<<4);
//            sp_errors |=(1<<3);
//            blink_en = 1;
//            break;
//        case LINE_STATE_SHORT:
//            all_errors1 |=(1<<4);
//            sp_errors |=(1<<4);
//            blink_en = 1;
//            break;
//        case LINE_STATE_GROUND_FAULT:
//            all_errors1 |=(1<<4);
//            sp_errors |=(1<<5);
//            blink_en = 1;
//            break;
//        default:
//            // Handle unexpected state for Line 2
//            break;
//    }
//    
//            test_on = 0;
//            adc_en = 0; 
//            print_sp_errors_();
////           lcd_gotoxy(67,3);
////           lcd_putch('1',0);  
//    }
//}


//void sp_line_malfunction(void){
//    
//    	switch(line_trouble){
//
//			case 0: if(all_errors1 == 0){
//                    blink_en = 0;
//                    lcd_gotoxy(40,0);
//                    lcd_putch('1',0); 
//                    img_view(45,3,45,_floor_ok,0);
//                    }
//					break;
//			case 2:img_view(25,4,76,_sp_line1_short_,1);
//                    blink_en = 1;
//
//					break;
//			case 1:img_view(25,4,79,_sp_line2_cut_,1);
//            //case 1:img_view(25,4,76,cut_line,1);
//
//                   //img_view(50,5,7,_sp_line2_cut_);
//                    blink_en = 1;
//
//					break;
////            case 3:img_view(25,4,56,_sp_gnd1_,1);
////                    blink_en = 1;
////
////					break;
//			}
//        
//
//        
//}  

//void sys_malfunction(void){
//    
//switch(line_trouble){
//
//			case 0: if(all_errors1 == 0){
//                    blink_en = 0;
//
//                    }
//					break;
//
//			}
//    
//    
//    
//}

//void pwr_malfunction(void){
//    
//       if(all_errors1!=0){
//           
////       if(ac_troble == 1){
////            img_view(50,5,34,ac_error_,1);
////            blink_en = 1;
////            PWR_LED = 0;
////        }
//        
//        if(batt_troble == 1){
//            img_view(50,4,34,dc_error_,1);
//            blink_en = 1;
//        }
//       }else{
//           PWR_LED = 1;
//           lcd_clear_line(4);
//       }
//       
//       if(sd_troble == 1){
//            img_view(35,3,55,msg_error_,0);
//            blink_en = 1;   
//        }else{
//           lcd_clear_line(5);
//           
//        }
//}


//img_view(30,5,56,gnd_fault,1)
//img_view(30,5,76,cut_line,1)
//img_view(30,5,76,short_line,1)

//img_view(40,4,34,dc_error_,1)
//img_view(40,4,46,ac_error_,1)
//img_view(40,4,55,msg_error_,1)
//img_view(40,4,34,amp_error_,1)
