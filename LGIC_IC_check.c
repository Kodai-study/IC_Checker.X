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

#define Q_PORT PORTA
#define Q_LAT  LATA
#define Q_TRIS TRISA

/* 74393の出力Q0?3を0?3番ポートに、2つ目のQ0?3を4?8番ポートに接続 */

#define DFF_CL1 LAT(A,0)
#define DFF_PR1 LAT(A,1)
#define DFF_Q1  P(A,2)
#define DFF_nQ1 P(A,3)
#define DFF_CL2 LAT(A,4)
#define DFF_PR2 LAT(A,5)
#define DFF_Q2  P(A,6)
#define DFF_nQ2 P(A,7)

#define DFF_COMD LATAbits.LA4
#define DFF_COMCLR LATAbits.LA6



#define CO_CLR DFF_COMD
#define CO_CLK DFF_COMCLR

#define DFF_CHECK(b) \
    if(DFF_Q1 != b || DFF_nQ1 != (!b) || DFF_Q2 != b || DFF_nQ2 != (!b))  {return NG;}

extern unsigned char bitPattern[8]; //添え字の数のビットのみが1になったデータ配列
/*
 * D-フリップフロップのチェック
 * クリア、プリセット、Dに0入力→1入力  で出力が
 * 0,1,0,1 の順に変化することを確かめる
 * 
 */
CHECK_RESULT dff_Check(){
        TRISA = 0x00;
        LATA = 0x00;  //CLR.PRCを1(off)に
        DFF_PR1 = 1;
        DFF_PR2 = 1;
        DFF_COMCLR = 1;
        DOWN_CLOCK(DFF_PR1);        //プリセット端子を入れてHで初期化
        DOWN_CLOCK(DFF_PR2);
        DFF_CHECK(1)
        
        downClock(A, 0);       //クリア端子を0に
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
                
        downClock(A, 2);       //プリセット端子を0に
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
                
        /* D端子を0にしてクロック */
        DFF_COMD = 0;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
        
        /* D端子を1にしてクロック */
        DFF_COMD = 1;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
        
        return OK;
}


CHECK_RESULT count_check(){
    CLOCK(CO_CLR)
    for(int i = 0;i < 15;i++){
        if(num_check(i) == 0){
            return NG;
        }
        CLOCK(CO_CLK)
    }
    if(num_check(0) != 0){
        return OK;
    }
}

int num_check(int value){
    
    if(value != (Q_PORT & 0x0f)){
        return 0;
    }
    if(value != (Q_PORT & 0xf0) >> 4){
        return 0;
    }
    
    return 1;
}