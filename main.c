
//REV1//


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
#include <stdbool.h>


void sys_init(void){

    OSCCON = 0b01110000;                                                       //oscialtor 16MHZ//OSCCONbits.IRCF
    
    CM1CON0 = 0x00;  // Disable Comparator 1
    CM2CON0 = 0x00;  // Disable Comparator 2
    
    TRISA = 0b10111111;
    TRISB = 0b00000101;
    TRISC = 0b10111101;
    TRISD = 0b10000000;
    TRISE = 0b00000111;  
    
    ANSELAbits.ANSA0 = 1;
    ANSELAbits.ANSA1 = 1;
    ANSELAbits.ANSA2 = 0;
    ANSELAbits.ANSA3 = 1;
    ANSELAbits.ANSA5 = 0;
    
    ANSELCbits.ANSC2 = 1;
    ANSELCbits.ANSC3 = 0;
    ANSELCbits.ANSC4 = 0;
    ANSELCbits.ANSC5 = 1;
    ANSELCbits.ANSC6 = 0;
    ANSELCbits.ANSC7 = 0;
    
    ANSELB = 0x00;
    ANSELD = 0b00000000;
    ANSELE = 0x00;
    
    WPUB = 0b00000100;
    
    PORTA = 0b00000000;
    PORTB = 0x00;
    PORTC = 0x00;
    PORTD = 0x00;
    
    PWR_LED = 1;
    BACK_LIGHT = 1;
    
    LCDInit(); 
    __delay_ms(10);
    ADC_init();
    __delay_ms(10);
    I2CInit();                                                                  // Digi Poten
    __delay_ms(10);
    uart1_init(115200);                                                         // RS485 UART
    __delay_ms(10);
   uart2_init(9600);                                                           // MSG UART
     __delay_ms(10);
    init_TMR0(1);
     __delay_ms(10);
    init_TMR5(0,1,1);   
      __delay_ms(10);  
    //CCP2_Init();
    //PWM_Set_Duty(1023);
    
    SetWiperValue(MCP4561_ADDR1, 0, 0xff);
    SetWiperValue(MCP4561_ADDR2, 0, 0xff);

    LoadConfigData();
    _first_run_(first_menu,0);                                                                                //MSG INIT
    BACK_LIGHT = 1;
    startup_logo_leds();
    Display_Picture(_main_7_HE); 
  //==========================================================================// 
    adc_en = 1;
  //  PWR_LED=1;
  //  sys_adc_tst(1);                                                             // first PWR test
  //  handleLineStates(1);                                                        // first Line test
    INTCONbits.PEIE = 1;     
    INTCONbits.GIE = 1; 
}

void startup_logo_leds(void){
   
    Display_Picture(_logo_);
    __delay_ms(500);
    __delay_ms(500);
    __delay_ms(500);
     buzzer = 1;
     __delay_ms(200);
    buzzer = 0;
    lcd_clear();
}

void _first_run_(unsigned char en,unsigned char en2){

if(en==1){
   // if()
    Display_Picture(mode001_HE);
   //contr1 = 1;
//    while(contr1==1){
//     
//                if(!PORTEbits.RE2){
//                    //_right1.click = 0; 
//                    BACK_LIGHT = 1;
//                    config.mode = 2;
//                    }
//
//                 if(!PORTCbits.RC0){
//                    //_right2.click = 0;
//                    BACK_LIGHT = 1;
//                    config.mode = 1;
//
//                    }
//         if(config.mode == 1){
//
//             display_digit(59,6,1,0);
//         }else{
//             display_digit(59,6,2,0);
//            }
//
//
//        if(!PORTEbits.RE1){
//                //_left2.click = 0;
//                BACK_LIGHT = 1;
//                contr1 = 0;
//                lcd_clear();
//                first_menu = 0;
//                EEPROM_Write(EEPROM_ADDR_MODE, config.mode);
//                img_view(0,6,9,_back,0); // 
//            }
//
//        }
    }
       
       if(counter[0].Zones == 0xff){if(config.mode == 1){counter[0].Zones = 0;}else{counter[0].Zones = 2;}}
       if(counter[0].MSG == 0xff){if(config.mode == 1){counter[0].MSG = 4;}else{counter[0].MSG = 1;}} 
   
       if(counter[1].Zones == 0xff){if(config.mode == 1){counter[1].Zones = 0;}else{counter[1].Zones = 2;}}     
       if(counter[1].MSG == 0xff){if(config.mode == 1){counter[1].MSG = 4;}else{counter[1].MSG = 2;}} 
             
       if(config.zone_sel1 == 0xff){if(config.mode == 1){config.zone_sel1 = 3;}else{config.zone_sel1 = 3;}}
       if(config.zone_sel2 == 0xff){if(config.mode == 2){config.zone_sel2 = 3;}else{config.zone_sel2 = 3;}}

       if(config.zone_sel3 == 0xff){if(config.mode == 1){config.zone_sel3 = 2;}else{config.zone_sel3 = 2;}}
       if(config.mic1_mode == 0xff){if(config.mode == 1){config.mic1_mode = 1;}else{config.mic1_mode = 1;}}

}

void LoadConfigData(void) {

    config.mem_blank    = EEPROM_Read(memory_blank);
    config.mode         = EEPROM_Read(EEPROM_ADDR_MODE);

    config.zone_sel1    = EEPROM_Read(EEPROM_ADDR_MIC1);
    config.zone_sel2    = EEPROM_Read(EEPROM_ADDR_MIC2);
    config.zone_sel3    = EEPROM_Read(EEPROM_ADDR_MIC3);

    config.mic1_volume  = EEPROM_Read(EEPROM_ADDR_MIC1_VOLUME);
    config.mic2_volume  = EEPROM_Read(EEPROM_ADDR_MIC2_VOLUME);
    config.mic3_volume  = EEPROM_Read(EEPROM_ADDR_MIC3_VOLUME);
    config.mic1_mode    = EEPROM_Read(EEPROM_ADDR_AUX);

    config.msg_volume   = EEPROM_Read(EEPROM_ADDR_MSG_VOLUME);

    counter[0].Zones    = EEPROM_Read(EEPROM_ADDR_dry1);
    counter[1].Zones    = EEPROM_Read(EEPROM_ADDR_dry2);

    counter[0].MSG      = EEPROM_Read(EEPROM_ADDR_msg1);
    counter[1].MSG      = EEPROM_Read(EEPROM_ADDR_msg2);

    //config.lang         = EEPROM_Read(EEPROM_ADDR_lang);
    config._485         = EEPROM_Read(EEPROM_ADDR_485);
    config.slave_count  = EEPROM_Read(EEPROM_ADDR_SLAVE_COUNT);

    config.line_timer   = EEPROM_Read(EEPROM_ADDR_line_timer);
    loadMic1Thresholds();

    if (config.mem_blank == 0xff) {
    }


    if (config.mode == 0xff) {
        first_menu = 1;
        config.mode = 2;
    } else {
        first_menu = 0;
    }
    
    if (config.mic1_mode == 255 || config.mic1_mode == 1){
        
        config.mic1_mode = 1;
        shiftRegisterData |= (1 << 12); // MIC1/AUX
    }
    
    
    if (config.mic1_mode == 0) {
        shiftRegisterData &= ~(1u << 12); // AUX/MIC1
    } 
    
    
    if (config.mic1_volume == 0xff) {
        config.mic1_volume = 255;
    }
    
    if (config.mic2_volume == 0xff) {
        config.mic2_volume = 255;
    }
    
    if (config.mic3_volume == 0xff) {
        config.mic3_volume = 200;
    }
    
    if (config.msg_volume == 0xff) {
        config.msg_volume = 20;
    }

    if (config._485 == 0xff) {
        config._485 = 0;
    }
    
    if (config.slave_count == 0xff) {
        config.slave_count = 1;
    }
    
   if (config.line_timer == 0xff) {
        config.line_timer = 1;
    }
    
    SendShiftData595(shiftRegisterData);
    
    SetWiperValue(MCP4561_ADDR1, 0, config.mic1_volume);
    SetWiperValue(MCP4561_ADDR2, 0, config.mic2_volume);
    
    jq8900_init(); 
    
    line_check_presentes(config.line_timer);
}

void line_check_presentes(unsigned char presente){
    
        switch (presente) {

        case 0:
            tmr_line_tst_set = 1000; // 10 Sec
            break;
        case 1:
            tmr_line_tst_set = 6000; // 1 min
            break;
        case 2:
            tmr_line_tst_set = 30000; // 5 min
            break;
        case 3:
            tmr_line_tst_set = 60000; // 10 min
            break;
        case 4:
            tmr_line_tst_set = 90000; // 15 min
            break;
        default:
            // 
            break;
    }
    
}

void __interrupt() isr(void) {
    //---------- TIMER0 Interrupt (every ~10ms) ----------//
    if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0; // Clear Timer0 interrupt flag

       volatile unsigned int current_error_state = ((unsigned int)sp_errors << 8) | all_errors1;

        //==================== Line Test ====================//
        if (all_alarms == 0 && line_test_on != 1) {
            tmr_line_tst++;
            if (tmr_line_tst >= line_fast_check) {
                adc_en = 1;
                tmr_line_tst = 0;
            }
        } else {
            tmr_line_tst = 0;
        }
        line_fast_check = (sp_errors != 0) ? 100 : tmr_line_tst_set;

        //==================== Power Test ====================//
        if (!pwr_tst) pwr_tmr++;
        if (pwr_tmr >= ((all_errors1 != 0) ? 100 : 3000)) {
            pwr_tmr = 0;
            pwr_tst = 1;
        }

        //==================== Backlight Auto-Off ====================//
        if (BACK_LIGHT && all_errors1 == 0 && all_alarms == 0 &&
            m1 == 0 && m2 == 0 && m3 == 0 && m4 == 0) {
            backlight_tmr++;
            if (backlight_tmr >= 6000) {
                backlight_tmr = 0;
                BACK_LIGHT = 0;
            }
        }

        //==================== Emergency Call Timer ====================//
        if (++em_call_tmr >= 100) {
            em_call_tmr = 0;
            em_call = 1;
        }

        //==================== Error LED & Buzzer Logic ====================//
        if (current_error_state != last_error_state && current_error_state != 0) {
            // New error appeared ? cancel silence
            buzzer_silenced = 0;
            buzzer_silence_tmr = 0;
        }
        last_error_state = current_error_state;

        if (buzzer_silenced) {
            buzzer_silence_tmr++;
            if (buzzer_silence_tmr >= 90000) { // 15 minutes
                buzzer_silenced = 0;
                buzzer_silence_tmr = 0;
            }
        }

        if (current_error_state != 0) {
            mlfnction = 1;
            _one_step = 0;

            if (++tmr0_blink_led >= 100) {
                tmr0_blink_led = 0;
                mlfn_neg = !mlfn_neg;
                ERR_LED = mlfn_neg;
                buzzer = (!buzzer_silenced && mlfn_neg) ? 1 : 0;
            }
        } else {
            ERR_LED = 0;
            buzzer = 0;
            mlfnction = 0;
            buzzer_silenced = 0;
            buzzer_silence_tmr = 0;
            last_error_state = 0;
        }

        //==================== Back to Main Screen Timer ====================//
        if (m1 || m2 || m3 || m4) {
            back_to_m_tmr++;
            if (back_to_m_tmr >= 6000) {
                back_to_m_tmr = 0;
                exit_en = 1;
                m1 = m2 = m3 = m4 = 0;
                rs_ = 1;
                m_a = 1;
                aud = 1;
            }
        } else {
            back_to_m_tmr = 0;
        }

        //==================== RS Timer ====================//
        if (rs_tmr_on && ++rc_time >= 100) {
            rc_time = 0;
            success = 0;
            rs_tmr_on = 0;
        }

        //==================== Reload Timer0 ====================//
        TMR0 = TMR0_RELOAD; // Pre-calculated for ~10ms
    } // End Timer0 ISR

    //---------- TIMER5 Interrupt (1ms) ----------//
    if (PIR5bits.TMR5IF) {
        PIR5bits.TMR5IF = 0;

        if (--cnt_but == 0) {
            cnt_but = 20;
            Butt_Tick(_left1); 
            Butt_Tick(_left2);     	 
            Butt_Tick(_right1); 
            Butt_Tick(_right2);
        }
        // Set Timer5 registers for next 1ms interrupt
        TMR5H = 0xFE; // High byte
        TMR5L = 0x18; // Low byte

        //PIR5bits.TMR5IF = 0; // Clear Timer5 interrupt flag
    }
    
    //---------- UART1 Receive Interrupt ----------//
    if (PIR1bits.RC1IF) {
        unsigned char received_byte = RCREG1;  // Read received byte
        if (rx_index < BUFFER_SIZE) {
            rx_buffer[rx_index++] = received_byte; // Store in buffer
        }
        if (rx_index >= BUFFER_SIZE) {
            rx_index = 0;      // Reset buffer index
            data_ready = 1;    // Set flag indicating data is ready
            success = 0;
            rs_tmr_on = 0;
        }
        PIR1bits.RC1IF = 0; // Clear UART receive interrupt flag
    }
}

void main(void){
   
    sys_init();
//    SetControlRegister(MCP4561_ADDR1, 0x00); // Example value
//    SetControlRegister(MCP4561_ADDR2, 0x00); // Example value
    CLRWDT();
    while (1){

        CLRWDT();
        tmr_drop_menu(exit_en);
        push_butt();
        handleLineStates(adc_en);
        sys_adc_tst(pwr_tst);
        handleEmergencyCalls(em_call);
        mulfunctions_(mlfn_neg);
        main_icon_(1);
        handleDryCont_3_4(dry_c);
        RS485_Master_Task(); 
       
    }
}

void push_butt(void){
    
if( _left1.click == 1){
       
       _left1.click = 0;
      BACK_LIGHT = 1;
       m1 = 1;
       menu1();
   }
if( _left2.click == 1){
       _left2.click = 0;
       if (all_errors1 != 0 || sp_errors != 0){
            buzzer_silenced = 1;
            buzzer_silence_tmr = 0;
            buzzer = 0;                                                         // Immediately silence
       }
        BACK_LIGHT = 1;
         m2 = 0;
         //menu2();
   }
if( _right1.click == 1){
       
       _right1.click = 0;
      BACK_LIGHT = 1;
       m3 = 1;
       menu3();
   }

if( _right2.click == 1){
       
       _right2.click = 0;
      BACK_LIGHT = 1;
       press_but = 1;
       m4 = 1;
       menu4();
   }   
}

void zone_on_off(unsigned char rly1 , unsigned char rly2){

    if (rly1 == 1 && (config.mode == 1 || config.mode == 2)) {                  // Single Channel

        RLY_L = 1;
        l1_inv = 1;

    } else {
        RLY_L = 0;
        l1_inv = 0;
    }
    if (config.mode == 2 && rly2 == 1){                                         // Dual channal 

        RLY_R = 1;
        l2_inv = 1;
    } else {
        RLY_R = 0;
        l2_inv = 0;
    }   
}

void action(unsigned char z1,unsigned char z2,unsigned int shuft){

    SendShiftData595(shuft);
    __delay_ms(100);
    zone_on_off(z1, z2);      
} 

void main_icon_(unsigned char en){

    if (config.zone_sel1 == 3 || (all_errors1 >> 5) & 1) {
        lcd_gotoxy(0, 7);
        lcd_putstr("    ", 0);
//        if (mulf_screen1 == 0 && config.zone_sel1 != 3) {
//            img_view(40, 3, 48, _mic1_cut_89, 0);
//            //clr1 = 1;
//        }
        if (mic1_cut == 1) {
            img_view(40, 3, 48, _mic1_cut_89, 0);
            //clr1 = 1;
        }
        if(mic1_short == 1){
            img_view(40, 3, 42, _m1_short_42, 0);
        }
    } else {
        if (config.mic1_mode == 1) {
             lcd_gotoxy(0, 7);
             lcd_putstr(" M1 ", m1_inv);
        } else {
            lcd_gotoxy(0, 7);
            lcd_putstr(" A1 ", m1_inv);
        }
        if(((all_alarms>> 2) & 1) == 1  && m1_inv == 0){
             lcd_gotoxy(0, 7);
             lcd_putstr(" IN1 ", 1);            
 
        }
        
        if(((all_alarms>> 5) & 1) == 1  && m1_inv == 0){
             lcd_gotoxy(0, 7);
             lcd_putstr(" M3 ", 1);            
        }
    }
    // MIC2   5.7
    if(config.zone_sel2 == 3 || (all_errors1 >> 7) & 1) {
        lcd_gotoxy(24,7);
        lcd_putstr("    ",0);
//        if(mulf_screen1 == 1 && config.zone_sel1 != 3) {
//            img_view(40, 3, 48, _mic2_cut_89, 0);
//            //clr1 = 1;
//        }
       if (mic2_cut == 1) {
           img_view(40, 3, 48, _mic2_cut_89, 0);
            //clr1 = 1;
        }
        if(mic2_short == 1){
            img_view(40, 3, 42, _m2_short_42, 0);
        }
    }else{
        lcd_gotoxy(24,7);
        lcd_putstr(" M2  ",m2_inv);
        
        if(((all_alarms>> 3) & 1) == 1 && m2_inv == 0){
           lcd_gotoxy(24, 7);
           lcd_putstr(" IN2 ", 1);            
 
        }
    }     
    
    
        // MIC3   5.7
    if(config.zone_sel3 == 3 || (all_errors1 >> 3) & 1) {
//        if(mulf_screen1 == 1 && config.zone_sel1 != 3) {
//            img_view(40, 3, 48, _m3_cut_48, 0);
//           // m3_inv = 1;
//        }
        if (mic3_cut == 1) {
           img_view(40, 3, 48, _m3_cut_48, 0);
            //clr1 = 1;
        }
        if(mic3_short == 1){
            img_view(40, 3, 42, _m3_short_42, 0);
        }
    }   
   // if(config.zone_sel3 == 3 || (all_errors1 >> 3) & 1){lcd_gotoxy(30,7);lcd_putstr("    ",0);if(mulf_screen1 == 1 && config.zone_sel1 != 3){img_view(20, 3, 89, _mic2_cut_89, 0);clr1 = 1;}}else{lcd_gotoxy(30,7);lcd_putstr(" M2 ",m2_inv);}     // MIC2   30.7
        

    if(config.mode != 1){
    if((sp_errors>> 6) & 1){lcd_gotoxy(55,7);lcd_putstr("    ",0);}else{lcd_gotoxy(55,7);lcd_putstr(" L1 ",l1_inv);} //55.7
    if((sp_errors >> 7) & 1){lcd_gotoxy(80,7);lcd_putstr("    ",0);}else{lcd_gotoxy(80,7);lcd_putstr(" L2 ",l2_inv);} //80.7
    
    }else{
        if((sp_errors >> 6) & 1){lcd_gotoxy(55,7);lcd_putstr("        ",0);}else{lcd_gotoxy(55,7);lcd_putstr("  L1  ",l1_inv);} //55.7
    }

    if (config._485 == 0) {
        lcd_gotoxy(90, 0);
        lcd_putstr("   ", 0);
    } else {
        lcd_gotoxy(95, 0);
        if(config._485 != 21){
        sprintf(scr_buff, "%d", config._485);
        lcd_putstr(scr_buff, 0);
        }else{lcd_putstr("M", 0);
        }
        rs_icon();
    }//88.0 . ??_icon lcd_gotoxy(90,0);

//    if (buzzer_silenced) {
//       lcd_draw_icon(15, 0, bell_icon); // Draw at column 110, page 0
//        }else {
//          lcd_draw_icon(15, 0, empty_icon); // Clear icon by overwriting with blank data
//    }
    //if((all_errors1 >> 0) & 1){lcd_gotoxy(35,0);lcd_putstr("   ",0);}else{lcd_draw_icon(35, 0, battery_icon);} // lcd_gotoxy(50,0);lcd_putstr("DC±",0);} //
    //if((all_errors1 >> 1) & 1){lcd_gotoxy(30,0);lcd_putstr("   ",0);}else{lcd_draw_icon(60, 0, ac_icon);}//lcd_gotoxy(30,0);lcd_putch('~',0);lcd_putstr("AC",0);}
    
};

unsigned char PTT_3(void) {
    return p_t_t_3;  // Assuming BUSY pin is connected to RA5
}

unsigned char PTT_4(void) {
    return p_t_t_4;  // Assuming BUSY pin is connected to RA5
}

//unsigned char PTT_5(void) {
//    return p_t_t_5;  // Assuming BUSY pin is connected to RA5
//}






//void __interrupt() isr(void) {
//
//    //---------- TIMER0 Interrupt (every ~10ms) ----------//
//    if (INTCONbits.TMR0IF) { //TMR0 interrupt flag
//        INTCONbits.TMR0IF = 0; // Clear Timer0 interrupt flag
//        unsigned int current_error_state = (sp_errors << 8) | all_errors1;
//        //==================== Line Test ====================//
//        // Only update the line-test timer if:
//        //   - No alarms active,
//        //   - Not already performing a line test,
//        //   - And no errors exist (sp_errors and all_errors1 are zero).
//        
//        if ((all_alarms == 0) && (line_test_on != 1)){// && (sp_errors == 0) && (all_errors1 == 0)) {                 
//            tmr_line_tst++;
//            if (tmr_line_tst >= line_fast_check) {  // e.g. 6000 for 1 minute normally
//                adc_en = 1;
//                tmr_line_tst = 0; // reset the counter after triggering the test
//            }
//        } else {
//            // If errors or alarms exist, reset the line test counter.
//            tmr_line_tst = 0;
//        }
//        // Adjust line test interval based on error condition.
//        line_fast_check = (sp_errors != 0) ? 100 : tmr_line_tst_set;
//      
//        //==================== Power Test ====================//
//        if (pwr_tst == 0) {
//            pwr_tmr++;
//        }
//        if (pwr_tmr >= pwr_fast_check) { // normally 3000 ticks for 30 sec, or 100 for fast mode
//            pwr_tmr = 0;
//            pwr_tst = 1;
//        }
//        
//        pwr_fast_check = (all_errors1 != 0) ? 100 : 3000;
//        
//        //==================== BackLight Control ====================//
//        if(BACK_LIGHT == 1 && (all_errors1 == 0) && (all_alarms == 0) && m1 == 0 && m2 == 0 && m3 == 0 && m4 == 0){
//            backlight_tmr++;
//            if (backlight_tmr >= 6000) { // 1 minute threshold
//                backlight_tmr = 0;
//                //backlight_en = 0;
//                BACK_LIGHT = 0;
//                //PWM_Set_Duty(20);
//            }
//            
//        }
//        //==================== Emergency Call Timer ====================//
//        em_call_tmr++;
//        if (em_call_tmr == 100) {
//            em_call_tmr = 0;
//            em_call = 1;
//        }
//
//        //==================== Blink Error LED & Buzzer ====================//
//        if (current_error_state != last_error_state && current_error_state != 0) {
//    // New or additional error appeared, cancel silence
//                buzzer_silenced = 0;
//                buzzer_silence_tmr = 0;
//            }
//            last_error_state = current_error_state;
//        if (buzzer_silenced) {
//               buzzer_silence_tmr++;
//               if (buzzer_silence_tmr >= 90000) { // 15 minutes
//                        buzzer_silenced = 0;
//                        buzzer_silence_tmr = 0;
//                    }
//                }
//            
//        if (current_error_state != 0) {
//            mlfnction = 1;
//            _one_step = 0;           
//            tmr0_blink_led++;  // Increment blink counter
//            if (tmr0_blink_led >= 100) { // e.g., toggle every 100 ticks
//                tmr0_blink_led = 0;
//                if (mlfn_neg == 0) {
//                    mlfn_neg = 1;
//                    ERR_LED = 1;
//                    //buzzer = 1;
//                    if (!buzzer_silenced) buzzer = 1;
//                } else {
//                    mlfn_neg = 0;
//                    ERR_LED = 0;
//                    buzzer = 0;
//                }
//            }
//        } else {
//            ERR_LED = 0;
//            buzzer = 0;
//            mlfnction = 0;
//            buzzer_silenced = 0;
//            buzzer_silence_tmr = 0;
//            last_error_state = 0;
//        }
//        
//        //==================== Back to Main Screen Timer ====================//
//        if (m1 == 1 || m2 == 1 || m3 == 1 || m4 == 1) {
//            back_to_m_tmr++;
//            if (back_to_m_tmr >= 6000) {
//                back_to_m_tmr = 0; 
//                exit_en = 1;
//                m1 = m2 = m3 = m4 = 0;
//                rs_ = 1;
//                m_a = 1;
//                aud = 1;
//                //BACK_LIGHT = 0;
//            }
//        } else {
//            back_to_m_tmr = 0;
//        }
//        
//        //==================== RS Timer (if active) ====================//
//        if (rs_tmr_on == 1) {
//            rc_time++;
//            if (rc_time >= 100) {
//                rc_time = 0;
//                success = 0;
//                rs_tmr_on = 0;
//            }
//        }
//        
//        //============= Reload Timer0 for next 10ms interval =============//
//        TMR0 = TMR0_RELOAD;                                                     // 65536 - ((16000000 / 4) / 256 / 100);  // Reload Timer0
//
//    } // End of Timer0 ISR section