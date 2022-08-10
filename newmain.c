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
#include "lcdlib_xc8_v03.h"
#include "datas.h"


#define LED_BLUE LATAbits.LA0
#define LED_RED LATAbits.LA1
#define LED_GREEN LATAbits.LA2


const char* ic_names[] = {  //LCDに表示するための、チェック項目の文字列
    " 74LS74 ",
    " 74LS00 ",
    " 74LS02 ",
    " LM555 ",
};
//CHECK_RESULT results[10] = {NO_CHECK};  //チェック項目の結果を表す
CHECK_RESULT results[10] = {OK,OK,OK,OK,OK,OK,OK,OK,OK,OK}; ///デバッグ用
char st[2][17] = {0};                   //LCDに表示する文字列
int ng_count = 0;                       //チェックが失敗した項目の数をカウント
void main() {
    //sprintf(st[0],"IC_%s",ic_names[0]);
    
    OSCCON = 0b01100000;
    RCONbits.IPEN = 0;
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    //LCD_String(st[0]);
    TRISA = 0x00;
    TRISB = 0x00;
    LATA = 0b10101010;
    ADCON1 = 0x0f;
    //TMchecker();
    //viewResults();
    results[2] = NG;
    results[3] = NG;
    results[1] = NG;
    LATBbits.LATB0 = 1;
    LCD_Init();
    usart_init();
    LCD_String("start\n");
    while(1){
        
       /* if(count_check() == OK){
            LATBbits.LATB3 = 1;
            LCD_String("OK");
            
        } else{
            LATBbits.LATB3 = 0;
            LCD_String("NG");
        }*/
        __delay_ms(1000);
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
       /*for(int i = 0;i < 10;i++){
        if(results[i] != OK){
            switch(ng_count){
                case 0 : LCD_String(ic_names[i]);
                    LCD_String("\n"); break;
                case 1 : LCD_String(ic_names[i]); break;
                case 2 : LCD_Locate(1,15); LCD_Character(0x7e); break;
                default : break;
            }
            ng_count++;
        }
    }
    */
    
    return;
}


void __interrupt ( ) isr (void){
    if(PIR1bits.RCIF != 0){
        PIR1bits.RCIF = 0;
        LCD_Number(RCREG);
    } else if(PIR1bits.TXIF != 0){
        
    }
    //PIE2bits.CMIE; = 
    //CMCONbits.C1OUT;
    //CMCONbits.C2OUT;
}
