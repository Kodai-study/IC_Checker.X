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

#define Q_PORT PORTD
#define Q_LAT  LATD
#define Q_TRIS TRISD

/* 74393の出力Q0?3を0?3番ポートに、2つ目のQ0?3を4?8番ポートに接続 */

#define DFF_CL1 LAT(D,0)
#define DFF_PR1 LAT(D,1)
#define DFF_Q1  P(D,2)
#define DFF_nQ1 P(D,3)
#define DFF_nQ2 P(D,4)
#define DFF_Q2  P(D,5)
#define DFF_PR2 LAT(D,6)
#define DFF_CL2 LAT(D,7)



#define DFF_COMCLR LAT(C,0)
#define DFF_COMD   LAT(C,1)

#define COMCLR_TRIS TRIS(C,0)
#define COMD_TRIS TRIS(C,1)

#define CO_CLR DFF_COMD
#define CO_CLK DFF_COMCLR
#define CO_CLR_TRIS COMD_TRIS
#define CO_CLK_TRIS COMCLR_TRIS

#define DFF_CHECK(b) \
    if(DFF_Q1 != b || DFF_nQ1 != (!b) || DFF_Q2 != b || DFF_nQ2 != (!b))  {return NG;}

extern unsigned char bitPattern[8]; //添え字の数のビットのみが1になったデータ配列
/*
 * D-フリップフロップのチェック
 * クリア、プリセット、Dに0入力→1入力  で出力が
 * 0,1,0,1 の順に変化することを確かめる
 * 
 */
CHECK_RESULT dff_Check(int mode){
        LCD_Clear();
        Q_TRIS = 0b00111100;
        COMD_TRIS = 0;
        COMCLR_TRIS = 0;
        LATB = 0;
        Q_LAT = 0x00;  //CLR.PRCを1(off)に
        DFF_PR1 = 1;
        DFF_PR2 = 1;
        DFF_COMCLR = 1;
        DOWN_CLOCK(DFF_PR1);        //プリセット端子を入れてHで初期化
        DOWN_CLOCK(DFF_PR2);
        
        LCD_Number(0);
        DFF_CHECK(1)
        
        DOWN_CLOCK(DFF_COMCLR);       //クリア端子を0に
        __delay_ms(WAIT_TIME);
        LCD_Number(1);
        DFF_CHECK(0)
                
        DOWN_CLOCK(DFF_PR1);       //プリセット端子を0に
        DOWN_CLOCK(DFF_PR2);
        LCD_Number(2);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
                
        /* D端子を0にしてクロック */
        DFF_COMD = 0;
        __delay_ms(WAIT_TIME);
        CLOCK(DFF_CL1)
        CLOCK(DFF_CL2)
        LCD_Number(3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
        
        /* D端子を1にしてクロック */
        DFF_COMD = 1;
        __delay_ms(WAIT_TIME);
        CLOCK(DFF_CL1);
        CLOCK(DFF_CL2);
        LCD_Number(4);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
        
        return OK;
}

int num_check(int value);

CHECK_RESULT count_check(int mode){
    LCD_Clear();
    Q_TRIS = 0xff;  
    CO_CLK_TRIS = 0;
    CO_CLR_TRIS = 0;
    CO_CLR = 0;
    CO_CLK = 0;
    
    CLOCK(CO_CLR)           //クリア端子を1にして出力をクリア
    /* 1?15まで値をカウントアップさせ、出力をチェック */
    for(int i = 0;i < 16;i++){
        if(num_check(i) == 0){
            return NG;
        }
        CLOCK(CO_CLK)
    }
    if(num_check(0) != 0){      //16回目のカウントアップで、0になることをチェック
        return OK;
    }
}

/* カウントICの出力が引数と等しいかをチェック */
int num_check(int value){
    LCD_Clear();
    if(value != (Q_PORT & 0x0f)){
        return 0;
    } 
    unsigned char num = P(D,7) ? 1 : 0;
    if(P(D,6)) num |= 2;
    if(P(D,5)) num |= 4;
    if(P(D,4)) num |= 8;
    if(value != num){
        return 0;
    }
    LCD_Number(Q_PORT);
    __delay_ms(500);
    return 1;
}