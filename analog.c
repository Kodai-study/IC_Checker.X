/*
 * File:   analog.c
 * Author: kouda
 *
 * Created on 2022/08/15, 21:46
 */

#include "datas.h"
#include <xc.h>

#define R_CHENGE LATAbits.LATA6
#define CHENGE_TRIS TRISAbits.TRISA6

#define REG_POWER LATAbits.LA3


void convert(int an_chan);

CHECK_RESULT opamp_check(int mode){
    ADCON1bits.PCFG = 0b1100;       //AN0~3がアナログ入力
    CHENGE_TRIS = 0;
    R_CHENGE = 0;
    ADCON0bits.ADON = 1;        //A/Dコンバータを有効
    ADCON2bits.ADFM = 0;        //右詰めで、上位8ビットのみを使う
    ADCON2bits.ACQT = 0;        //GOを1にしたときに変換開始
    /* 変換開始、終了まで待つ */
    convert(1);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    convert(2);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    
    R_CHENGE = 1;
    convert(1);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    convert(2);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    R_CHENGE = 0;
    return OK;
}

CHECK_RESULT reg_check(int mode){
    
}


void convert(int an_chan){
    ADCON0bits.CHS = an_chan;
    __delay_ms(1);
    /* 変換開始、終了まで待つ */
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO) {;}
}