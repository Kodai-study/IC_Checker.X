/*
 * File:   newmain.c
 * Author: gorin
 *
 * Created on 2022/07/14, 19:50
 */

// PIC18F4620 Configuration Bit Settings

// 'C' source line config statements


// PIC18F4620 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1H
#pragma config OSC = INTIO67     // Oscillator Selection bits (Internal oscillator block, CLKOUT function on RA6, port function on RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-003FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (004000-007FFFh) not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (008000-00BFFFh) not code-protected)
#pragma config CP3 = OFF        // Code Protection bit (Block 3 (00C000-00FFFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-003FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (004000-007FFFh) not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (008000-00BFFFh) not write-protected)
#pragma config WRT3 = OFF       // Write Protection bit (Block 3 (00C000-00FFFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-003FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (004000-007FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (008000-00BFFFh) not protected from table reads executed in other blocks)
#pragma config EBTR3 = OFF      // Table Read Protection bit (Block 3 (00C000-00FFFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot Block (000000-0007FFh) not protected from table reads executed in other blocks)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include <stdio.h>
#include "fucntions.h"
#include "datas.h"


#define LED_BLUE LATAbits.LA0
#define LED_RED LATAbits.LA1
#define LED_GREEN LATAbits.LA2

_Bool stop = 0;
_Bool t0_flg = 0;   //1msごと、タイマ0の割り込みが起きたときに1になるフラグ
_Bool sw1_flg = 0;  //スイッチ1が押された時に1になる
_Bool sw2_flg = 0;  //スイッチ2が押されたときに1になる
int hoge_count = 0;
char rx_buf = 0;    //USARTで受け取った値を保存
int mode = 1;       
static const char dot = 0b10100101;     //LCDで、選択項目を表すドットのデータ
static const char blank = 0b10100000;   //LCDで、空白を表示させるデータ
static const char allow = 0x7e;         //右矢印〃

const char* ic_names[] = {  //LCDに表示するための、チェック項目の文字列
    "74LS74 ",
    "74LS00 ",
    "74LS02 ",
    "LM555 ",
    "74LS393",
    "74LS195"
};
const int ic_kinds = sizeof(ic_names) / sizeof(char*);  //チェックする項目の数

const char* mode_names[] = {    //チェックするモードを表示する文字列
    " ALLﾁｪｯｸ",
    " ﾀﾝﾀｲﾁｪｯｸ"
};

//CHECK_RESULT results[10] = {NO_CHECK};  //チェック項目の結果を表す
CHECK_RESULT results[10] = {OK,OK,OK,OK,OK,OK,OK,OK,OK,OK}; ///デバッグ用
char st[2][17] = {0};                   //LCDに表示する文字列
int ng_count = 0;                       //チェックが失敗した項目の数をカウント
MODE now_mode = HOME;
int select_item = -1;

void main() {
    //sprintf(st[0],"IC_%s",ic_names[0]);
    
    OSCCON = 0b01100000;    //クロックを4MHzに設定
    /* 優先度なしで、割り込みを許可 */
    RCONbits.IPEN = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    SW_TRIS = 0;
    TRISA = 0xff;
    TRISB = 0x00;
    LATB = 0x03;
    LATA = 0b10101010;
    ADCON1 = 0x0f;      //全てディジタルピンに設定
    
    LATBbits.LATB0 = 1;
    LCD_Init();
    usart_init();
    comp_init();
    LCD_String("start\n");
    LCD_CursorOff();
    POWER_SW = 1;
    
    /* 1msごとのタイマ割り込みを設定 */
    T0CON = 0b10000000;
    TMR0H = (65035 >> 8);
    TMR0L = (65035 & 0xff);
    INTCONbits.T0IE = 1;
    
    SW2_TRIS = 1;
    SW1_TRIS = 1;
    while(1){
        switch(now_mode){
            case HOME : menu_mode(); break;
            case CHECK_SELECT : select_check(); break;
            case SINGLE_TEST:  LCD_String(ic_names[select_item]); single_check(select_item); break;
            default : break;
        }
       /* if(count_check() == OK){
            LATBbits.LATB3 = 1;
            LCD_String("OK");
            
        } else{
            LATBbits.LATB3 = 0;
            LCD_String("NG");
        }*/
    }
    
       /**  実装予定のコード
    *  TRISA = 0x00; 
    TRISB = 0x00;
    LATA = 0b00001001;    
    
    ///　測定中は黄色に点灯
    LED_RED = 0;
    LED_GREEN = 0;
    LED_BLUE = 1;
    
    results[0] = dff_Check();
    results[1] = nand_check(0);
    
    results[2] = NG;
    results[3] = NG;
    results[1] = NG;

    ///　エラー項目があれば赤色に点灯
    if(ng_count > 0){
        LED_RED = 0;
        LED_BLUE = 1;
        LED_GREEN = 1;
    } else {        ///OKならば緑色に点灯
        LED_GREEN = 0;
        LED_BLUE = 1;
        LED_RED = 1;
    }
        NGの数を数えて、NG項目をLCDに表示する。
    for(int i = 0;i < 10;i++){
        if(results[i] != OK){
            switch(ng_count){
                case 0 : LCD_String(ic_names[i]);
                    LCD_String("\n"); break;
                case 1 : LCD_String(ic_names[i]); break;
                case 2 : LCD_Locate(1,15); LCD_Character(0x7e); break;  //NG項目が3つ以上の時は、矢印を出してスクロールさせる
                default : break;
            }
            ng_count++;
        }
    }
    */

    return;
}

/* 単体チェックか、全てをチェックするかを選択する */
void menu_mode(){
    LCD_Clear();        
    static unsigned int cur = 0;     //選択モードがどちらかを格納
    SW1_TRIS = 1;           
    SW2_TRIS = 1;
    
    /* 初期は全てチェックモード */       
    LCD_String(mode_names[0]);
    LCD_String("\n");
    LCD_String(mode_names[1]);
    LCD_Locate(cur,0);
    LCD_Character(dot);
    while(1){
        T0_WAIT
        sw_check();     //1msごとに、スイッチが押されたかどうかチェック
        /* スイッチ1が押されたら、選択モードを変更 */
        if(sw1_flg != 0){       
            sw1_flg = 0;
            LCD_Locate(cur,0);
            LCD_Character(blank);
            cur = !cur;
            LCD_Locate(cur,0);
            LCD_Character(dot);
        }
        /* スイッチ2が押されたら、選択されたモードに移行 */
        if(sw2_flg != 0){
            sw2_flg = 0;
            now_mode = (cur == 0) ? ALL_CHECK : CHECK_SELECT;
            return;
        }
    }
}

/* 単体チェックモードで、チェックする項目を決める */
void select_check(){
    LCD_Clear();
    static unsigned int cur = 0;
    /* 最初の0,1個めを表示する */
    LCD_Character(dot);
    LCD_String(ic_names[cur]);
    LCD_String("\n ");
    LCD_String(ic_names[cur + 1]);
    LCD_Locate(1,15);
    LCD_Character(allow); 
    while(1){
        T0_WAIT;
        sw_check();
        if(sw1_flg != 0){
            LCD_Clear();
            sw1_flg = 0;
            cur++;
            
            /* 項目を1つずらして表示 */
            if(cur < ic_kinds - 1){
                LCD_Character(dot);
                LCD_String(ic_names[cur]);
                LCD_String("\n ");
                LCD_String(ic_names[cur + 1]);
                if(cur < ic_kinds - 2){
                    LCD_Locate(1,15);
                    LCD_Character(allow);       //まだ下に項目があるときは、矢印を表示
                }
            } else if(cur == ic_kinds - 1){
                LCD_Character(blank);
                LCD_String(ic_names[cur - 1]);
                LCD_String("\n");
                LCD_Character(dot);
                LCD_String(ic_names[cur]);
            } else {
                cur = 0;
                LCD_Character(dot);
                LCD_String(ic_names[0]);
                LCD_String("\n ");
                LCD_String(ic_names[1]);
                LCD_Locate(1,15);
                LCD_Character(allow); 
            }
        }
        
        if(sw2_flg != 0){
            LCD_Clear();
            LCD_Number(cur);
            select_item = cur;
            LCD_Number(select_item);
            
            now_mode = SINGLE_TEST;
            LCD_Number(now_mode);
            return;
        }
        
    }
}

void __interrupt ( ) isr (void){
    if(PIR2bits.CMIF != 0){ //コンパレータの出力が変化した割り込み。
        PIR2bits.CMIF = 0;
        if(CMCONbits.C1OUT != 0){       //C1の出力が基準電圧を超えた→過電流
            POWER_SW = 0;               //電源を停止
            current_over();
            stop = 1;                  
        }
        LCD_Number(CMCONbits.C1OUT);
    }
    /* USARTで値を受け取ったとき */
    if(PIR1bits.RCIF != 0){
        PIR1bits.RCIF = 0;
        rx_buf = RCREG;
        LCD_Number(RCREG);
    }
    /* タイマ0割り込み(1ms)が起きたとき */
    if(INTCONbits.TMR0IF != 0){
       INTCONbits.TMR0IF = 0;
        TMR0H = (65035 >> 8);
        TMR0L = (65035 & 0xff);
        t0_flg = 1;
    }
}
