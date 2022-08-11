/*
 * File:   nand_nor_check.c
 * Author: kouda
 *
 * Created on 2022/07/20, 23:01
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"

#define COM_INPUT LATBbits.LB0
#define COM_INPUT_TRIS TRIS(B,0)
#define IC3_TRIS TRISA
#define IC3_LAT LATA
#define IC3_PORT PORTA
#define NAND_INPUT 0x0f     //NANDのチェックで、ICへの入力に割り当てられるビット
#define NAND_OUTPUT 0xf0    //NANDのチェックで、ICのの出力の読み取りに割り当てられるビット
#define NOR_INPUT NAND_OUTPUT   //NORの入力、出力端子は、NANDの端子設定と逆
#define NOR_OUTPUT NAND_INPUT   

CHECK_RESULT nand_check(int mode){
    
    const int result[2][4] =            //NAND,NORの真理値表の結果
    { {1, 1, 1, 0}, {1, 0, 0, 0} };     //[0]がNAND、[1]がNOR
    unsigned char mask;
    COM_INPUT_TRIS = 0;

    /* モードによって、出力、入力を割り当てる */
    if(mode){
        IC3_TRIS = NAND_OUTPUT;
        mask = NAND_OUTPUT;
    } else {
        IC3_TRIS = NOR_OUTPUT;
        mask = NOR_OUTPUT;
    }
    
    /* 入力を00?11まで切り替え、出力が真理値表通りであることを確かめる */
    for(int i = 0;i <= 3; i++){
        COM_INPUT = i % 2;                 //入力の1桁目の値を共通部分に出力
        IC3_LAT = (i & 0x02) != 0 ? ~mask : 0;  //2桁目は、それぞれの入力部に出力
        __delay_ms(WAIT_TIME);

        unsigned char data = IC3_PORT & mask;
        if(result[mode][i] == 0 && data != 0){  //出力の真理値が0の時に、出力が0ならOK
            return NG;
        } else if(result[mode][i] == 1 && data != mask){    //真理値1の時、出力が1ならOK
            return NG;
        }
    }
    return OK;      //0?3で間違った出力が1つもなかったらOK
}
