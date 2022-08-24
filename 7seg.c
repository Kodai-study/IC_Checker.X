/*
 * File:   7seg.c
 * Author: gorin
 *
 * Created on 2022/08/17, 15:30
 */
#include "datas.h"

#define RCL   LAT(E,0)
#define INPUT LAT(E,1)
#define CLK   LAT(E,2)

#define RCL_TRIS   TRIS(E,0)
#define INPUT_TRIS TRIS(E,1)
#define CLK_TRIS   TRIS(E,2)

CHECK_RESULT seg7_check(int mode){
    
    ADCON1bits.PCFG = 0b1100;       //AN0~3がアナログ入力
    ADCON0bits.ADON = 1;        //A/Dコンバータを有効
    ADCON2bits.ADFM = 0;        //右詰めで、上位8ビットのみを使う
    ADCON2bits.ACQT = 0;        //GOを1にしたときに変換開始
    
    RCL_TRIS = 0;
    INPUT_TRIS = 0;
    CLK_TRIS = 0;
    INPUT = 0;
    for(int i = 0;i < 8;i++){
        CLOCK(CLK);
    }
    CLOCK(RCL);
    convert(0);
    if(ADRESH >= 100){
        return NG;
    }
    INPUT = 1;
    CLOCK(CLK);
    CLOCK(RCL);
    if(ADRESH <= 100 || ADRESH >= 200){
        return NG;
    }
    INPUT = 0;
    for(int i = 0;i < 7;i++){
        CLOCK(CLK);
        CLOCK(RCL);
        convert(0);
        if(ADRESH <= 100 || ADRESH >= 200){
            return NG;
        }
    }
    return OK;
}
        

