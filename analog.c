/*
 * File:   analog.c
 * Author: kouda
 *
 * Created on 2022/08/15, 21:46
 */

#include "datas.h"
#include <xc.h>

//#define R_CHENGE LAT(A,6)
//#define CHENGE_TRIS TRIS(A,6)
#define R_CHENGE    LAT(B,5)
#define CHENGE_TRIS TRIS(B,5)

#define MODE      LAT(A,6)
#define MODE_TRIS TRIS(A,6)



void convert(int an_chan);
//3.5V : 0.5V   25.6  179

CHECK_RESULT opamp_check(int mode){
    
    const int volt_05 = 25;
    const int volt_35 = 179;
    const int width = 20;
    LCD_Clear();
    ADCON1bits.PCFG = 0b1100;       //AN0~3がアナログ入力
    CHENGE_TRIS = 0;
    R_CHENGE = 0;
    MODE_TRIS = 0;
    MODE = 0;
    ADCON0bits.ADON = 1;        //A/Dコンバータを有効
    ADCON2bits.ADFM = 0;        //右詰めで、上位8ビットのみを使う
    ADCON2bits.ACQT = 0;        //GOを1にしたときに変換開始
    /* 変換開始、終了まで待つ */
    convert(1);
    if(ADRESH < volt_35 - width || ADRESH > volt_35 + width ){
        LCD_Number(0);
        LCD_Number(ADRESH);
        return NG;
    }
    convert(2);
    if(ADRESH < volt_35 - width || ADRESH > volt_35 + width){
        LCD_Number(1);
        LCD_Number(ADRESH);
        return NG;
    }
    
    R_CHENGE = 1;
    __delay_ms(1000);
    convert(1);
    if(ADRESH < volt_05 - width || ADRESH > volt_05 + width ){
        LCD_Number(2);
        LCD_Number(ADRESH);
        return NG;
    }
    convert(2);
    if(ADRESH < volt_05 - width || ADRESH > volt_05 + width ){
        LCD_Number(3);
        LCD_Number(ADRESH);
        return NG;
    }
    R_CHENGE = 0;
    return OK;
}




CHECK_RESULT com_check(int mode){
    ADCON1bits.PCFG = 0b1100;       //AN0~3がアナログ入力
    CHENGE_TRIS = 0;
    R_CHENGE = 0;
    MODE_TRIS = 0;
    MODE = 1;
    ADCON0bits.ADON = 1;        //A/Dコンバータを有効
    ADCON2bits.ADFM = 0;        //右詰めで、上位8ビットのみを使う
    ADCON2bits.ACQT = 0;        //GOを1にしたときに変換開始
    /* 変換開始、終了まで待つ */
    convert(1);
    if(ADRESH < 250){
        LCD_Number(ADRESH);
        return NG;
    }
    convert(2);
    if(ADRESH < 250){
        LCD_Number(ADRESH);
        return NG;
    }
    
    R_CHENGE = 1;
    __delay_ms(1000);
    convert(1);
    if(ADRESH > 10){
        LCD_Number(ADRESH);
        return NG;
    }
    convert(2);
    if(ADRESH > 10){
        LCD_Number(ADRESH);
        return NG;
    }
    R_CHENGE = 0;
    return OK;
}


void convert(int an_chan){
    ADCON0bits.CHS = an_chan;
    __delay_ms(1);
    /* 変換開始、終了まで待つ */
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO) {;}
}