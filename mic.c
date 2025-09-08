/*
 * File:   mic.c
 * Author: romanvinokur
 *
 * Created on June 2, 2024, 1:45 PM
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

// ??????? ?????? MIC1
MicThresholds mic1Thresholds;

unsigned int detectStatePTT(unsigned char zone_sel, char adcChannel, MicThresholds *mt) {
    if (zone_sel < 3) {
       unsigned int adcValue = (unsigned int)readADC(adcChannel);

        if (adcValue >= mt->ok_min && adcValue <= mt->ok_max) {
            return STATE_MIC_OK;
        } else if (adcValue >= mt->pr_min && adcValue <= mt->pr_max) {
            return STATE_MIC_PRESET;
        } else if (adcValue >= mt->ct_min && adcValue <= mt->ct_max) {
            return STATE_MIC_CUT;
        } //else if (adcValue >= mt->sh_min && adcValue <= mt->sh_max) {
           // return STATE_MIC_SHORT;
        //}
    }
    return STATE_MIC_OFF; // Default return value
}
// Refactored handleEmergencyCalls: splits handling for MIC1,MIC2,MIC3
void handleEmergencyCalls(unsigned char en) {
    if (!en)
        return;

    mic1_state(detectStatePTT(config.zone_sel1, 17,&mic1Thresholds));              // MIC1 (PIN43/AN17)
    mic2_state(detectStatePTT(config.zone_sel2, 14,&mic1Thresholds));              // MIC2 (PIN36/AN14)
    mic3_state(detectStatePTT(config.zone_sel3, 3,&mic1Thresholds));               // MIC3 (PIN24/AN3)

    action(zone1State, zone2State, shiftRegisterData);                          // After all modifications, send the shift register data and update relays.
}

void mic1_state(unsigned int PTT1){
    
     // --- Handle MIC1 ---
    if (PTT1 == STATE_MIC_PRESET && press1 == 0 && press_all == 0) {            // Handle MIC1
        dry_c = 0;
        line_test_on = 1;
        tmr_line_tst = 0;
        press1 = 1;
        mic1_not_prs = 0;
        //backlight_en = 0;
        BACK_LIGHT = 1;
        all_alarms |= (1 << 0);
        m1_inv = 1;

        // Configure MIC1 based on zone selection and mic1_mode.
        switch (config.zone_sel1) {
            case 0:
                if (config.mic1_mode == 0)
                    shiftRegisterData &= ~(1u << 12);                           // AUX/MIC1
                            if(vol_set1 == 0){                                  // ? 3
                                    vol_set1 = 1;                               // ? 3
                                    SetWiperValue(MCP4561_ADDR1, 0, config.mic1_volume);
                                }
                else
                    shiftRegisterData |= (1 << 12);                             // MIC1/AUX

                zone1State = 1;
                shiftRegisterData |= (1 << 8) | (1 << 9);
                shiftRegisterData &= ~(1u << 4);
                // Audio output: single vs dual channel
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 0);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 0) | (1 << 1));
                break;
            case 1:
                if (config.mic1_mode == 0)
                    shiftRegisterData &= ~(1u << 12);
                else
                    shiftRegisterData |= (1 << 12);

                zone2State = 1;
                shiftRegisterData |= (1 << 8) | (1 << 9);
                shiftRegisterData &= ~(1u << 5);
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 0);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 0) | (1 << 1));
                break;
            case 2:
                if (config.mic1_mode == 0)
                    shiftRegisterData &= ~(1u << 12);
                else
                    shiftRegisterData |= (1 << 12);

                zone1State = 1;
                zone2State = 1;
                shiftRegisterData |= (1 << 8) | (1 << 9);
                shiftRegisterData &= ~((1u << 4) | (1u << 5));
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 0);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 0) | (1 << 1));
                jq8900_stop();
                break;
        }
    } else if (PTT1 == STATE_MIC_OK && mic1_not_prs == 0 && press_all == 0) {
        // Turn off MIC1 audio if PTT returns to OK.
        //vol_set3 = 0;
        mic1_cut = 0;
        mic1_short = 0;
        line_test_on = 0;
        mic1_not_prs = 1;
        m1_inv = 0;
        all_alarms &= ~(1 << 0);
        all_errors1 &= ~(1 << 5);
        press1 = 0;
        dry_c = 1;
        if (config.zone_sel1 == 0 || config.zone_sel1 == 2)
            zone1State = 0;
        if (config.zone_sel1 == 1 || config.zone_sel1 == 2)
            zone2State = 0;

        // Clear corresponding audio bits.
        if (mic2_not_prs)
            shiftRegisterData &= ~((1u << 0) | (1u << 1) | (1u << 8) | (1u << 9));
        else
            shiftRegisterData &= ~((1u << 0) | (1u << 1) | (1u << 9));
    }

    if (PTT1 == STATE_MIC_CUT ){//|| PTT1 == STATE_MIC_SHORT) {
        mic1_cut = 1;
        mic1_not_prs = 0;
        all_errors1 |= (1 << 5);
        blink_en = 1;
    }
//    if(PTT1 == STATE_MIC_SHORT){
//            mic1_short = 1;
//            mic1_not_prs = 0;
//            all_errors1 |= (1 << 5);
//            blink_en = 1;
//    }
    
   if(PTT1 == STATE_MIC_OFF){
            mic1_short = 0;
            mic1_not_prs = 0;
            all_errors1 &= ~(1 << 5);
            blink_en = 0;
            mic1_cut = 0;
    }
}

void mic2_state(unsigned int PTT2){
    
    if (PTT2 == STATE_MIC_PRESET && press2 == 0 && press_all == 0) {       // --- Handle MIC2 ---
        
        if(vol_set2 == 0){
            vol_set2 = 1;
            SetWiperValue(MCP4561_ADDR2, 0, config.mic2_volume);
            }
        dry_c = 0;
        line_test_on = 1;
        tmr_line_tst = 0;
        press2 = 1;
        mic2_not_prs = 0;
        //backlight_en = 0;
        BACK_LIGHT = 1;
        all_alarms |= (1 << 1);
        m2_inv = 1;

        switch (config.zone_sel2) {
            case 0:
                zone1State = 1;
                shiftRegisterData |= (1 << 8) | (1 << 9);
                shiftRegisterData &= ~(1u << 4);
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 2);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 2) | (1 << 3));
                break;
            case 1:
                zone2State = 1;
                shiftRegisterData |= (1 << 8) | (1 << 9);
                shiftRegisterData &= ~(1u << 5);
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 2);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 2) | (1 << 3));
                break;
            case 2:
                zone1State = 1;
                zone2State = 1;
                shiftRegisterData |= (1u << 8) | (1u << 9);
                shiftRegisterData &= ~((1u << 4) | (1u << 5));
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 2);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 2) | (1 << 3));
                jq8900_stop();
                break;
                
        }
    } else if (PTT2 == STATE_MIC_OK && mic2_not_prs == 0 && press_all == 0) {
        vol_set2 = 0;
        mic2_short = 0;
        mic2_cut = 0;
        line_test_on = 0;
        mic2_not_prs = 1;
        m2_inv = 0;
        all_alarms &= ~(1 << 1);
        all_errors1 &= ~(1 << 7);
        press2 = 0;
        dry_c = 1;
        if (config.zone_sel2 == 0 || config.zone_sel2 == 2)
            zone1State = 0;
        if (config.zone_sel2 == 1 || config.zone_sel2 == 2)
            zone2State = 0;

        if (mic1_not_prs)
            shiftRegisterData &= ~((1u << 2) | (1u << 3) | (1u << 8) | (1u << 9));
        else
            shiftRegisterData &= ~((1u << 2) | (1u << 3) | (1u << 9));
    }
    
    if (PTT2 == STATE_MIC_CUT ){
        mic2_cut = 1;
        all_errors1 |= (1 << 7);
        blink_en = 1;
        mic2_not_prs = 0;
    }
//    if(PTT2 == STATE_MIC_SHORT){
//        mic2_short = 1;
//        all_errors1 |= (1 << 7);
//        blink_en = 1;
//        mic2_not_prs = 0;
//    }
    
       if(PTT2 == STATE_MIC_OFF){
            mic2_short = 0;
            mic2_not_prs = 0;
            all_errors1 &= ~(1 << 7);
            blink_en = 0;
            mic2_cut = 0;
    }
}

void mic3_state(unsigned int PTT3){
    
        if (PTT3 == STATE_MIC_PRESET && press_all == 0) {
                
            if(vol_set3 == 0){
                vol_set3 = 1;
                //SetWiperValue(MCP4561_ADDR1, 0, config.mic3_volume);            // To change MIC3 ON MIC1 "MCP4561_ADDR1"
                SetWiperValue(MCP4561_ADDR2, 0, config.mic3_volume);            // To change MIC3 ON MIC2 "MCP4561_ADDR1"
            }
        press_all = 1;
        dry_c = 0;
        line_test_on = 1;
        tmr_line_tst = 0;
        //press3 = 1;
       // mic1_not_prs = 0;
        m3_inv = 1;
        BACK_LIGHT = 1;
        all_alarms |= (1 << 5);

        jq8900_stop();

        switch (config.zone_sel3) {    
 //======================================================MIC3 ON MIC2===========//           
            case 0:
                zone1State = 1;
                shiftRegisterData |= (1 << 8) | (1 << 9);
                shiftRegisterData &= ~(1u << 4);
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 2);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 2) | (1 << 3));
                break;
            case 1:
                zone2State = 1;
                shiftRegisterData |= (1 << 8) | (1 << 9);
                shiftRegisterData &= ~(1u << 5);
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 2);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 2) | (1 << 3));
                break;
            case 2:
                zone1State = 1;
                zone2State = 1;
                shiftRegisterData |= (1u << 8) | (1u << 9);
                shiftRegisterData &= ~((1u << 4) | (1u << 5));
                if (config.mode == 1)
                    shiftRegisterData |= (1 << 2);
                else if (config.mode == 2)
                    shiftRegisterData |= ((1 << 2) | (1 << 3));
                jq8900_stop();
                break;
                }                                                               // 
                
//===================================MIC3 ON MIC1========================================//

//            case 0:
//                if (config.mic1_mode == 0)
//                    shiftRegisterData &= ~(1u << 12);                           // AUX/MIC1
//                else
//                    shiftRegisterData |= (1 << 12);                             // MIC1/3/AUX
//
//                zone1State = 1;
//                shiftRegisterData |= (1 << 8) | (1 << 9);                       // MUTE - LED_EM ==1
//                shiftRegisterData &= ~(1u << 4);
//                // Audio output: single vs dual channel
//                if (config.mode == 1)
//                    shiftRegisterData |= (1 << 0);                              // CTRLA0 = 1,
//                else if (config.mode == 2)
//                    shiftRegisterData |= ((1 << 0) | (1 << 1));                 // CTRLA0 = 1, CTRLB1 = 1 MIC3    
//                break;
//            case 1:
//                if (config.mic1_mode == 0)
//                    shiftRegisterData &= ~(1u << 12);
//                else
//                    shiftRegisterData |= (1 << 12);
//
//                zone2State = 1;
//                shiftRegisterData |= (1 << 8) | (1 << 9);
//                shiftRegisterData &= ~(1u << 5);
//                if (config.mode == 1)
//                    shiftRegisterData |= (1 << 0);
//                else if (config.mode == 2)
//                    shiftRegisterData |= ((1 << 0) | (1 << 1));
//                break;
//            case 2:
//                if (config.mic1_mode == 0)
//                    shiftRegisterData &= ~(1u << 12);                           // LINE/MIC3
//                else
//                    shiftRegisterData |= (1 << 12);                             // MIC3/LINE
//
//                zone1State = 1;                                                 // Zone 1 - ON
//                zone2State = 1;                                                 // Zone 2 - ON
//                shiftRegisterData |= (1 << 8) | (1 << 9);                       // LED_EM - ON
//                shiftRegisterData &= ~((1u << 4) | (1u << 5));                  // AUDIO MSG 1/2 - OFF
//                if (config.mode == 1)
//                    shiftRegisterData |= (1 << 0);                              // Zone 1
//                else if (config.mode == 2)                                      
//                    shiftRegisterData |= ((1 << 0) | (1 << 1));                 // Zone 1-2
//                jq8900_stop();
//                break;
//        }

                
                
        // Clear unused audio channels
        shiftRegisterData &= ~((unsigned int)((1 << 2) | (1 << 3) | (1 << 4) |(1 << 5) | (1 << 6) | (1 << 7)));//shiftRegisterData &= ~((unsigned int)((1 << 2) | (1 << 3) | (1 << 4) |(1 << 5) | (1 << 6) | (1 << 7)));
                
        if (config.mode == 1) {
            
            // Enable audio for MIC3/MIC1.
            // Single channel: only left audio channel bit (bit0) is enabled.
//            shiftRegisterData |= (1 << 0);
//        } else if (config.mode == 2) {
//            // Dual channel: both left and right (bit0 and bit1).
//            shiftRegisterData |= ((1 << 0) | (1 << 1));
            
            // Enable audio for MIC3/MIC2.
          shiftRegisterData |= (1 << 2);
        } else if (config.mode == 2) {
            // Dual channel: both left and right (bit0 and bit1).
            shiftRegisterData |= ((1 << 2) | (1 << 3));
        }
        zone1State = 1;
        zone2State = 1;
        
    } else if (PTT3 == STATE_MIC_OK && press_all == 1) {
        press_all = 0;
        mic3_cut = 0;
        mic3_short = 0;
        m3_inv = 1;
        vol_set3 = 0;
        all_errors1 &= ~(1 << 3);
        // Clear audio bits for MIC3/MIC1.
        if (config.mode == 1) {
            shiftRegisterData &= ~(1u << 0);                                    
        } else if (config.mode == 2) {
            shiftRegisterData &= ~((1u << 0) | (1u << 1));                      
        }
        shiftRegisterData &= ~((unsigned int)(1 << 8 | 1 << 9));
        zone1State = 0;
        zone2State = 0;
        all_alarms &= ~(1 << 5);
    }

    // MIC3 error handling
    if (PTT3 == STATE_MIC_CUT) {
        all_errors1 |= (1 << 3);
        blink_en = 1;
        mic3_cut = 1;
    }
    if(PTT3 == STATE_MIC_SHORT){
        all_errors1 |= (1 << 3);
        blink_en = 1;
        mic3_short = 1;
    }
    
       if(PTT3 == STATE_MIC_OFF){
            mic3_short = 0;
            all_errors1 &= ~(1 << 3);
            blink_en = 0;
            mic3_cut = 0;
    }
}

void handleDryCont_3_4(unsigned char st) {
    unsigned char statePTT3 = (m1_inv == 0 && m2_inv == 0) ? PTT_3() : STATE_OFF_ptt;
    unsigned char statePTT4 = (m1_inv == 0 && m2_inv == 0) ? PTT_4() : STATE_OFF_ptt;

    if (st && press_all == 0) {

        if (statePTT3 == STATE_ON_ptt) {
            line_test_on = 1;
            tmr_line_tst = 0;
            press3 = 1;
            //_backlight_tmr_en_(0, 1);
            BACK_LIGHT = 1;
            all_alarms |= (1 << 2); // Dry1 Cont Alarm
            dry_cont_state(0);
        } else if (press3 == 1) {
            line_test_on = 0;
            press3 = 0;
            jq8900_stop();
            all_alarms &= ~(1 << 2); // Clear Dry1 Cont Alarm
            shiftRegisterData &= ~((unsigned int) ((1 << 4) | (1 << 5) | (1 << 8) | (1 << 9))); // Clear PTT3-related bits
            if (statePTT3 && statePTT4 == STATE_OFF_ptt) {
                zone1State = 0;
                zone2State = 0;
            }

        }

        if (statePTT4 == STATE_ON_ptt) {
            line_test_on = 1;
            tmr_line_tst = 0;
            press4 = 1;
            //_backlight_tmr_en_(0, 1);
            BACK_LIGHT = 1;
            all_alarms |= (1 << 3); // Dry2 Cont Alarm
            dry_cont_state(1);

        } else if (press4 == 1) {
            line_test_on = 0;
            press4 = 0;
            jq8900_stop();
            all_alarms &= ~(1 << 3); // Clear Dry2 Cont Alarm
            shiftRegisterData &= ~((unsigned int) ((1 << 4) | (1 << 5) | (1 << 8) | (1 << 9))); // Clear PTT4-related bits
            if (statePTT3 && statePTT4 == STATE_OFF_ptt) {
                zone1State = 0;
                zone2State = 0;
            }
        }

        action(zone1State, zone2State, shiftRegisterData);

    }
}

void dry_cont_state(unsigned char st) {

    switch (counter[st].Zones) {
        case 0: shiftRegisterData = 0b0001011100010000;
            zone1State = 1;
            break;
        case 1: shiftRegisterData = 0b0001011100100000;
            zone2State = 1;
            break;
        case 2: shiftRegisterData = 0b0001011100110000;
            zone1State = 1;
            zone2State = 1;
            break;
        case 3: shiftRegisterData = 0b0001011100000000;
            break;
    }
    switch (counter[st].MSG) {
        case 1: case 2: case 3: case 4: case 5:
            if (is_module_busy() == 0) {
                jq8900_playTrack(counter[st].MSG);
            }
            break;
        case 6:
            SendShiftData595(0b0001011111000000);
            break;
        default:
            break;
    }
}



void loadMic1Thresholds(void) {
    // ?????? ??? ?????? ?? EEPROM
    mic1Thresholds.ok_min = EEPROM_Read16(EEPROM_ADDR_ok_min);
    mic1Thresholds.ok_max = EEPROM_Read16(EEPROM_ADDR_ok_max);
    mic1Thresholds.pr_min = EEPROM_Read16(EEPROM_ADDR_pr_min);
    mic1Thresholds.pr_max = EEPROM_Read16(EEPROM_ADDR_pr_max);
    mic1Thresholds.ct_min = EEPROM_Read16(EEPROM_ADDR_ct_min);
    mic1Thresholds.ct_max = EEPROM_Read16(EEPROM_ADDR_ct_max);
    mic1Thresholds.sh_min = EEPROM_Read16(EEPROM_ADDR_sh_min);
    mic1Thresholds.sh_max = EEPROM_Read16(EEPROM_ADDR_sh_max);

    // ????????: ?????? ?????? (EEPROM ?????? ??? ???????? = 0xFFFF)
    if ((mic1Thresholds.ok_min == 0xFFFF && mic1Thresholds.ok_max == 0xFFFF) ||
        (mic1Thresholds.ok_min == 0 && mic1Thresholds.ok_max == 0)) {
        
        mic1Thresholds = mic1ThresholdsDefault; // ?????? ????????? ????????
         __delay_ms(100);
        // ????????? ????????? ?????? ? EEPROM
        EEPROM_Write16(EEPROM_ADDR_ok_min, 480);
        EEPROM_Write16(EEPROM_ADDR_ok_max, 515);
        EEPROM_Write16(EEPROM_ADDR_pr_min, 300);
        EEPROM_Write16(EEPROM_ADDR_pr_max, 350);
        EEPROM_Write16(EEPROM_ADDR_ct_min, 900);
        EEPROM_Write16(EEPROM_ADDR_ct_max, 1023);
        EEPROM_Write16(EEPROM_ADDR_sh_min, 0);
        EEPROM_Write16(EEPROM_ADDR_sh_max, 5);
    }
}

void saveMic1ThresholdsToEEPROM(void) {
    EEPROM_Write16(EEPROM_ADDR_ok_min, mic1Thresholds.ok_min);
    EEPROM_Write16(EEPROM_ADDR_ok_max, mic1Thresholds.ok_max);
    EEPROM_Write16(EEPROM_ADDR_pr_min, mic1Thresholds.pr_min);
    EEPROM_Write16(EEPROM_ADDR_pr_max, mic1Thresholds.pr_max);
    EEPROM_Write16(EEPROM_ADDR_ct_min, mic1Thresholds.ct_min);
    EEPROM_Write16(EEPROM_ADDR_ct_max, mic1Thresholds.ct_max);
    EEPROM_Write16(EEPROM_ADDR_sh_min, mic1Thresholds.sh_min);
    EEPROM_Write16(EEPROM_ADDR_sh_max, mic1Thresholds.sh_max);
}









//unsigned int detectStatePTT(unsigned char zone_sel, char adcChannel) {
//    if (zone_sel < 3) {
//        int adcValue = readADC(adcChannel);
//
//        if (adcValue >= 480 && adcValue <= 515) {
//            return STATE_MIC_OK;
//        } else if (adcValue >= 300 && adcValue <= 350) {
//            return STATE_MIC_PRESET;
//        } else if (adcValue >= 900 && adcValue <= 1023) {
//            return STATE_MIC_CUT;
//        }
//        else if (adcValue >= 0 && adcValue <= 0x0005) {
//            return STATE_MIC_SHORT;
//        }
//    }
//    return STATE_MIC_OFF; // Default return value
//}

//void handleMIC3(void){
//    
//    if(config.zone_sel3 == 3){
//        all_errors1 &= ~(1 << 3);
//        mic3_cut = 0;
//        mic3_short = 0;
//        return;
//    }
//        
//unsigned int statePTT3 = detectStatePTT(0, 3);                                  // MIC3 (PIN24/AN4)
//
//        if (statePTT3 == STATE_MIC_PRESET && press_all == 0) {
//                
//            if(vol_set3 == 0){
//                vol_set3 = 1;
//                //SetWiperValue(MCP4561_ADDR1, 0, config.mic3_volume);            // To change MIC3 ON MIC1 "MCP4561_ADDR1"
//                SetWiperValue(MCP4561_ADDR2, 0, config.mic3_volume);            // To change MIC3 ON MIC2 "MCP4561_ADDR1"
//            }
//        press_all = 1;
//        dry_c = 0;
//        line_test_on = 1;
//        tmr_line_tst = 0;
//        press1 = 1;
//        mic1_not_prs = 0;
//        m3_inv = 1;
//        BACK_LIGHT = 1;
//        all_alarms |= (1 << 5);
//
//        jq8900_stop();
////======================================================MIC3 ON MIC2===========//
//        switch (config.zone_sel3) {            
//            case 0:
//                zone1State = 1;
//                shiftRegisterData |= (1 << 8) | (1 << 9);
//                shiftRegisterData &= ~(1u << 4);
//                if (config.mode == 1)
//                    shiftRegisterData |= (1 << 2);
//                else if (config.mode == 2)
//                    shiftRegisterData |= ((1 << 2) | (1 << 3));
//                break;
//            case 1:
//                zone2State = 1;
//                shiftRegisterData |= (1 << 8) | (1 << 9);
//                shiftRegisterData &= ~(1u << 5);
//                if (config.mode == 1)
//                    shiftRegisterData |= (1 << 2);
//                else if (config.mode == 2)
//                    shiftRegisterData |= ((1 << 2) | (1 << 3));
//                break;
//            case 2:
//                zone1State = 1;
//                zone2State = 1;
//                shiftRegisterData |= (1u << 8) | (1u << 9);
//                shiftRegisterData &= ~((1u << 4) | (1u << 5));
//                if (config.mode == 1)
//                    shiftRegisterData |= (1 << 2);
//                else if (config.mode == 2)
//                    shiftRegisterData |= ((1 << 2) | (1 << 3));
//                jq8900_stop();
//                break;
//                }                                                               // 
//                
////===================================MIC3 ON MIC1========================================//
//
////            case 0:
////                if (config.mic1_mode == 0)
////                    shiftRegisterData &= ~(1u << 12);                           // AUX/MIC1
////                else
////                    shiftRegisterData |= (1 << 12);                             // MIC1/3/AUX
////
////                zone1State = 1;
////                shiftRegisterData |= (1 << 8) | (1 << 9);                       // MUTE - LED_EM ==1
////                shiftRegisterData &= ~(1u << 4);
////                // Audio output: single vs dual channel
////                if (config.mode == 1)
////                    shiftRegisterData |= (1 << 0);                              // CTRLA0 = 1,
////                else if (config.mode == 2)
////                    shiftRegisterData |= ((1 << 0) | (1 << 1));                 // CTRLA0 = 1, CTRLB1 = 1 MIC3    
////                break;
////            case 1:
////                if (config.mic1_mode == 0)
////                    shiftRegisterData &= ~(1u << 12);
////                else
////                    shiftRegisterData |= (1 << 12);
////
////                zone2State = 1;
////                shiftRegisterData |= (1 << 8) | (1 << 9);
////                shiftRegisterData &= ~(1u << 5);
////                if (config.mode == 1)
////                    shiftRegisterData |= (1 << 0);
////                else if (config.mode == 2)
////                    shiftRegisterData |= ((1 << 0) | (1 << 1));
////                break;
////            case 2:
////                if (config.mic1_mode == 0)
////                    shiftRegisterData &= ~(1u << 12);                           // LINE/MIC3
////                else
////                    shiftRegisterData |= (1 << 12);                             // MIC3/LINE
////
////                zone1State = 1;                                                 // Zone 1 - ON
////                zone2State = 1;                                                 // Zone 2 - ON
////                shiftRegisterData |= (1 << 8) | (1 << 9);                       // LED_EM - ON
////                shiftRegisterData &= ~((1u << 4) | (1u << 5));                  // AUDIO MSG 1/2 - OFF
////                if (config.mode == 1)
////                    shiftRegisterData |= (1 << 0);                              // Zone 1
////                else if (config.mode == 2)                                      
////                    shiftRegisterData |= ((1 << 0) | (1 << 1));                 // Zone 1-2
////                msg_stop();
////                break;
////        }
//
//                
//                
////        // Clear unused audio channels
////        shiftRegisterData &= ~((unsigned int)((1 << 2) | (1 << 3) | (1 << 4) |
////                                                (1 << 5) | (1 << 6) | (1 << 7)));
////                // Enable audio for MIC3/MIC1.
////        if (config.mode == 1) {
////            // Single channel: only left audio channel bit (bit0) is enabled.
////            shiftRegisterData |= (1 << 0);
////        } else if (config.mode == 2) {
////            // Dual channel: both left and right (bit0 and bit1).
////            shiftRegisterData |= ((1 << 0) | (1 << 1));
////        }
////        zone1State = 1;
////        zone2State = 1;
//        
//    } else if (statePTT3 == STATE_MIC_OK && press_all == 1) {
//        press_all = 0;
//        mic3_cut = 0;
//        mic3_short = 0;
//        m3_inv = 1;
//        vol_set3 = 0;
//        all_errors1 &= ~(1 << 3);
//        // Clear audio bits for MIC3/MIC1.
//        if (config.mode == 1) {
//            shiftRegisterData &= ~(1u << 0);                                    
//        } else if (config.mode == 2) {
//            shiftRegisterData &= ~((1u << 0) | (1u << 1));                      
//        }
//        shiftRegisterData &= ~((unsigned int)(1 << 8 | 1 << 9));
//        zone1State = 0;
//        zone2State = 0;
//        all_alarms &= ~(1 << 5);
//    }
//
//    // MIC3 error handling
//    if (statePTT3 == STATE_MIC_CUT) {
//        all_errors1 |= (1 << 3);
//        blink_en = 1;
//        mic3_cut = 1;
//    }
//    if(statePTT3 == STATE_MIC_SHORT){
//        all_errors1 |= (1 << 3);
//        blink_en = 1;
//        mic3_short = 1;
//    }
////    } else {
////        all_errors1 &= ~(1 << 3);
////        mic3_cut = 0;
////        blink_en = 0;
////    }
//}        

