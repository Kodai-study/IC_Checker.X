/*
 * File:   IC_check.c
 * Author: gorin
 *
 * Created on 2022/07/15, 19:22
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"

//#define TM_OUT LATBbits.LATB0
#define TM_OUT PORTBbits.RB0
#define TM_SELECT LATBbits.LATB1
#define DFF_OUT1 PORTAbits.RA0
#define DFF_OUT2 PORTAbits.RA1
#define D_FF_CHECK(b) if(DFF_OUT1 != b || DFF_OUT2 != b) return NG;

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
        D_FF_CHECK(1)
                
        downClock(A, 0);       //クリア端子を0に
        __delay_ms(WAIT_TIME);
        D_FF_CHECK(0)
                
        downClock(A, 2);       //プリセット端子を0に
        __delay_ms(WAIT_TIME);
        D_FF_CHECK(1)
                
        /* D端子を0にしてクロック */
        LATAbits.LA1 = 0;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        D_FF_CHECK(0)
        
        /* D端子を1にしてクロック */
        LATAbits.LA1 = 1;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        D_FF_CHECK(1)
        
        return OK;
}

/**
 * 
 * LM555 タイマICのチェック
 * 抵抗値を切り替えて、次の周波数、デューティー比を確認
 * 100kΩ : 12.02Hz, 91.6%
 * 9.1kΩ : 49.58Hz、65.6%
 * 
 */
CHECK_RESULT TMchecker(){
    /* 出力が1に切り替わった瞬間から測定を開始。それまで待つ */
    int old = TM_OUT;
    int new = old;
    const int cycle = (int)(1000.0 / 12.025);   //12.25Hzの時の周期(ms)
    while(old != (new = TM_OUT) && old == 0){
        if(old != new){
            old = new;
        }
        __delay_ms(1);
    }
    
    int onCount;    //出力が0nだった回数=時間(ms)をカウント
    int offCount;   
    /* 4周期周波数とデューティー比を測る */
    for(int i = 0,onCount = 0,offCount = 0;i < 4;i++){
        while(TM_OUT != 0){
            onCount++;
            __delay_ms(1);
        }
        while(TM_OUT == 0){
            offCount++;
            __delay_ms(1);
        }
        int cycleCount = onCount + offCount;    //1周期の時間(ms)
        
        /* 周期、デューティー比が既定の±5%以上離れていたら失敗とする */
        if(cycleCount <= (int)(cycle * 1.05) ||
           cycleCount >= (int)(cycle * 0.95) || //周期が理論値±5%
           (double)onCount / cycleCount >= 0.85 ||  //オン時間が±5%
           (double)onCount / cycleCount <= 0.95 ||
           (double)offCount / cycleCount >= 0.05 || //オフ時間が±5%
           (double)offCount / cycleCount >= 0.15)
            return NG;
    }
    return OK;  //4周期測定して、周波数とデューティー比が範囲内ならOK
}
    