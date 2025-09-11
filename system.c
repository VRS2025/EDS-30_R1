/*
 * File:   system.c
 * Author: romanvinokur
 *
 * Created on June 23, 2024, 4:42 PM
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


//**************************************************************************
// sys_adc_tst: Tests ADC?related inputs (AC, battery, amplifier) and
// displays one error message at a time based on priority (AC > Battery > Amp).
//**************************************************************************
void sys_adc_tst(unsigned char i) {
    // Only perform the test if i is 1.
    if (i != 1)
        return;
    
    // Use a static variable to track the last error displayed.
    static unsigned char last_error = 0;  // 0: no error, 1: AC, 2: Battery, 3: Amplifier
    unsigned char error_code = 0;         // To store the current error code

    // Clear bit 10 before testing and update the shift register.
    shiftRegisterData &= ~(1u << 10);
    SendShiftData595(shiftRegisterData);
    __delay_ms(100);

    // ---------- Test AC Voltage ----------
    if (ac_ == 0) { 
        all_errors1 |= (1u << 1);  // Set error bit for AC
        error_code = 1;            // AC error code
        PWR_LED = 0;
        a = 0;
    } else {
        all_errors1 &= ~(1u << 1);
        if (last_error == 1) {
            // If AC error was previously displayed, clear it now.
            lcd_clear_line(2);

        }
        PWR_LED = 1;
        p4 = 0;
    }

    // ---------- Test Battery Voltage ----------
    // Only check battery if no AC error is active.
    if (error_code == 0 && batt_ == 0) { 
        all_errors1 |= (1u << 0);  // Set error bit for battery
        error_code = 2;            // Battery error code
        b = 0;                 
    } else { 
        all_errors1 &= ~(1u << 0);
        p3 = 1;
    }

    // ---------- Test Amplifier Fault ----------
    // Only check amplifier if neither AC nor battery error exists.
    if (error_code == 0 && fault_amp != 1) {
        all_errors1 |= (1u << 6);  // Set error bit for amplifier
        error_code = 3;            // Amplifier error code
        c = 0;                       
    } else {
        all_errors1 &= ~(1u << 6);
        p5 = 0;
    }

    // ---------- Update the Display if Error Changed ----------
    if (error_code != last_error || exit_menu == 0) {
        exit_menu = 1;
        lcd_clear_line(2);
        switch (error_code) {
            case 1:
                img_view(20, 2, 85, ac_error_, 0);
                break;
            case 2:
                img_view(30, 2, 65, dc_error_, 0);
                break;
            case 3:
                img_view(40, 2, 34, amp_error_, 0);
                break;
            default:
                // No error; leave the line clear.
                break;
        }
        last_error = error_code;
    }

    // Restore bit 10 and update the shift register.
    shiftRegisterData |= (1u << 10);
    SendShiftData595(shiftRegisterData);

    pwr_tst = 0;
}

//**************************************************************************
// mulfunctions_: Displays malfunction screen if any errors are active,
// and clears error indicators when there are none.
//**************************************************************************

void mulfunctions_(unsigned char _neg) {
    // Set the malfunction screen flag.
    mulf_screen1 = _neg;

    // If any errors are active (all_errors1 or sp_errors nonzero),
    // display the malfunction image.
    if (all_errors1 != 0 || sp_errors != 0) {
        img_view(36, 0, 51, Malfunction_HE_34, _neg);
        shiftRegisterData |= ((unsigned int) (1 << 11));                   // - DRY_CONT - On
        one_step = 0;
        backlight_tmr = 0;
        BACK_LIGHT = 1;
        clr1 = 0;
        clr2 = 0;
        clr3 = 0;
    } else {
        //lcd_clear_pixels(30, 95, 0);
        if (config.mode == 1)
        {
            if (clr1 == 0 && clr2 == 0 && clr3 == 0)
            {
                lcd_gotoxy(35, 0);
                lcd_putstr(" EDS-30  ", 0);
                shiftRegisterData &= ~((unsigned int) (1 << 11)); // - DRY_CONT - OFF
                //img_view(45, 0, 37, eds_30, 0);
                clr1 = 1;
                clr2 = 1;
                clr3 = 1;
            }
        } else {
            
            if(clr1 == 0 || clr2 == 0 || clr3 == 0){
                lcd_gotoxy(35, 0);
                lcd_putstr(" EDS-60  ", 0);
                //img_view(38, 0, 48, eds_30D, 0);
                clr1 = 1;
                clr2 = 1;
                clr3 = 1;
            }
        }
        one_step = 1;
    }
if ((((all_errors1 & (1 << 3)) == 0) && ((all_errors1 & (1 << 5)) == 0) && ((all_errors1 & (1 << 7)) == 0))) {
    lcd_clear_pixels(10, 105, 3);

} 
else if ((((sp_errors & (1 << 0)) == 0) && ((sp_errors & (1 << 1)) == 0) && ((sp_errors & (1 << 2)) == 0))) {
    lcd_clear_pixels(10, 105, 4);

} 
else if ((((sp_errors & (1 << 3)) == 0) && ((sp_errors & (1 << 4)) == 0) && ((sp_errors & (1 << 5)) == 0))) {
    lcd_clear_pixels(10, 105, 5);

} 

}









////volatile unsigned int shift_reg_charger = 0b0000010000000000;
//
//
//void sys_adc_tst(unsigned char i) {
//    static unsigned char last_error = 0;  // Keeps track of the last error printed
//
//    if (i == 1) {
//       shiftRegisterData &= ~ ((unsigned int) (1 << 10));
//        SendShiftData595(shiftRegisterData);
//        __delay_ms(100);
//        if (ac_ == 0) { 
//            all_errors1 |= (1 << 1);
//           // if (last_error != 1) {
//                lcd_clear_line(2);
//                img_view(20, 2, 85, ac_error_, 0); 
//                last_error = 1;
//                PWR_LED = 0;
//           // }
//            a = 0;
//        } else {
//            all_errors1 &= ~(1 << 1);
//            if (last_error == 1) {
//                lcd_clear_line(2);
//                last_error = 0;
//                PWR_LED = 1;
//            }
//            p4 = 0;
//        }
//
//        if (batt_ == 0) { 
//            all_errors1 |= (1 << 0);
//            //if (last_error != 2) {
//                lcd_clear_line(2);
//                img_view(30, 2, 65, dc_error_, 0);
//                last_error = 2;
//            //}
//            b = 0;                 
//        } else { 
//            all_errors1 &= ~(1 << 0);
//            if (last_error == 2) {
//                lcd_clear_line(2);
//                last_error = 0;
//            }
//            p3 = 1;
//        }
//
//        if (fault_amp != 1) {
//            all_errors1 |= (1 << 6);
//            //if (last_error != 3) {
//                lcd_clear_line(2);
//                img_view(40, 2, 34, amp_error_, 0);
//                last_error = 3;
//           // }
//            c = 0;                       
//        } else {
//            all_errors1 &= ~(1 << 6);
//            if (last_error == 3) {
//                lcd_clear_line(2);
//                last_error = 0;
//            }
//            p5 = 0;
//        }
//        shiftRegisterData |= ((unsigned int) (1 << 10));
//        SendShiftData595(shiftRegisterData);
//        pwr_tst=0;
//    }
//}
//
//
//
//void mulfunctions_(unsigned char _neg){
//    
//mulf_screen1 = _neg;
//        if(all_errors1 !=0 || sp_errors !=0){
//             img_view(35, 1, 51, Malfunction_HE_34, _neg);
//             //img_view(75, 2, 34, koma_HE_34, 0);
////             display_digit(1,7,10,0);
//             one_step = 0;
//             backlight_tmr = 0;
//             backlight_en = 0;
//             BACK_LIGHT = 1;
//         }
//   if(all_errors1 == 0 && one_step == 0){
//        lcd_clear_pixels(30,95,1);
////        display_digit(1,7,11,0);
//        if(config.mode == 1){
//        img_view(40, 2, 37, eds_30, 0);
//        }else{
//            img_view(38, 2, 48, eds_30D, 0);
//        }
//        one_step = 1;
//        backlight_en = 1;
//
//    if(clr1 == 1){                                                              // Clear MIC1,2 error
//             clr1 = 0;
//              lcd_clear_pixels(10,105,3);
//        }
//   }
//}







//void msg_test_func(void) {
//    
//    if (n_f == 1 && all_alarms == 0) {
//
//        if (t1 == 1) {
//
//            DY1703_SetVolume(0);                                        // Volume  = 0
//           _play_track(5);                                        // Play First Track
//            __delay_ms(150);
//            n_f = 0;
//            __delay_ms(100);
//            if (is_module_busy() == 0) {
//                
//                sd_troble = 1;
//                all_errors1 |=(1<<2);                                           // MSG Error
//                if(p6 == 1){img_view(40,2,55,msg_error_,0);p6 = 0;}                     // NO SD CARD
//                
//            } else {
//                p6 = 1;
//                sd_troble = 0;
//                all_errors1 &=~(1<<2);                                          // MSG NO Error
//                n_f = 0;
//            }
//            msg_stop();                                        // Stop Play
//            __delay_ms(50);
//            __delay_ms(50);
//            DY1703_SetVolume(config.msg_volume);                        //Set MSG Volume 
//
//
//        }
//    }
//}


