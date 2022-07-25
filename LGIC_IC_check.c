/*
 * File:   IC_check.c
 * Author: gorin
 *
 * Created on 2022/07/15, 19:22
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"
#include "lcdlib_xc8_v03.h"



#define DFF_Q1 PORTAbits.RA0
#define DFF_nQ1 PORTAbits.RA1
#define DFF_Q2 PORTAbits.RA2
#define DFF_nQ2 PORTAbits.RA3
#define DFF_CHECK(b) \
    if(DFF_Q1 != b || DFF_nQ1 != (!b) || DFF_Q2 != b || DFF_nQ2 != (!b))  return NG;
#define DFF_D1 LATAbits.LA4
#define DFF_D2 LATAbits.LA5
#define DFF_CLR LATAbits.LA6
#define DRR_PRE LATAbits.LA7

#define CLOCK(a) a = 1; __delay_ms(PLUS_TIME); a = 0;

extern unsigned char bitPattern[8]; //添え字の数のビットのみが1になったデータ配列
/*
 * D-フリップフロップのチェック
 * クリア、プリセット、Dに0入力→1入力  で出力が
 * 0,1,0,1 の順に変化することを確かめる
 * 
 */
CHECK_RESULT dff_Check(){
        TRISA = 0x00;
        LATA = 0b00000101;  //CLR.PRCを1(off)に
        downClock(A, 2);        //プリセット端子を入れてHで初期化
        DFF_CHECK(1)
        CLOCK(DFF_CLR)
        
        downClock(A, 0);       //クリア端子を0に
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
                
        downClock(A, 2);       //プリセット端子を0に
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
                
        /* D端子を0にしてクロック */
        DFF_D1 = 0;
        DFF_D2 = 0;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
        
        /* D端子を1にしてクロック */
        DFF_D1 = 1;
        DFF_D2 = 1;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
        
        return OK;
}

