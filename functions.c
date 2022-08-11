/*
 * File:   functions.c
 * Author: gorin
 *
 * Created on 2022/07/19, 18:04
 */


#include <xc.h>
#include "datas.h"

extern const int ic_kinds;
int is_info[] = {1,1,0,0,0,0,1,0,0,1};

//#define SW1 PORTAbits.RA0

//#define SW2 PORTAbits.RA0


/* ポート番号、ビット数を入れると、その端子に立ち上がりクロックを出力する */
/*
void clock(PORT_NUM portnumber,int bitNum){
    switch(portnumber){
        case A : LATA |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATA &= ~bitPattern[bitNum]; break;
        case B : LATB |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATB &= ~bitPattern[bitNum]; break;
        case C : LATC |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATC &= ~bitPattern[bitNum]; break;
        case D : LATD |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATD &= ~bitPattern[bitNum]; break;
        case E : LATE |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATE &= ~bitPattern[bitNum]; break;
    }
}
*/
/* ポート番号、ビット数を入て、その端子に立ち下がりクロックを出力する */
/*
void downClock(PORT_NUM portnumber,int bitNum){
    switch(portnumber){
        case A : LATA &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATA |= bitPattern[bitNum]; break;
        case B : LATB &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATB |= bitPattern[bitNum]; break;
        case C : LATC &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATC |= bitPattern[bitNum]; break;
        case D : LATD &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATD |= bitPattern[bitNum]; break;
        case E : LATE &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATE |= bitPattern[bitNum]; break;
    }
}*/

/* スイッチ1，2を監視して、新たに押されたらフラグを立てる */
void sw_check(){
    static int count1 = 0;
    static int count2 = 0;
    static _Bool old1 = 0;
    static _Bool old2 = 0;
    static _Bool state1 = 0;
    static _Bool state2 = 0;
    
    if(SW1 == old1){
        if(++count1 >= 5){
            if(SW1 == 1 && state1 == 0){
                sw1_flg = 1;
            }
            state1 = SW1;
        } else {
            count1 = 0;
        }
    }
    if(SW2 == old2){
        if(++count2 >= 5){
            if(SW2 == 1 && state2 == 0){
                sw2_flg = 1;
            }
            state2 = SW2;
        } else {
            count2 = 0;
        }
    }
    old1 = SW1;
    old2 = SW2;
}

/* 引数で受け取った種類のICを単体チェックする。 */
void single_check(int kind){
    switch(kind){
        case 1 : if(dff_Check() == OK) LCD_String("OK");  break;
        case 2 : 
        default : LCD_String("ERROR"); return;
    }
    while(1){
        T0_WAIT;
        sw_check();
        if(is_info[kind] != 0){
            if(sw1_flg != 0){
                sw1_flg = 0;
                
            } else if(sw2_flg != 0){
                sw2_flg = 0;
                mode = 0;
                return;
            }
        } else if(sw1_flg != 0 || sw2_flg != 0){
            mode = 0;
            return;
        }
    }
}

void current_over(){
    LCD_Clear();
    LCD_String("ｼｮｰﾄｼﾃｲﾏｽ ｾﾂｿﾞｸｦｶｸﾆﾝｼﾃｸﾀﾞｻｲ");
    int count = 0;
    while(1){
        T0_WAIT;
        if(++count >= 250){
            //LED_RED = !LED_RED;
            count = 0;
        }
    }
}
