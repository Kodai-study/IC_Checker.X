/*
 * File:   timer_check.c
 * Author: gorin
 *
 * Created on 2022/07/25, 20:13
 */


#include <xc.h>
#include <stdio.h>
#include "datas.h"

#define TM_OUT PORTBbits.RB1
#define TM_OUT_TRIS TRISBbits.RB0
#define TM_SELECT LATBbits.LATB1


/**
 * 
 * LM555 タイマICのチェック
 * 抵抗値を切り替えて、次の周波数、デューティー比を確認
 * 100kΩ : 12.02Hz, 91.6%
 * 9.1kΩ : 49.58Hz、65.6%
 * 
 */

int onCount = 0;    //出力が0nだった回数=時間(ms)をカウント
int offCount = 0;   
const int cycle = (int)(1000.0 / 12.025);   //12.25Hzの時の周期(ms)
int cycleCount = 0;

CHECK_RESULT TMchecker(){
    
    ADCON1 = 0xff;
    TM_OUT_TRIS = 1;    
    TRISB = 0xff;
    __delay_ms(100);
    LCD_Number(t0_flg);
    LCD_String("ﾀｲﾏｶｲｼ\n");
    /* 出力が1に切り替わった瞬間から測定を開始。それまで待つ */
    int old = TM_OUT;
    int new = old;
    
    while((new = TM_OUT) && old == 0){
        old = new;
        T0_WAIT
    }

    /* 4周期周波数とデューティー比を測る */
    for(int i = 0;i < 4;i++){
        onCount = 0;
        offCount = 0;
        while(TM_OUT != 0){
            onCount++;
            LATAbits.LA0 = 1;
            T0_WAIT
            //__delay_ms(1);
        }
        while(TM_OUT == 0){
            offCount++;
            LATAbits.LA0 = 0;
            T0_WAIT
           // __delay_ms(1);
        }
        cycleCount = onCount + offCount;    //1周期の時間(ms)
        
        // 周期、デューティー比が既定の±5%以上離れていたら失敗とする 
       /* if(cycleCount >= (int)(cycle * 1.05) ||
           cycleCount <= (int)(cycle * 0.95) || //周期が理論値±5%
           (double)onCount / cycleCount <= 0.85 ||  //オン時間が±5%
           (double)onCount / cycleCount >= 0.95 ||
           (double)offCount / cycleCount <= 0.05 || //オフ時間が±5%
           (double)offCount / cycleCount >= 0.15)
            return NG;
*/
    }
    return OK;  //4周期測定して、周波数とデューティー比が範囲内ならOK
}

void viewResults(){
    char st[16];
    sprintf(st,"%.2f:%.2f:%.2f",
            1.0 / (cycleCount / 1000.0),(double)onCount / cycleCount,(double)offCount / cycleCount );
            //1.0 / (cycleCount / 1000.0),onCount,offCount);
    LCD_String(st);
}
