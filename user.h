
#include <stdbool.h>


// CONFIG1H
#pragma config FOSC = INTIO67   // Oscillator Selection bits (Internal oscillator block)
#pragma config PLLCFG = ON      // 4X PLL Enable (Oscillator multiplied by 4)
#pragma config PRICLKEN = OFF   // Primary clock enable bit (Primary clock can be disabled by software)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRTEN = OFF     // Power-up Timer Enable bit (Power up timer disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 220       // Brown Out Reset Voltage bits (VBOR set to 1.90 V nominal)

// CONFIG2H
#pragma config WDTEN = OFF      // Watchdog Timer Enable bits (WDT is always enabled. SWDTEN bit has no effect)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
//#pragma config CCP2MX = PORTC1  // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<5:0> pins are configured as analog input channels on Reset)
//#pragma config CCP3MX = PORTB5  // P3A/CCP3 Mux bit (P3A/CCP3 input/output is multiplexed with RB5)
#pragma config HFOFST = ON      // HFINTOSC Fast Start-up (HFINTOSC output and ready status are not delayed by the oscillator stable status)
//#pragma config T3CMX = PORTC0   // Timer3 Clock input mux bit (T3CKI is on RC0)
//#pragma config P2BMX = PORTD2   // ECCP2 B output mux bit (P2B is on RD2)
#pragma config MCLRE = EXTMCLR  // MCLR Pin Enable bit (MCLR pin enabled, RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = ON         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled if MCLRE is also 1)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection Block 0 (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection Block 1 (Block 1 (002000-003FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection Block 2 (Block 2 (004000-005FFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection Block 3 (Block 3 (006000-007FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection Block 0 (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection Block 1 (Block 1 (002000-003FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection Block 2 (Block 2 (004000-005FFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection Block 3 (Block 3 (006000-007FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection Block 0 (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection Block 1 (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection Block 2 (Block 2 (004000-005FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection Block 3 (Block 3 (006000-007FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

 #define _XTAL_FREQ 16000000

//======================== Buttons============================================//
unsigned int butt_count=0;
/****************************************************************/
static unsigned char cnt_but = 20;    // 
#define PERIOD_BUTT_TICK    10      //
#define BUTTON_ACT_LEVEL    0       // 
/****************************************************************/
typedef struct {
    unsigned        stat: 1 ;
    unsigned        prev: 1 ;
    unsigned        click: 1;
    unsigned int    cnt_ms  ;
}T_Butt;
/****************************************************************/
#define Butt_Init(_name, _port_name, _bit_num)                 \
T_Butt  _name;                                                  \
const unsigned char _name##_bit = _bit_num;                    \
const unsigned int  _name##_port = (unsigned int)&_port_name
/****************************************************************/
#define Butt_Tick(_name)                            \
_name.stat=!BUTTON_ACT_LEVEL;                       \
if(*((unsigned int*)_name##_port)&(1<<_name##_bit)) \
    _name.stat=BUTTON_ACT_LEVEL;                    \
if(_name.prev && !_name.stat && (_name.cnt_ms<500)) \
    _name.click=1;                                  \
if(_name.stat)                                      \
    _name.cnt_ms+=PERIOD_BUTT_TICK;                 \
else                                                \
    _name.cnt_ms=0;                                 \
_name.prev=0;                                       \
if(_name.stat)                                      \
    _name.prev=1
/****************************************************************/

	Butt_Init(_right1,       PORTE, 2);
	Butt_Init(_right2,       PORTC, 0);
	Butt_Init(_left1,      PORTE, 0);
    Butt_Init(_left2,      PORTE, 1);
//===========================Main=============================================//

#define PWR_LED             LATBbits.LATB6
#define ERR_LED             LATBbits.LATB7 
 
#define buzzer              PORTAbits.RA6
      
#define ac_                 PORTBbits.RB0 
#define batt_               PORTAbits.RA2     
    
#define p_t_t_3             PORTAbits.RA5 
#define p_t_t_4             PORTAbits.RA7 
//#define p_t_t_5             PORTAbits.RA3 
    
#define RLY_R               PORTBbits.RB4               //LATBbits.LATB4  //PORTBbits.RB4
#define RLY_L               PORTBbits.RB5               //LATBbits.LATB5   //PORTBbits.RB5  
    
#define fault_amp           PORTBbits.RB2
    
#define DATA_PIN            PORTDbits.RD5   // DS
#define SRCLK_PIN           PORTDbits.RD4   // SHCP
#define RCLK_PIN            PORTBbits.RB1   // STCP
    
// Define bool if not supported
//typedef unsigned char bool;
    
    unsigned int shiftRegisterData = 0b0001010000000000;
    
//                           | | | |  | | | |
 //   shiftRegisterData = 0b00010100 00000000;
//                          | | | |  | | | |     
    
    unsigned char zone1State = 0;
    unsigned char zone2State = 0;
    unsigned char dry_c = 0;
    unsigned char _zone1 = 0;
    unsigned char _zone2 = 0;
    
unsigned char adc_en = 0;
unsigned char press_but = 0;
unsigned char first_menu = 0;
unsigned char mlfnction = 0;
unsigned char mlfn_neg = 0;
unsigned char _one_step = 0;
unsigned char m1_inv = 0;
unsigned char m2_inv = 0;
unsigned char m3_inv = 0;
unsigned char l1_inv = 0;
unsigned char l2_inv = 0;
unsigned char exit_en = 0;

unsigned char mulf_screen1 = 0;
volatile unsigned int last_error_state = 0;
// current_error_state = 0;

    char _no = 0;
    
extern void sys_init(void);
extern void push_butt(void);

extern void _first_run_(unsigned char en,unsigned char en2);
extern void zone_on_off(unsigned char rly1,unsigned char rly2);
extern void mulfunctions_(unsigned char _neg);
extern void clear_scree_(void);
extern void main_icon_(unsigned char en);
extern void SendShiftData595(unsigned int data);
extern void updateBit(unsigned int *data, unsigned int bitPosition, char value);    
extern void action(unsigned char z1,unsigned char z2,unsigned int shuft);

extern void line_check_presentes(unsigned char presente);

unsigned char PTT_3(void);
unsigned char PTT_4(void);
unsigned char PTT_5(void);


//===========================TMR=============================================//

#define TMR0_RELOAD (65536 - ((16000000UL / 4UL) / 256UL / 100UL)) // ~10ms

unsigned int tmr0_blink_led = 0;
unsigned int tmr0_bip = 0;

unsigned int tmr_line_tst = 0;
unsigned long tmr_line_tst_set = 0;
unsigned char line_test_on = 0;

unsigned int pwr_tmr = 0;
unsigned int pwr_fast_check = 0;
unsigned long line_fast_check = 0;

unsigned char pwr_tst = 0;
unsigned int em_call_tmr = 0;
unsigned int back_to_m_tmr = 0;

volatile unsigned char buzzer_silenced = 0;
volatile unsigned long buzzer_silence_tmr = 0; // 10ms tick, 15 min = 90000 ticks




extern void init_TMR0(unsigned char en);
extern void init_TMR5(unsigned char h ,unsigned char EN,unsigned char int_en);

//============================PWM=============================================//
#define BACK_LIGHT  PORTCbits.RC1

unsigned int backlight_tmr = 0;

extern void CCP2_Init(void);
void PWM_Set_Duty(unsigned int duty);
            
//=========================MENU===============================================//


typedef struct {
    unsigned int ok_min, ok_max;
    unsigned int pr_min, pr_max;
    unsigned int ct_min, ct_max;
    unsigned int sh_min, sh_max;
} MicThresholds;

// ????????? ?????? MIC1
const MicThresholds mic1ThresholdsDefault = {
    .ok_min = 480, .ok_max = 515,
    .pr_min = 300, .pr_max = 350,
    .ct_min = 900, .ct_max = 1023,
    .sh_min = 0,   .sh_max = 5
};


//// ----- ??????? ?????? MIC1 -----
//extern MicThresholds mic1Thresholds;
//
//// ----- ????????? ?????? MIC1 -----
//extern const MicThresholds mic1ThresholdsDefault;


unsigned char m1 = 0;
unsigned char m2 = 0;
unsigned char m3 = 0;
unsigned char m4 = 0;

unsigned char contr = 0;
unsigned char contr1 = 0;
unsigned char m_a = 0;
unsigned char rs_ = 0;
unsigned char m_t = 0; 

unsigned char aud = 0;

unsigned char debug_screen = 0;

char w,choise,end,end2,yes,no,x;

 struct zone {
    char Zones;
	char MSG;

};
struct zone counter[20];

extern void main_screen(void);

extern void menu1(void);
extern void menu2(void);
extern void menu3(void);
extern void menu4(void);

extern void _klali_(void);
extern void _rs_comm_(void);
extern void _resete_all(void);
extern void _about(void);
extern void _audio_sett(void);
extern void adc_monitor(void);
extern void gen_dac(void);
extern void settings_start_screen(void);
extern void sys_timer_set(void);
extern void first_screen(void);
extern void tmr_drop_menu(unsigned char en);

void increaseParamMin(MicThresholds *mt, unsigned char param);
void decreaseParamMin(MicThresholds *mt, unsigned char param);
void increaseParamMax(MicThresholds *mt, unsigned char param);
void decreaseParamMax(MicThresholds *mt, unsigned char param);
void adc_menu_MIC1(void);
void adc_menu_MIC2(void);
void adc_menu_MIC3(void);
void adc_menu_SP1(void);



//==========================MICs/PTTs===============================================//

// Define state constants
#define STATE_MIC_OFF 0
#define STATE_MIC_OK 1
#define STATE_MIC_PRESET 2
#define STATE_MIC_CUT 3
#define STATE_MIC_SHORT 4


#define STATE_ON_ptt  0 
#define STATE_OFF_ptt 1
#define STATE_OFF 0

// Define thresholds
//#define THRESHOLD_MAX_MIC_OK 511     // Threshold for Mic-OK state
//#define THRESHOLD_MIC_PRESET 351 // Threshold for Mic-Preset state
#define THRESHOLD_MIC_CUT 1023   // Threshold for Mic Cut state

    unsigned char one_step1 = 0;
    unsigned char one_step2 = 0;
unsigned char mic1_err_scr = 0;
unsigned char mic2_err_scr = 0;


                              //        M3 PTT4 MIC2
                              //    |   |   |   |
unsigned char all_alarms = 0; // 0b 0 0 0 0 0 0 0 0                             // all_alarms|=(1<<0); - SET first BIT
                              //      |   |   |   |                             // all_alarms &=~(1<<0); - CLEAR first BIT
                              //         RS PTT3 MIC1
unsigned char mic3_cut = 0;

unsigned char em_call = 0;

int Mic1_adcValue = 0;
int Mic2_adcValue = 0;

//float mic_1 = 0.0;

int statePTT1 = 0;
int PTT2 = 0;

char vol_set1 = 0;
char vol_set2 = 0;
char vol_set3 = 0;

char mic1_short = 0;
char mic2_short = 0;
char mic3_short = 0;

char mic1_cut = 0;
char mic2_cut = 0;
//char mic3_cut = 0;

unsigned char press1 = 0;
unsigned char press2 = 0;
unsigned char press3 = 0;
unsigned char press4 = 0;
unsigned char press_all = 0;

unsigned char mic1_not_prs = 0;
unsigned char mic2_not_prs = 0;

unsigned char clr1 = 0;
unsigned char clr2 = 0;
unsigned char clr3 = 0;

unsigned char msg_press1 = 0;
unsigned char msg_press2 = 0;
unsigned char msg_press3 = 0;
unsigned char msg_press4 = 0;
unsigned char msg_press5 = 0;
unsigned char msg_press6 = 0;

unsigned int lastMessagePlayed = 0;
unsigned char shift_mic1_mode = 0;

//extern unsigned int detectStatePTT(unsigned char zone_sel, char adcChannel);
extern unsigned int detectStatePTT(unsigned char zone_sel, char adcChannel, MicThresholds *mt);
extern void handleEmergencyCalls(unsigned char en);
extern void dry_cont_state(unsigned char st);
extern void handleDryCont_3_4(unsigned char st);

extern void mic1_state(unsigned int PTT1);
extern void mic2_state(unsigned int PTT2);
extern void mic3_state(unsigned int PTT3);

//===========================SP_Line===========================================//
//
//#define LINE_STATE_OK 0
//#define LINE_STATE_CUT 1
//#define LINE_STATE_SHORT 2
//#define LINE_STATE_GROUND_FAULT 3

// Define constants for line states if not defined elsewhere
#define LINE_STATE_OK           1
#define LINE_STATE_CUT          2
#define LINE_STATE_SHORT        3
#define LINE_STATE_GROUND_FAULT 4
#define LINE_STATE_UNKNOWN      0

// Define constants for line states if not defined elsewhere
//#define LINE_STATE_OK           1
//#define LINE_STATE_CUT          2
//#define LINE_STATE_SHORT        3
//#define LINE_STATE_GROUND_FAULT 4
//#define LINE_STATE_UNKNOWN      0
//
#define ADC_OK_MIN 200  // Example value, adjust based on actual ADC readings
#define ADC_OK_MAX 230  // Example value, adjust based on actual ADC readings
#define ADC_OPEN_MIN 310 // Example value, adjust based on actual ADC readings // 320
#define ADC_OPEN_MAX 370 // Example value, adjust based on actual ADC readings // 355
#define ADC_SHORT_MIN 80   // 0V is the minimum for short //80
#define ADC_SHORT_MAX 145  // Example: 0.25V as the max for short // 130
#define ADC_GROUND_FAULT_MIN 0 // Example: 0.25V as the min for ground fault
#define ADC_GROUND_FAULT_MAX 195 // Example: assuming 1.5V as max for ground f 185


extern char detectLineState(char channel);
extern void handleState(unsigned char lineState,unsigned char lineOffset,unsigned char all_err);
extern void handleLineStates(unsigned char en);

//================================ADC==========================================//


extern void ADC_init(void);
extern int readADC(unsigned char channel);

//===============================Screens======================================//

char scr_buff[16];

extern void print_scrn(void);
extern void startup_logo_leds(void);
extern void m_screen(unsigned char i);

//==============================Malfunction====================================//

unsigned char blink_en = 0;//

                                //   M2  M1  M3  AC
                               //    |   |   |   |
unsigned char all_errors1 = 0; // 0b 0 0 0 0 0 0 0 0
                               //      |   |   |   |      
                               //     AMP RS  FN  Batt


                                // L2 gnd open2 short1
                               //  |   |   |   |
unsigned char sp_errors = 0; // 0b 0 0 0 0 0 0 0 0
                               //    |   |   |   |      
                               //  L1 short2 gnd1 open1


    unsigned char p1=0;
    unsigned char p2=0;
    
    unsigned char p3 = 0;
    unsigned char p4 = 0;
    unsigned char p5 = 0;
    unsigned char p6 = 0;
    
    unsigned char a = 0;
    unsigned char b = 0;
    unsigned char c = 0;
    unsigned char d = 0;
    unsigned char exit_menu = 0;


extern void sys_malfunction(void);
extern void sys_adc_tst(unsigned char i);
extern void print_errors_(void);
extern void print_sp_errors_(void);


//============================EPPROM===========================================//

#define memory_blank                    0x00
#define EEPROM_ADDR_MODE                0x01

#define EEPROM_ADDR_MIC1                0x02
#define EEPROM_ADDR_MIC2                0x03
#define EEPROM_ADDR_MIC3                0x08

#define EEPROM_ADDR_AUX                 0x09

#define EEPROM_ADDR_dry1                0x04
#define EEPROM_ADDR_dry2                0x05

#define EEPROM_ADDR_msg1                0x06
#define EEPROM_ADDR_msg2                0x07




#define EEPROM_ADDR_CONTRAST            0x10

#define EEPROM_ADDR_dutycycle           0x11

#define EEPROM_ADDR_lang                0x12

#define EEPROM_ADDR_485                 0x13

#define EEPROM_ADDR_line_timer          0x14

#define EEPROM_ADDR_MSG_VOLUME          0x15
#define EEPROM_ADDR_MIC1_VOLUME         0x16
#define EEPROM_ADDR_MIC2_VOLUME         0x17
#define EEPROM_ADDR_MIC3_VOLUME         0x18

#define EEPROM_ADDR_SLAVE_COUNT         0x19

#define EEPROM_ADDR_ok_min         0x20// low @0x20, high @0x21
#define EEPROM_ADDR_ok_max         0x22// low @0x22, high @0x23
#define EEPROM_ADDR_pr_min         0x24
#define EEPROM_ADDR_pr_max         0x26
#define EEPROM_ADDR_ct_min         0x28
#define EEPROM_ADDR_ct_max         0x2A
#define EEPROM_ADDR_sh_min         0x2C
#define EEPROM_ADDR_sh_max         0x2E




//
typedef struct {
    unsigned char mem_blank;
    unsigned char MSG;
    unsigned char mic1_mode;
    unsigned char msg_volume;
    unsigned char mic1_volume;
    unsigned char mic2_volume;
    unsigned char mic3_volume;
    unsigned char mode ;
    unsigned char zone_sel1 ;
    unsigned char zone_sel2 ;
    unsigned char zone_sel3 ;
    unsigned char lang;
    unsigned char _485;
    unsigned char slave_count;
    unsigned char line_timer;
} ConfigData;

ConfigData config;

extern void LoadConfigData(void);
extern void EEPROM_Write(unsigned char address, unsigned char data);
extern unsigned char EEPROM_Read(unsigned char address);
extern void EEPROM_Write_Int(unsigned char address, unsigned int data);
extern unsigned int EEPROM_Read_Int(unsigned int address);

extern void EEPROM_Write16(unsigned char addr, unsigned int value);
extern unsigned int EEPROM_Read16(unsigned char addr);


extern void handleMIC3(void);
extern void loadMic1Thresholds(void);
extern void saveMic1ThresholdsToEEPROM(void);

//============================UART============================================//

#define UART1_BUFFER_SIZE 15
//#define UART2_BUFFER_SIZE 5

//unsigned char uartBuffer[UART2_BUFFER_SIZE];
//unsigned char uartBufferIndex = 0;
//unsigned char uartDataReady = 0;

extern void uart1_init(unsigned long baudrate);
extern void uart2_init(unsigned long baudrate);
extern void uart2_write(uint8_t data);

//=============================RS485==========================================//

#define RS485_TX_MODE PORTBbits.RB3 = 1   // RC5 = 1 for transmit mode
#define RS485_RX_MODE PORTBbits.RB3 = 0   // RC5 = 0 for receive mode

#define BUFFER_SIZE 5

volatile unsigned char rx_buffer[BUFFER_SIZE];
volatile unsigned char rx_index = 0;
volatile unsigned char data_ready = 0;                                          // Flag to indicate that data is ready to be processed
volatile uint8_t response_sent = 0; // Flag to indicate response has been sent

unsigned char current_slave = 1;
unsigned char success = 0;
unsigned char rs_data = 0;
unsigned char rs_tmr_on = 0;
unsigned int rc_time = 0;

extern void UART1_Write(uint8_t data);
extern uint8_t UART1_Read(void);
extern void RS485_Send(uint8_t address, uint8_t command, uint8_t data);
extern uint8_t RS485_Receive(void);
extern void Poll_Slaves(void);
extern uint8_t Calculate_Checksum(uint8_t address, uint8_t command, uint8_t data);
extern void RS485_Master_Task(void);


void RS485_Slave_Task(void);
void Handle_Command(uint8_t address, uint8_t command, uint8_t data);
void RS485_Send_Response(uint8_t address, uint8_t response_data);

//============================DY1703A==========================================//
 

#define FN_busy     PORTAbits.RA4

#define CMD_SET_VOLUME            0x13
#define DEVICE_FLASH              0x02
#define MODE_FULL_CYCLE           0x00
#define MODE_SINGLE_CYCLE         0x01
#define MODE_SINGLE_STOP          0x02


unsigned char packet[5];

extern uint8_t is_module_busy(void);
extern void JQ8900_SendCommand(uint8_t cmd_type, uint8_t data_length, uint8_t* data);
extern void jq8900_init(void);
extern void jq8900_setVolume(uint8_t volume);
extern void jq8900_stop(void);
extern void jq8900_playTrack(uint8_t trackNumber);



