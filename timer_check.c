/*
 * File:   timer_check.c
 * Author: gorin
 *
 * Created on 2022/07/25, 20:13
 */


#include <xc.h>
#include <stdio.h>
#include "datas.h"

#define TM_OUT P(B,3)
#define TM_OUT_TRIS TRIS(B,3)
#define TM_SELECT LAT(B,4)
#define TM_SL_TRIS TRIS(B,4)


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
const int cycle[] = {(int)(1000.0 / 12.025), (int)(1000.0 / 49.58),};   //12.25Hzの時の周期(ms)
const double on_time[] = {0.916, 0.656};
int cycleCount = 0;

CHECK_RESULT TMchecker(int mode){
    
    ADCON1 = 0xff;
    TM_OUT_TRIS = 1; 
    TM_SL_TRIS = 0;
    __delay_ms(100);
    LCD_Number(t0_flg);
    LCD_String("ﾀｲﾏｶｲｼ\n");
    /* 出力が1に切り替わった瞬間から測定を開始。それまで待つ */
    int old = TM_OUT;
    int new = old;
    
    for(int i = 0;i <= 1;i++){
        TM_SELECT = i;
        while((new = TM_OUT) == 0 || old != 0){
            old = new;
        }

        /* 4周期周波数とデューティー比を測る */
        for(int j = 0;j < 4;j++){
            LCD_Clear();
            onCount = 1;
            offCount = 0;
            while(TM_OUT != 0){
                onCount++;
                T0_WAIT;
            }
            onCount++;
            while(TM_OUT == 0){
                offCount++;
                T0_WAIT;
            }
            offCount++;
            cycleCount = onCount + offCount;    //1周期の時間(ms)

            // 周期、デューティー比が既定の±20%以上離れていたら失敗とする 
           if(cycleCount >= (int)(cycle[i] * 1.2) ||
               cycleCount <= (int)(cycle[i] * 0.8) || //周期が理論値±20%
               ((double)onCount / cycleCount) >= on_time[i] * 1.2  ||  //オン時間が±20%
               ((double)onCount / cycleCount) <= on_time[i] * 0.8 )
           {
                return NG;
           }
        }
        viewResults();
        __delay_ms(500);
    }
    return OK;  //4周期測定して、周波数とデューティー比が範囲内ならOK
}

void viewResults(){
    char st[16];
    sprintf(st,"%d:%d\n%.2f:%.2f",
            cycleCount,cycle[0],((double)onCount / cycleCount),on_time[0] );

    LCD_String(st);

}
