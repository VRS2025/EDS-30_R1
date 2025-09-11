#include <xc.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>   
#include <pic18F46K22.h>
#include "user.h"
#include "LCD_ST7565.h"
#include "logos.h"
#include "mcp4561.h"
#include "i2c1.h"


MicThresholds mic1Thresholds = {480,515, 300,344, 900,1023, 0,5};


void display_zone_image(unsigned char zone_sel, unsigned char y_pos, unsigned char m_neg) {
    switch (zone_sel) {
        case 0: img_view(30, y_pos, 30, _zone_1_HE, m_neg); break;
        case 1: img_view(30, y_pos, 30, _zone_2_HE, m_neg); break;
        case 2: img_view(30, y_pos, 30, _zone_1_2_HE, m_neg); break;
        case 3: img_view(30, y_pos, 30, _mic_off_HE, m_neg); break;
    }
}

void menu1(void) {                                                              // Mic settings
    if (m1 == 1) {
        unsigned char y = 0;
        unsigned char tr[3] = {1, 0, 0};  // tr1, tr2, tr3
        unsigned char m_neg[3] = {1, 0, 0};  // m_neg1, m_neg2, m_neg3
        unsigned char backup_zone_sel[3] = {config.zone_sel1, config.zone_sel2, config.zone_sel3};
        unsigned char _mic1_mode = 0;
        
        _mic1_mode = config.mic1_mode;
        lcd_clear();
        Display_Picture(_mic_menu_1_HE);

        while (m1 == 1) {
            // Handle right button clicks (increment zone selection)
            if (_right1.click == 1) {
                _right1.click = 0;
                if (y == 0){config.zone_sel1++;if (config.zone_sel1 >= 3){config.zone_sel1 = 0;_mic1_mode ^= 1 << 0;}}                   
                if (y == 1) config.zone_sel2 = (config.zone_sel2 + 1) % 3;
                //if (y == 2){if(config.zone_sel3 == 3){config.zone_sel3 = 2;}}
                if (y == 2){if(config.mode == 2){config.zone_sel3 =(config.zone_sel3 + 1) % 3;}else{if (y == 2){if(config.zone_sel3 == 3){config.zone_sel3 = 2;}}}}   
            }

            // Handle left button clicks (decrement zone selection)
            if (_right2.click == 1) {
                _right2.click = 0;

                if (y == 0){config.zone_sel1--;if (config.zone_sel1 == 255){config.zone_sel1 = 3;_mic1_mode ^= 1 << 0;}}                   
                if (y == 1) config.zone_sel2 = (config.zone_sel2 == 0) ? 3 : config.zone_sel2 - 1;
                if (y == 2){
                    if(config.mode == 2){config.zone_sel3 = (config.zone_sel3 == 0) ? 3 : config.zone_sel3 - 1;}else{if (y == 2) {if(config.zone_sel3 == 2){config.zone_sel3 = 3;}}}}    
            }

            // Handle menu navigation with left button
            if (_left1.click == 1) {
                _left1.click = 0;
                y = (y + 1) % 3;

                for (int i = 0; i < 3; i++) {
                    tr[i] = (i == y) ? 1 : 0;
                    m_neg[i] = (i == y) ? 1 : 0;
                }

                if (y == 0) Display_Picture(_mic_menu_1_HE);
                if (y == 1) Display_Picture(_mic_menu_2_HE);
                if (y == 2) Display_Picture(_mic_menu_3_HE);
            }

            // Display zone images using a single function for each zone
            display_zone_image(config.zone_sel1, 2, m_neg[0]);
            display_zone_image(config.zone_sel2, 4, m_neg[1]);
            display_zone_image(config.zone_sel3, 6, m_neg[2]);

            // Display zone labels
            if (_mic1_mode == 1) {
                lcd_gotoxy(60, 2);
                lcd_putstr(" - M1", m_neg[0]);
                config.mic1_mode = 1;
                shiftRegisterData |= (1 << 12);                                  // MIC1/3/AUX
                SendShiftData595(shiftRegisterData);
            } else {
                lcd_gotoxy(60, 2);
                lcd_putstr(" - A1", m_neg[0]);
                config.mic1_mode = 0;
                shiftRegisterData &= ~(1u << 12);                                // AUX/MIC1
                SendShiftData595(shiftRegisterData);
            }
            lcd_gotoxy(60, 4); lcd_putstr(" - M2", m_neg[1]);
            lcd_gotoxy(60, 6); lcd_putstr(" - M3", m_neg[2]);

            // Save zone selection to EEPROM on exit
            if (_left2.click == 1 || m1 == 0) {
                _left2.click = 0;

                if (backup_zone_sel[0] != config.zone_sel1) {
                    EEPROM_Write(EEPROM_ADDR_MIC1, config.zone_sel1);
                    config.zone_sel1 = EEPROM_Read(EEPROM_ADDR_MIC1);
                }
                if (backup_zone_sel[1] != config.zone_sel2) {
                    EEPROM_Write(EEPROM_ADDR_MIC2, config.zone_sel2);
                    config.zone_sel2 = EEPROM_Read(EEPROM_ADDR_MIC2);
                }
                if (backup_zone_sel[2] != config.zone_sel3) {
                    EEPROM_Write(EEPROM_ADDR_MIC3, config.zone_sel3);
                    config.zone_sel3 = EEPROM_Read(EEPROM_ADDR_MIC3);
                }
                EEPROM_Write(EEPROM_ADDR_AUX , config.mic1_mode);
                m1 = 0;
                tmr_drop_menu(1);
            }
        }
    }
}

void menu3(void) {                                                              // DRY contact menu
    
unsigned char y = 0; 
unsigned char tr1 = 0;
unsigned char tr2 = 0;
unsigned char tr3 = 0;
unsigned char tr4 = 0;
unsigned char neg1 = 1;
unsigned char neg2 = 0;
unsigned char neg3 = 0;
unsigned char neg4 = 0;

unsigned char dry1_sel;
unsigned char dry2_sel;
unsigned char msg1_sel;
unsigned char msg2_sel;

dry1_sel = counter[0].Zones;
msg1_sel = counter[0].MSG;

dry2_sel = counter[1].Zones;
msg2_sel = counter[1].MSG;

lcd_clear();
Display_Picture(dry_menu_1_HE);

    while(m3==1) {

        if (_right1.click == 1) {
            _right1.click = 0;
            if (y == 0x0) {
                dry1_sel++;
                if (dry1_sel == 4) {
                    dry1_sel = 0;
                }
            } else if (y == 1) {
                msg1_sel++;
                if (msg1_sel == 7) {
                    msg1_sel = 1;
                }
            } else if (y == 2) {
                dry2_sel++;
                if (dry2_sel == 4) {
                    dry2_sel = 0;
                }
            } else if (y == 3) {
                 msg2_sel++;
                if (msg2_sel == 7) {
                    msg2_sel = 1;
                }
            }


        }

        if (_right2.click == 1) {
            _right2.click = 0;
            if (y == 0x0) {
                dry1_sel--;
                if (dry1_sel == 0) {
                    dry1_sel = 3;
                }
            } else if (y == 1) {
                msg1_sel--;
                if (msg1_sel == 255) {
                    msg1_sel = 6;
                }
            } else if (y == 2) {
                dry2_sel--;
                if (dry2_sel == 0) {
                    dry2_sel = 3;
                }
            } else if (y == 3) {
                msg2_sel--;
                if (msg2_sel == 255) {
                    msg2_sel = 6;
                }
            }


        }
               
        if (_left1.click == 1) { // 
            _left1.click = 0;
            y++;
            if(y >3 ){
                y = 0;
            }
            if (y == 0 && tr1 == 0) {
                Display_Picture(dry_menu_1_HE);
                tr1 = 1;
                tr2 = 0;
                tr3 = 0;
                tr4 = 0;
                neg1 = 1;
                neg2 = 0;
                neg3 = 0;
                neg4 = 0;
            }else if (y == 1 && tr2 == 0) {
                Display_Picture(dry_menu_2_HE);
                tr1 = 0;
                tr2 = 1;
                tr3 = 0;
                tr4 = 0;
                neg1 = 0;
                neg2 = 1;
                neg3 = 0;
                neg4 = 0;
            }else if (y == 2 && tr3 == 0) {
                Display_Picture(dry_menu_3_HE);
                tr1 = 0;
                tr2 = 0;
                tr3 = 1;
                tr4 = 0;
                neg1 = 0;
                neg2 = 0;
                neg3 = 1;
                neg4 = 0;
            }else if (y == 3 && tr4 == 0) {
                Display_Picture(dry_menu_4_HE);
                tr1 = 0;
                tr2 = 0;
                tr3 = 0;
                tr4 = 1;
                neg1 = 0;
                neg2 = 0;
                neg3 = 0;
                neg4 = 1;
            }  

        }

         switch (dry1_sel) {

            case 0: img_view(64, 2, 30, _zone_1_HE, neg1);
                    counter[0].Zones= 0;
                break;
            case 1: img_view(64, 2, 30, _zone_2_HE, neg1);
                    counter[0].Zones = 1;
                break;
            case 2: img_view(64, 2, 30, _zone_1_2_HE, neg1);
                    counter[0].Zones = 2;
                break;
            case 3: img_view(64, 2, 30, _mic_off_HE, neg1);
                    counter[0].Zones = 3;
                    msg1_sel = 0;
                break;

         }
         
         switch (msg1_sel) {
             
            case 0: img_view(22, 2, 30, _mic_off_HE,neg2);
                    counter[0].MSG = 0;
                break;
            case 1: img_view(22, 2, 32, msg_1_HE, neg2);
                    counter[0].MSG = 1;
                break;
            case 2: img_view(22, 2, 32, msg_2_HE, neg2);
                    counter[0].MSG = 2;
                break;
            case 3: img_view(22, 2, 32, msg_3_HE, neg2);
                    counter[0].MSG = 3;
                break;
            case 4: img_view(22, 2, 32, msg_4_HE, neg2);
                    counter[0].MSG = 4;
                break;
            case 5: img_view(22, 2, 32, msg_5_HE, neg2);
                    counter[0].MSG = 5;
                break;
//            case 6: img_view(22, 2, 32, a_in_HE, neg2);
//                    counter[0].MSG = 6;
                break;
         }
            
         switch (dry2_sel) {

            case 0: img_view(64, 5, 30, _zone_1_HE, neg3);
                    counter[1].Zones= 0;
                break;
            case 1: img_view(64, 5, 30, _zone_2_HE, neg3);
                    counter[1].Zones= 1;
                break;
            case 2: img_view(64, 5, 30, _zone_1_2_HE, neg3);
                    counter[1].Zones= 2;
                break;
            case 3: img_view(64, 5, 30, _mic_off_HE, neg3);
                    counter[1].Zones= 3;
                    msg2_sel = 0;
                break;

         }
         
         
         switch (msg2_sel) {

            case 0: img_view(22, 5, 30, _mic_off_HE, neg4);
                    counter[1].MSG = 0;
                break;
            case 1: img_view(22, 5, 32, msg_1_HE, neg4);
                    counter[1].MSG = 1;
                break;
            case 2: img_view(22, 5, 32, msg_2_HE, neg4);
                    counter[1].MSG = 2;
                break;
            case 3: img_view(22, 5, 32, msg_3_HE, neg4);
                    counter[1].MSG = 3;
                break;
            case 4: img_view(22, 5, 32, msg_4_HE, neg4);
                    counter[1].MSG = 4;
                break;
            case 5: img_view(22, 5, 32, msg_5_HE, neg4);
                    counter[1].MSG = 5;
                break;
//            case 6: img_view(22, 5, 32, a_in_HE, neg4);
//                    counter[1].MSG = 6;
                    
                break;
         }
         
        if (_left2.click == 1 || m3 == 0) {
            _left2.click = 0;
            
            EEPROM_Write(EEPROM_ADDR_dry1 , counter[0].Zones);
            EEPROM_Write(EEPROM_ADDR_msg1 , counter[0].MSG);
                    
            EEPROM_Write(EEPROM_ADDR_dry2 , counter[1].Zones);
            EEPROM_Write(EEPROM_ADDR_msg2 , counter[1].MSG);
            

            
            m3 = 0;
            one_step = 0;
            tmr_drop_menu(1);
        }
         
    }
}

void menu4(void) { // Sys settings

    unsigned char selected[5] = {0}; // Selection flags
    
    if (m4 == 1) {
        lcd_clear();
        first_screen();

        while (m4 == 1) {
            memset(selected, 0, sizeof(selected)); // Reset selection
            if (x < 5) selected[x] = 1; // Set selected option

            // Display menu options
            img_view(24, 2, 80, _audio_50, selected[0]);
            img_view(24, 3, 80, _mic_set_80, selected[1]);
            img_view(24, 4, 80, _rs_485_HE, selected[2]);
            img_view(24, 5, 80, reset_HE, selected[3]);
            img_view(24, 6, 80, about__HE, selected[4]);

            // Right button (next)
            if (_right2.click == 1) {
                _right2.click = 0;
                if (x < 4) x++;
            }

            // Left button (previous)
            if (_right1.click == 1) {
                _right1.click = 0;
                if (x > 0) x--;
            }

            // Enter button (select option)
            if (_left1.click == 1) {
                _left1.click = 0;
                lcd_clear();
                
                switch (x) {
                    case 0: _audio_sett(); break;
                    case 1: contr = 0; adc_menu_MIC1(); break;
                    case 2: contr = 0; _rs_comm_(); break;
                    case 3: // Reset confirmation
                            Display_Picture(erase_menu);
                            unsigned int hold_time = 10000;
                            unsigned char both_pressed = 0;

                            while (hold_time > 0) {
                                lcd_gotoxy(64, 7);    
                                lcd_putch((hold_time / 1000) + '0', 0);
                                if (PORTEbits.RE0 == 0 && PORTEbits.RE2 == 0) {
                                    if (!both_pressed) {
                                        both_pressed = 1;
                                        _resete_all();
                                    }
                                } else {
                                    both_pressed = 0;
                                }
                                __delay_ms(1000);
                                hold_time -= 1000;
                            }
                            lcd_clear();
                            lcd_gotoxy(30, 5);
                            lcd_putstr("Returning...", 0);
                            __delay_ms(1000);
                            first_screen();
                            break;
                    case 4: _about(); break;
                }
            }

            // Exit menu
            if (_left2.click == 1 || m4 == 0) {
                _left2.click = 0;
                m4 = 0;
                backlight_tmr = 0;
                exit_en = 1;
                tmr_drop_menu(1);
            }
        }
    }
}

void _rs_comm_(void){

    unsigned char press_ = 0;
    unsigned char backup_addr = config._485;
    unsigned char slave_count = config.slave_count; // Variable to store number of slaves
    unsigned char in_master_mode = 0; // Variable to track if in Master mode
    unsigned char x = 0;
    unsigned char neg1 = 0;
    unsigned char neg2 = 0;
    rs_ = 0;                                                                    // start rs_menu

    // Array to hold the address strings
    const char *addresses[] = {
        "   OFF  ", "    1   ", "    2   ", "    3   ", "    4   ", "    5   ", "    6   ", "    7   ", "    8   ", "    9   ",
        "   10   ", "   11   ", "   12   ", "   13   ", "   14   ", "   15   ", "   16   ", "   17   ", "   18   ", "   19   ",
        "   20   ", " Master " // Address 21
    };
    lcd_gotoxy(0,1); lcd_putstr("OK",0);
    //_backlight_tmr_en_(0,1);
    BACK_LIGHT = 1;
    lcd_gotoxy(0,6); lcd_putstr("X",0);
    //img_view(0,6,9,_back,0); //*
    img_view(115,1,9,_up,0);  
    img_view(115,6,9,_down,0);
    img_view(30,0,70,rs_70,1);
    img_view(75,2,34,addr_34,0);

    while(rs_ != 1){   
        
        if (x == 0) { neg1 = 1; neg2 = 0;}
   else if (x == 1) { neg1 = 0; neg2 = 1;}
        // Display the selected address
        lcd_gotoxy(20, 2); 
        sprintf(scr_buff, "%s", addresses[config._485]);
        lcd_putstr(scr_buff, neg1);

        // If the selected address is "Master" (21), allow slave count selection
        if(config._485 == 21){
             in_master_mode = 1;// Set Master mode flag

            // Show slave count only in "Master" mode
            lcd_gotoxy(20, 4); 
            sprintf(scr_buff, "Slaves: %02d", slave_count);
            lcd_putstr(scr_buff, neg2);
            x = 1;
        } else {
            x = 0;
            in_master_mode = 0;
            lcd_gotoxy(20, 4); 
            lcd_putstr("              ", 0); // Clear slave count display if not in Master mode
        }

        // Increment the address or slave count
        if (_right1.click == 1) {
            _right1.click = 0;
            if (x == 0) {
                config._485++;
                if (config._485 > 21) config._485 = 0; // Wrap around address selection
            }
            if (x == 1 && in_master_mode) { // Allow slave count selection only in Master mode
                slave_count++;
                if (slave_count > 21) slave_count = 1; // Wrap around slave count
            }
        }

        // Decrement the address or slave count
        if (_right2.click == 1) {
            _right2.click = 0;
            if (x == 0) {
                config._485--;
                if (config._485 == 255 || config._485 == 0) config._485 = 21; // Wrap around address selection
            }
            if (x == 1 && in_master_mode) { // Allow slave count selection only in Master mode
                slave_count--;
                if (slave_count < 1) slave_count = 20; // Wrap around slave count
            }
        }

        // Toggle between address selection and slave count mode
        if (_left2.click == 1){
            _left2.click = 0;
            in_master_mode = 0; // Reset Master mode flag
            config._485 = backup_addr; // Restore the previous address selection
            if(x==0){x = 1;}else{x = 0;config._485 = 0;}
            continue; // Return to the address selection without saving changes
        }

        // Exit the menu and save the selection
        if (_left1.click == 1){
            _left1.click = 0;
            if (backup_addr != config._485 || config.slave_count != slave_count) {
                EEPROM_Write(EEPROM_ADDR_485, config._485);
                config.slave_count = slave_count;
                EEPROM_Write(EEPROM_ADDR_SLAVE_COUNT, slave_count); // Store the number of slaves
            }
            rs_ = 1; // Exit the loop
            lcd_clear();
            first_screen(); // Return to the first screen
        }
    }
}

void _resete_all(void){
     
     unsigned char addr = 0x02;
     
     for (addr = 0x02; addr <= 0x50; addr++) {

            EEPROM_Write(addr, 0xff);
         }
     addr = 0x00;
        asm("RESET");
        __delay_ms(500);
        __delay_ms(500);
        __delay_ms(500);
        __delay_ms(500);
    }
 
void _about(void){
 lcd_clear();
 m_a = 0;
 img_view(0, 6, 9, _back, 0);
 img_view(40, 0, 49, _PA__HE,   1);
 img_view(100, 2, 25, rev__HE,   0);
 img_view(100, 4, 17, prod__HE,   0);
 img_view(100, 6, 30, date__HE,   0);
 lcd_gotoxy(50,2);
 lcd_putstr(" 1.1.1 ",0);
 lcd_gotoxy(50,4);
 lcd_putstr(" HVR ",0);
 lcd_gotoxy(10,6);
 lcd_putstr("  jan 2025",0);
 //lcd_gotoxy(10,7);
 //lcd_putstr("www.dpas.co.il",0);
 while(m_a !=1){

    if(_right1.click == 1){
        _right1.click = 0;
        adc_monitor();

    }
    if(_right2.click == 1){
        _right2.click = 0;
        sys_timer_set();

    }
     
       if(_left2.click == 1){
        _left2.click = 0;
        m_a = 1;
        lcd_clear();
        first_screen();
        //Display_Picture(sys_sett_HE);
    }
  }  
     
 }

void _audio_sett(void) {
    
    unsigned char neg1 = 0;
    unsigned char neg2 = 0;
    unsigned char neg3 = 0;
    unsigned char neg4 = 0;
    unsigned char x = 0;
    unsigned char play_ = 0;
    unsigned char press_1 = 0;
    aud  = 0;
    
    unsigned char _mgs_vol_backup = config.msg_volume;
    unsigned char _mic1_vol_backup = config.mic1_volume;
    unsigned char _mic2_vol_backup = config.mic2_volume;
    unsigned char _mic3_vol_backup = config.mic3_volume;
    
  //  config.mic1_mode = EEPROM_Read(EEPROM_ADDR_AUX);
    
   if(config.mic1_mode == 0){
       
      shiftRegisterData &= ~((unsigned int) (1 << 12));           // AUX/MIC1
      
   }else{
       
       shiftRegisterData |= ((unsigned int) (1 << 12));            // MIC1/AUX
       
        }

    
    // Set initial gains for MIC1, MIC2, and MIC3 via MCP4561
   
    SetWiperValue(MCP4561_ADDR1, 0, config.mic1_volume);
    SetWiperValue(MCP4561_ADDR2, 0, config.mic2_volume);

    // Your existing UI setup code
    //_backlight_tmr_en_(0,1);
    BACK_LIGHT = 1;
    tmr_line_tst = 0;
    adc_en = 0;
    img_view(30,0,70,_audio_70,1);

    img_view(0,6,9,_back,0);
    lcd_gotoxy(0,1); lcd_putstr("OK",0);
    img_view(115,1,9,_up,0);
    img_view(115,6,9,_down,0);

    while(aud != 1) {
        // Highlight the selected option
        if (x == 0) { neg1 = 0; neg2 = 0; neg3 = 0; neg4 = 0;}
   else if (x == 1) { neg1 = 1; neg2 = 0; neg3 = 0; neg4 = 0;}
   else if (x == 2) { neg1 = 0; neg2 = 1; neg3 = 0; neg4 = 0;}
   else if (x == 3) { neg1 = 0; neg2 = 0; neg3 = 1; neg4 = 0;}
   else if (x == 4) { neg1 = 0; neg2 = 0; neg3 = 0; neg4 = 1;}

        // Right button click to increase volume
        if (_right2.click == 1) {
            _right2.click = 0;
            if (x == 1) {
  //              config.mic1_volume += 5;
                if (config.mic1_volume >= 255) {
                    config.mic1_volume = 0xFF;
                }else{config.mic1_volume += 5;}
                
                SetWiperValue(MCP4561_ADDR1, 0, config.mic1_volume);
            }
            if (x == 2) {
                //config.mic2_volume += 5;
                if (config.mic2_volume >= 255) {
                    config.mic2_volume = 0xFF;
                }else{config.mic2_volume += 5;}
                
               SetWiperValue(MCP4561_ADDR2, 0, config.mic2_volume);
            }
            if (x == 3) {
               // config.mic3_volume += 5;
                if (config.mic3_volume >= 255) {
                    config.mic3_volume = 0xFF;
                }else{config.mic3_volume += 5;}
                
                //SetWiperValue(MCP4561_ADDR1, 0, config.mic3_volume);            // To change MIC3 ON MIC1 "MCP4561_ADDR1"
                SetWiperValue(MCP4561_ADDR2, 0, config.mic3_volume);            // To change MIC3 ON MIC2 "MCP4561_ADDR1"
            }
            if (x == 4) {
                
                if (config.msg_volume == 255 || config.msg_volume == 0) {
                    config.msg_volume = 0;
                }else{config.msg_volume--;}
                jq8900_setVolume(config.msg_volume);  // Adjust message volume
            }
        }

        // Left button click to decrease volume
        if (_right1.click == 1) {
            _right1.click = 0;
            if (x == 1) {
             //   config.mic1_volume -= 5;
                if (config.mic1_volume < 5) {
                    config.mic1_volume = 0x00;
                }else{ config.mic1_volume -= 5;}
                
                SetWiperValue(MCP4561_ADDR1, 0, config.mic1_volume);
            }
            if (x == 2) {
                //config.mic2_volume -= 5;
                if (config.mic2_volume < 5) {
                    config.mic2_volume = 0x00;
                }else{config.mic2_volume -= 5;}
                
                SetWiperValue(MCP4561_ADDR2, 0, config.mic2_volume);
            }
            if (x == 3) {
                if (config.mic3_volume < 5) {
                    config.mic3_volume = 0x00;
                }else{config.mic3_volume -= 5;}
                
                //SetWiperValue(MCP4561_ADDR1, 0, config.mic3_volume);            // To change MIC3 ON MIC1 "MCP4561_ADDR1"
                SetWiperValue(MCP4561_ADDR2, 0, config.mic3_volume);            // To change MIC3 ON MIC1 "MCP4561_ADDR1"
            }
            if (x == 4) {
                
                if (config.msg_volume >= 31) {
                    config.msg_volume = 30;
                }else{config.msg_volume++;}
                jq8900_setVolume(config.msg_volume);  // Adjust message volume
            }
        }
        
            // Convert each volume to dB
    float mic1_db = ConvertVolumeToDB(config.mic1_volume);
    float mic2_db = ConvertVolumeToDB(config.mic2_volume);
    float mic3_db = ConvertVolumeToDB(config.mic3_volume);

    // Display the dB values on the LCD
    lcd_gotoxy(15, 3);
    if(config.mic1_mode == 0){
    sprintf(scr_buff, " A1 : %4.1f dB", mic1_db);
    }else{
        sprintf(scr_buff, " M1 : %4.1f dB", mic1_db);
    }
    lcd_putstr(scr_buff, neg1);

    lcd_gotoxy(15, 4);
    sprintf(scr_buff, " M2 : %4.1f dB", mic2_db);
    lcd_putstr(scr_buff, neg2);

    lcd_gotoxy(15, 5);
    sprintf(scr_buff, " M3 : %4.1f dB", mic3_db);
    lcd_putstr(scr_buff, neg3);

//        // Display the current volume levels on the LCD
//lcd_gotoxy(15, 3); sprintf(scr_buff, " M1 : %3d ", config.mic1_volume); lcd_putstr(scr_buff, neg1);
//lcd_gotoxy(15, 4); sprintf(scr_buff, " M2 : %3d ", config.mic2_volume); lcd_putstr(scr_buff, neg2);
//lcd_gotoxy(15, 5); sprintf(scr_buff, " M3 : %3d ", config.mic3_volume); lcd_putstr(scr_buff, neg3);
 lcd_gotoxy(15, 7); sprintf(scr_buff, " MSG: %3d ", config.msg_volume); lcd_putstr(scr_buff, neg4);
        

        
        // Handle playback if MSG is selected
        if (x == 0 ) {
            jq8900_stop();           
            shiftRegisterData &= ~((unsigned int) (1 << 0));                    // Audio Source - MIC1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 1));                    // Audio Source - MIC1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 2));                    // Audio Source - MIC2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 3));                    // Audio Source - MIC2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 4));                    // Audio Source - MSG1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 5));                    // Audio Source - MSG1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 6));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 7));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 8));                    // - MUTE - OM
            shiftRegisterData &= ~((unsigned int) (1 << 9));                    // - LED_EM - OFF 
            shiftRegisterData |= ((unsigned int) (1 << 10));                    // - CHARG - ON
            shiftRegisterData &= ~((unsigned int) (1 << 11));                   // - DRY_CONT - OFF
            
            SendShiftData595(shiftRegisterData);
            zone_on_off(0, 0);
        }
        
   else if (x == 1 && press_1 == 0) {
            shiftRegisterData |= ((unsigned int) (1 << 0));                     // Audio Source1 - MIC1 - ON
            shiftRegisterData |= ((unsigned int) (1 << 1));                     // Audio Source1 - MIC1 - ON
            shiftRegisterData &= ~((unsigned int) (1 << 2));                    // Audio Source - MIC2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 3));                    // Audio Source - MIC2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 4));                    // Audio Source1 - MSG1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 5));                    // Audio Source1 - MSG2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 6));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 7));                    // Audio Source - 100V - OFF
            shiftRegisterData |= ((unsigned int) (1 << 8));                     // - MUTE - OFF
            shiftRegisterData |= ((unsigned int) (1 << 10));                    // - CHARG - ON
            shiftRegisterData &= ~((unsigned int) (1 << 11));                   // - DRY_CONT - OFF
            SendShiftData595(shiftRegisterData);                                //(0b0001000100000011);
            __delay_ms(1000);
            zone_on_off(1, 1);
            press_1 = 1;
        }
   else if ( x == 2  && press_1 == 0) {
            shiftRegisterData &= ~((unsigned int) (1 << 0));                    // Audio Source1 - MIC1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 1));                    // Audio Source1 - MIC1 - OFF            
            shiftRegisterData |= ((unsigned int) (1 << 2));                     // Audio Source - MIC2 - ON
            shiftRegisterData |= ((unsigned int) (1 << 3));                     // Audio Source - MIC2 - ON
            shiftRegisterData &= ~((unsigned int) (1 << 4));                    // Audio Source1 - MSG1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 5));                    // Audio Source1 - MSG2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 6));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 7));                    // Audio Source - 100V - OFF
            shiftRegisterData |= ((unsigned int) (1 << 8));                     // - MUTE - OFF
            shiftRegisterData |= ((unsigned int) (1 << 10));                    // - CHARG - ON
            shiftRegisterData &= ~((unsigned int) (1 << 11));                   // - DRY_CONT - OFF
            SendShiftData595(shiftRegisterData);                                // SendShiftData595(0b0001010100001100);                                                                               
            __delay_ms(1000);
            zone_on_off(1, 1);
            press_1 = 1;

        }
   else if ( x == 3  && press_1 == 0) {
//=============================================MIC3 ON MIC1===========================================================================//
//       
//            shiftRegisterData |= ((unsigned int) (1 << 0));                    // Audio Source1 - MIC3/MIC1 - ON
//            shiftRegisterData |= ((unsigned int) (1 << 1));                    // Audio Source1 - MIC3/MIC1 - ON            
//            shiftRegisterData &= ~((unsigned int) (1 << 2));                     // Audio Source - MIC2 - OFF
//            shiftRegisterData &= ~((unsigned int) (1 << 3));                     // Audio Source - MIC2 - OFF
//            shiftRegisterData &= ~((unsigned int) (1 << 4));                    // Audio Source1 - MSG1 - OFF
//            shiftRegisterData &= ~((unsigned int) (1 << 5));                    // Audio Source1 - MSG2 - OFF
//            shiftRegisterData &= ~((unsigned int) (1 << 6));                    // Audio Source - 100V - OFF
//            shiftRegisterData &= ~((unsigned int) (1 << 7));                    // Audio Source - 100V - OFF
//            shiftRegisterData |= ((unsigned int) (1 << 8));                     // - MUTE - OFF
//            shiftRegisterData |= ((unsigned int) (1 << 10));                    // - CHARG - ON
//            shiftRegisterData &= ~((unsigned int) (1 << 11));                   // - DRY_CONT - OFF
//            SendShiftData595(shiftRegisterData);                                // SendShiftData595(0b0001010100000011);
       
//=============================================MIC3 ON MIC2===========================================================================//
            shiftRegisterData &= ~((unsigned int) (1 << 0));                    // Audio Source1 - MIC1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 1));                    // Audio Source1 - MIC1 - OFF            
            shiftRegisterData |= ((unsigned int) (1 << 2));                     // Audio Source - MIC3/MIC2 - ON
            shiftRegisterData |= ((unsigned int) (1 << 3));                     // Audio Source - MIC3/MIC2 - ON
            shiftRegisterData &= ~((unsigned int) (1 << 4));                    // Audio Source1 - MIC3/MSG2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 5));                    // Audio Source1 - MIC3/MSG2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 6));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 7));                    // Audio Source - 100V - OFF
            shiftRegisterData |= ((unsigned int) (1 << 8));                     // - MUTE - OFF
            shiftRegisterData |= ((unsigned int) (1 << 10));                    // - CHARG - ON
            shiftRegisterData &= ~((unsigned int) (1 << 11));                   // - DRY_CONT - OFF
            SendShiftData595(shiftRegisterData);                                // SendShiftData595(0b0001010100001100);
            
            __delay_ms(1000);
            zone_on_off(1, 1);
            press_1 = 1;
        }

   else if (x == 4 && is_module_busy() == 0) {
            play_ = 1;
            shiftRegisterData &= ~((unsigned int) (1 << 0));                    // Audio Source1 - MIC1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 1));                    // Audio Source1 - MIC1 - OFF            
            shiftRegisterData &= ~((unsigned int) (1 << 2));                    // Audio Source - MIC2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 3));                    // Audio Source - MIC2 - OFF
            shiftRegisterData |= ((unsigned int) (1 << 4));                     // Audio Source1 - MSG1 - ON
            shiftRegisterData |= ((unsigned int) (1 << 5));                     // Audio Source1 - MSG2 - ON
            shiftRegisterData &= ~((unsigned int) (1 << 6));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 7));                    // Audio Source - 100V - OFF
            shiftRegisterData |= ((unsigned int) (1 << 8));                     // - MUTE - OFF
            shiftRegisterData |= ((unsigned int) (1 << 10));                    // - CHARG - ON
            shiftRegisterData &= ~((unsigned int) (1 << 11));                   // - DRY_CONT - OFF
            SendShiftData595(shiftRegisterData);                                //  SendShiftData595(0b0001010100110000); 
           
            __delay_ms(1000);
            zone_on_off(1, 1);
            jq8900_playTrack(5);

        }

        // Navigation buttons to move between options
        if (_left1.click == 1) {
            _left1.click = 0;
            press_1 = 0;
            zone_on_off(0, 0);
            x++;
            if (x >= 5) { x = 0; }
        }

        // Exit and save settings
        if (_left2.click == 1 || aud == 1) {
            _left2.click = 0;
            aud = 1;
            play_ = 0;
            adc_en = 1;
            shiftRegisterData &= ~((unsigned int) (1 << 0));                    // Audio Source - MIC1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 1));                    // Audio Source - MIC1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 2));                    // Audio Source - MIC2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 3));                    // Audio Source - MIC2 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 4));                    // Audio Source - MSG1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 5));                    // Audio Source - MSG1 - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 6));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 7));                    // Audio Source - 100V - OFF
            shiftRegisterData &= ~((unsigned int) (1 << 8));                    // - MUTE - OM
            shiftRegisterData &= ~((unsigned int) (1 << 9));                    // - LED_EM - OFF 
            shiftRegisterData |= ((unsigned int) (1 << 10));                    // - CHARG - ON
            shiftRegisterData &= ~((unsigned int) (1 << 11));                   // - DRY_CONT - OFF
            SendShiftData595(shiftRegisterData);                                //SendShiftData595(0b0001010000000000);
            
            jq8900_stop();
            zone_on_off(0, 0);
            if (_mgs_vol_backup != config.msg_volume) { 
                EEPROM_Write(EEPROM_ADDR_MSG_VOLUME, config.msg_volume);
            }
            if (_mic1_vol_backup != config.mic1_volume) { 
                EEPROM_Write(EEPROM_ADDR_MIC1_VOLUME, config.mic1_volume);
            }
            if (_mic2_vol_backup != config.mic2_volume) { 
                EEPROM_Write(EEPROM_ADDR_MIC2_VOLUME, config.mic2_volume);
            }
            if (_mic3_vol_backup != config.mic3_volume) { 
                EEPROM_Write(EEPROM_ADDR_MIC3_VOLUME, config.mic3_volume);
            }
            lcd_clear();
            first_screen();
        }
    }
}
 
void sys_timer_set(void) {
    unsigned char backup_line_timer = config.line_timer;
    unsigned char backup_mode       = config.mode; // 1 = 1 Zone, 2 = 2 Zones
    unsigned char menu_index = 0; // 0 = Line Timer, 1 = Zone Mode
    m_t = 0;

    lcd_clear();
    lcd_gotoxy(20,0);
    lcd_putstr("System Config", 1);

    img_view(115,1,9,_up,0);  
    img_view(115,6,9,_down,0);
    img_view(0,6,9,_back,0); 

    while (m_t != 1) {
        char buf[30];

        // === Draw Line Timer ===
        lcd_gotoxy(0,2);
        switch (config.line_timer) {
            case 0: sprintf(buf, "[Line Timer: 10 Sec]"); break;
            case 1: sprintf(buf, "[Line Timer: 1 Min]"); break;
            case 2: sprintf(buf, "[Line Timer: 5 Min]"); break;
            case 3: sprintf(buf, "[Line Timer: 10 Min]"); break;
            case 4: sprintf(buf, "[Line Timer: 15 Min]"); break;
        }
        if(menu_index != 0) {
            for(int i=0;i<strlen(buf);i++){
                if(buf[i]=='[' || buf[i]==']') buf[i]=' ';
            }
        }
        lcd_putstr(buf, 0);

        // === Draw Zone Mode ===
        lcd_gotoxy(0,4);
        sprintf(buf, "[Zone Mode: %d]", config.mode);
        if(menu_index != 1) {
            for(int i=0;i<strlen(buf);i++){
                if(buf[i]=='[' || buf[i]==']') buf[i]=' ';
            }
        }
        lcd_putstr(buf, 0);

        // === Buttons ===
        if (_right1.click) { // UP
            _right1.click = 0;
            if (menu_index == 0) {
                if (++config.line_timer > 4) config.line_timer = 0;
            } else if (menu_index == 1) {
                config.mode = (config.mode == 1) ? 2 : 1;
            }
        }

        if (_right2.click) { // DOWN
            _right2.click = 0;
            if (menu_index == 0) {
                if (config.line_timer == 0) config.line_timer = 4;
                else config.line_timer--;
            } else if (menu_index == 1) {
                config.mode = (config.mode == 1) ? 2 : 1;
            }
        }

        if (_left1.click) { // ENTER ? ??????????? ????
            _left1.click = 0;
            menu_index = (menu_index == 0) ? 1 : 0;
            
            if (backup_mode != config.mode) {
                EEPROM_Write(EEPROM_ADDR_MODE, config.mode);

                // ?????????? ??????? ?????? ??? ????????? ??????!
                asm("RESET");
                __delay_ms(500);
            }
        }

        if (_left2.click) { // BACK ? ????????? ? ?????
            _left2.click = 0;

            // --- ???????? ????????? ---
            if (backup_line_timer != config.line_timer) { 
                EEPROM_Write(EEPROM_ADDR_line_timer, config.line_timer);
                line_check_presentes(config.line_timer);
            }

            // ???? ?????? line_timer ??????? ? ?????? ???????
            m_t = 1;
        }

        __delay_ms(150);
    }
}


void m_screen(unsigned char i){
    
    if(i == 1){
        Display_Picture(_main_7_HE);
        i = 0;
    }
    
}

void first_screen(void){
    
    lcd_gotoxy(0,1);lcd_putstr("OK",0);
    //img_view(0,1,8,_enter,0);
    img_view(0,6,9,_back,0); //
    img_view(115,1,9,_up,0);  
    img_view(115,6,9,_down,0); 
    img_view(35, 0, 50,sys_sett_HE, 0);      
    
}

void tmr_drop_menu(unsigned char en){
          
if(en == 1){
        lcd_clear();
        m_screen(1);
        exit_menu       = 0;
        exit_en         = 0;
        one_step        = 0;
        backlight_tmr   = 0;
        clr1 = clr2 = clr3 = 0;
        m1 = m2 = m3 = m4 = 0;
    }
}

void adc_monitor(void) {
    static const char * const channelNames[] = {"SP1", "SP2", "MIC1", "MIC2", "MIC3"};
    static const unsigned char channels[] = {0, 1, 17, 14, 3};
    static const unsigned char num_channels = sizeof(channels) / sizeof(channels[0]);

    unsigned char menu_index = 0; // ??????? ????????? ?????
    debug_screen = 0;
    lcd_clear();
    BACK_LIGHT = 1;

    shiftRegisterData &= ~((unsigned int)(1 << 10));
    SendShiftData595(shiftRegisterData);

    // ???????? ??????????? ????? 1 ???
    for (unsigned char i = 0; i < num_channels; i++) {
        lcd_gotoxy(0, i);
        lcd_putstr("       ", 0); // ??????? ??????
    }

    while (!debug_screen) {
        
        for (unsigned char i = 0; i < num_channels; i++) {
            int adc_value = readADC(channels[i]);
            float voltage = (adc_value * 3.3f) / 1023.0f;
            sprintf(scr_buff, "%c%s:%.2fV", (i==menu_index)?'>':' ', channelNames[i], voltage);
            lcd_gotoxy(0, i);
            lcd_putstr(scr_buff, 0);
        }

        // ?????????????? ??????????
        lcd_gotoxy(75, 0); sprintf(scr_buff, "AC:  %d", ac_); lcd_putstr(scr_buff,0);
        lcd_gotoxy(75, 1); sprintf(scr_buff, "BATT:%d", batt_); lcd_putstr(scr_buff,0);
        lcd_gotoxy(75, 3); sprintf(scr_buff, "F_AMP:%d", fault_amp); lcd_putstr(scr_buff,0);
        lcd_gotoxy(0, 5); sprintf(scr_buff, "IN1:%d", PTT_3()); lcd_putstr(scr_buff,0);
        lcd_gotoxy(0, 6); sprintf(scr_buff, "IN2:%d", PTT_4()); lcd_putstr(scr_buff,0);

        // --- ????????? ?????? ---
        if (_right1.click) { _right1.click=0; if(menu_index>0) menu_index--; } 
        if (_right2.click) { _right2.click=0; if(menu_index<num_channels-1) menu_index++; } 

        if (_left1.click) { // Enter
             _left1.click=0;
                lcd_clear();
                adc_menu_SP1();
        }
        if (_left2.click) { // Back
            _left2.click=0;
            debug_screen = 1;
            m_a = 1;
            lcd_clear();
            first_screen();
        }

        __delay_ms(200);
    }

    shiftRegisterData |= (1 << 10);
    SendShiftData595(shiftRegisterData);
}

void adc_menu_MIC1(void) {
    unsigned char paramIndex = 0; // 0=OK,1=PR,2=CT,3=SH
    unsigned char editing = 0;    // min/max
    unsigned char editMax = 0;    //  max
    //int adcValue;
   // char buf[20];
    unsigned char micChannels[3] = {17, 14, 3}; // MIC1, MIC2, MIC3
    int adcValues[3];
    char buf[10];
    BACK_LIGHT = 1;
    MicThresholds *mt = &mic1Thresholds;


    lcd_gotoxy(10,3); lcd_putstr("OK:",0);
    lcd_gotoxy(10,4); lcd_putstr("PR:",0);
    lcd_gotoxy(10,5); lcd_putstr("CT:",0);
    lcd_gotoxy(10,6); lcd_putstr("SH:",0);

    while (!_left2.click) { // Back
 
        for(uint8_t i=0;i<3;i++){
            adcValues[i] = readADC(micChannels[i]);
        }

    // "M1:123  M2:456  M3:789"
        sprintf(buf,"M1:%3d M2:%3d M3:%3d", adcValues[0], adcValues[1], adcValues[2]);
        lcd_gotoxy(0,0); // ????? ?? ?????? ??????
        lcd_putstr(buf,0);

        // ?????????? ??????
        unsigned int minVal,maxVal;
        for(unsigned char i=0;i<4;i++){
            switch(i){
                case 0: minVal=mt->ok_min; maxVal=mt->ok_max; break;
                case 1: minVal=mt->pr_min; maxVal=mt->pr_max; break;
                case 2: minVal=mt->ct_min; maxVal=mt->ct_max; break;
                case 3: minVal=mt->sh_min; maxVal=mt->sh_max; break;
            }

            unsigned char y = 3+i;  
            unsigned char xVal = 40;  // min/max


            if(i==paramIndex && editing){
                if(editMax)
                    sprintf(buf,"[%3d]   ", maxVal);
                else
                    sprintf(buf,"[%3d]   ", minVal);
            } else {
                sprintf(buf," %3d - %3d ", minVal, maxVal);
            }
            lcd_gotoxy(xVal,y);
            lcd_putstr(buf,0);

            // ??????? ?????? ????????? (?????? ??? ?????????)
            lcd_gotoxy(30,y);
            lcd_putch(i==paramIndex && !editing?'>':' ',0);
        }

        // --- ?????? ---
        if(!editing){
            if(_right1.click){ _right1.click=0; if(paramIndex>0) paramIndex--; }
            if(_right2.click){ _right2.click=0; if(paramIndex<3) paramIndex++; }
            if(_left1.click){ _left1.click=0; editing=1; editMax=0; } // Enter ????????????? min
        } else {
            // ??????????? min/max ????????
            if(_right1.click){ _right1.click=0; if(!editMax) increaseParamMin(mt,paramIndex); else increaseParamMax(mt,paramIndex); }
            if(_right2.click){ _right2.click=0; if(!editMax) decreaseParamMin(mt,paramIndex); else decreaseParamMax(mt,paramIndex); }
            if(_left1.click){ _left1.click=0;
                if(!editMax) editMax=1; // ??????? ? max
                else editing=0;          // ????????? ??????????????
            }
        }

        __delay_ms(100);
    }
    _left2.click=0;
    saveMic1ThresholdsToEEPROM();
    lcd_clear();
    first_screen(); // Return to the first screen
}

void adc_menu_SP1(void){
 
    
    
}

void increaseParamMin(MicThresholds *mt, unsigned char param) {
    switch(param) {
        case 0: if(mt->ok_min<mt->ok_max) mt->ok_min++; break;
        case 1: if(mt->pr_min<mt->pr_max) mt->pr_min++; break;
        case 2: if(mt->ct_min<mt->ct_max) mt->ct_min++; break;
        case 3: if(mt->sh_min<mt->sh_max) mt->sh_min++; break;
    }
}

void decreaseParamMin(MicThresholds *mt, unsigned char param) {
    switch(param) {
        case 0: if(mt->ok_min>0) mt->ok_min--; break;
        case 1: if(mt->pr_min>0) mt->pr_min--; break;
        case 2: if(mt->ct_min>0) mt->ct_min--; break;
        case 3: if(mt->sh_min>0) mt->sh_min--; break;
    }
}

void increaseParamMax(MicThresholds *mt, unsigned char param) {
    switch(param) {
        case 0: mt->ok_max++; break;
        case 1: mt->pr_max++; break;
        case 2: mt->ct_max++; break;
        case 3: mt->sh_max++; break;
    }
}

void decreaseParamMax(MicThresholds *mt, unsigned char param) {
    switch(param) {
        case 0: if(mt->ok_max>mt->ok_min) mt->ok_max--; break;
        case 1: if(mt->pr_max>mt->pr_min) mt->pr_max--; break;
        case 2: if(mt->ct_max>mt->ct_min) mt->ct_max--; break;
        case 3: if(mt->sh_max>mt->sh_min) mt->sh_max--; break;
    }
}



//void sys_timer_set(void) {
//    
//    unsigned char backup_lime_timer = 0; 
//    m_t = 0;
//    
//    lcd_clear();
//    backup_lime_timer = config.line_timer;
//    
//    lcd_gotoxy(20,0);
//    lcd_putstr("Line Timer ", 1);
//
//    lcd_gotoxy(15,2);
//    lcd_putstr("L1/L2-", 0);
//    
//    img_view(115,1,9,_up,0);  
//    img_view(115,6,9,_down,0);
//    img_view(0,6,9,_back,0); 
//    
//    while (m_t != 1) {
//        if (_right1.click == 1) {
//            _right1.click = 0;
//            config.line_timer++;
//            if (config.line_timer == 5) {
//                config.line_timer = 0;
//            }
//        }
//
//        if (_right2.click == 1) {
//            _right2.click = 0;
//            if (config.line_timer == 0) {
//                config.line_timer = 4;
//            } else {
//                config.line_timer--;
//            }
//
//        }
//        
//                lcd_gotoxy(55, 2); // Adjust position as needed
//        switch (config.line_timer) {
//            case 0: lcd_putstr(" 10 Sec ", 1);break; // 10 Secbreak;
//            case 1: lcd_putstr(" 1 Min  ", 1);break;
//            case 2: lcd_putstr(" 5 Min  ", 1);break;
//            case 3: lcd_putstr(" 10 Min ", 1);break;
//            case 4: lcd_putstr(" 15 Min ", 1);break;
//            default:
//                break;
//        }
//
//        if (_left2.click == 1) {
//            _left2.click = 0;
//            if (backup_lime_timer != config.line_timer) { 
//                EEPROM_Write(EEPROM_ADDR_line_timer, config.line_timer);
//                line_check_presentes(config.line_timer);
//            }
//            m_t = 1;
//            m_a = 1;
//            lcd_clear();
//            first_screen();
//        }
//    } 
//}



//void adc_monitor(void) {
//    static const char * const channelNames[] = {"SP1", "SP2", "MIC1", "MIC2", "MIC3"};
//    static const unsigned char channels[] = {0, 1, 17, 14, 3};
//    static const unsigned char num_channels = sizeof(channels) / sizeof(channels[0]);
//
//    debug_screen = 0;
//    lcd_clear();
//    //_backlight_tmr_en_(0, 1);
//    BACK_LIGHT = 1;
//    
//    shiftRegisterData &= ~ ((unsigned int) (1 << 10));//SendShiftData595(shiftRegisterData);
//    SendShiftData595(shiftRegisterData);
//
//    while (!debug_screen) {
//        for (unsigned char i = 0; i < num_channels; i++) {
//            int adc_value = readADC(channels[i]);
//            float voltage = (adc_value * 3.3f) / 1023.0f; // Use float for better efficiency in XC8
//            sprintf(scr_buff, "%s:%.2fV", channelNames[i], voltage);
//            lcd_gotoxy(0, i);
//            lcd_putstr(scr_buff, 0);
//        }
//
//        // Additional display information
//        lcd_gotoxy(75, 0);
//        sprintf(scr_buff, "AC:  %d", ac_);
//        lcd_putstr(scr_buff, 0);
//
//        lcd_gotoxy(75, 1);
//        sprintf(scr_buff, "BATT:%d", batt_);
//        lcd_putstr(scr_buff, 0);
//
//        lcd_gotoxy(75, 3);
//        sprintf(scr_buff, "F_AMP:%d",fault_amp);
//        lcd_putstr(scr_buff, 0);
//
//        lcd_gotoxy(0, 5);
//        sprintf(scr_buff, "IN1:%d",PTT_3());
//        lcd_putstr(scr_buff, 0);
//
//        lcd_gotoxy(0, 6);
//        sprintf(scr_buff, "IN2:%d", PTT_4());
//        lcd_putstr(scr_buff, 0);
//
//        __delay_ms(500);
//
//        if (_left2.click) {
//            _left2.click = 0;
//            debug_screen = 1;
//            m_a = 1;
//            lcd_clear();
//            first_screen();
//        }
//    }
//
//    shiftRegisterData |= (1 << 10);  // Restore shift register state
//    SendShiftData595(shiftRegisterData);
    
   // adc_menu_MIC1();
//}




//void menu2(void) {                                                              // Error list menu
    
//    if(all_errors1 !=0 || sp_errors !=0){
//
//    if (m2 == 1) {
//       lcd_clear();
//       img_view(0,6,9,_back,0); //
//       img_view(30,0,50, error_list,0);
//       lcd_gotoxy(110,2);lcd_putstr(".1",0);
//       lcd_gotoxy(110,3);lcd_putstr(".2",0);
//       lcd_gotoxy(110,4);lcd_putstr(".3",0);
//       lcd_gotoxy(110,5);lcd_putstr(".4",0);
//       lcd_gotoxy(110,6);lcd_putstr(".5",0);
//      // Initialize mOrder to reset all statuses
//
//            while (m2 == 1) {
//
//                
//                
//                if (_left2.click == 1) {
//                    _left2.click = 0;
//                    m2 = 0;
//                    _backlight_tmr_en_(1,0);
//                    lcd_clear();
//                    m_screen(1);
//               }
//           }
//       }
//   }
//}
