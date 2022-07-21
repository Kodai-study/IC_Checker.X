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
#define IC3_TRIS TRISB
#define IC3_LAT LATB
#define IC3_PORT PORTB
#define NAND_INPUT 0x0f     //NANDのチェックで、ICへの入力に割り当てられるビット
#define NAND_OUTPUT 0xf0    //NANDのチェックで、ICのの出力の読み取りに割り当てられるビット
#define NOR_INPUT NAND_OUTPUT   //NORの入力、出力端子は、NANDの端子設定と逆
#define NOR_OUTPUT NAND_INPUT   

CHECK_RESULT nand_check(int mode){
    
    const int result[2][4] =            //NAND,NORの真理値表の結果
    { {1, 1, 1, 0}, {1, 0, 0, 0} };     //[0]がNAND、[1]がNOR 
    unsigned char mask;                 //ICからの出力を読み取るビット
    COM_INPUT_TRIS = 0;      
    
    /* チェックするICに合わせて、ポートの出力、入力の設定 */
    if(mode){
        IC3_TRIS = NAND_OUTPUT;
        mask = NAND_OUTPUT;
    } else {
        IC3_TRIS = NOR_OUTPUT;
        mask = NOR_OUTPUT;
    }
    __delay_ms(PLUS_TIME);
    
    /* 真理値表の0?3を入力して出力を確認 */
    for(int i = 0;i <= 3; i++){
        LATCbits.LATC0 = i % 2;                 //入力の1の桁は共通の入力部分に出力
        IC3_LAT = (i & 0x02) == 2 ? ~mask : 0;  //入力の2の桁はそれぞれの入力端子に出力
        __delay_ms(WAIT_TIME);
        unsigned char data = IC3_PORT & mask;   
        if(result[mode][i] == 0 && data != 0){  //真理値表の結果が0の時は、出力がすべて0でなければいけない
            return NG;
        } else if(result[mode][i] == 1 && data != mask){    //真理値表の結果が1
            return NG;
        }
    }
    return OK;
}
