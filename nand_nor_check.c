/*
 * File:   nand_nor_check.c
 * Author: kouda
 *
 * Created on 2022/07/20, 23:01
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"

#define COM_INPUT       LAT(C,5)
#define COM_INPUT_TRIS  TRIS(C,5)
#define IC3_TRIS TRISD
#define IC3_LAT LATD
#define IC3_PORT PORTD
#define NAND_INPUT 0x0f     //NANDのチェックで、ICへの入力に割り当てられるビット
#define NAND_OUTPUT 0xf0    //NANDのチェックで、ICのの出力の読み取りに割り当てられるビット
#define NOR_INPUT NAND_OUTPUT   //NORの入力、出力端子は、NANDの端子設定と逆
#define NOR_OUTPUT NAND_INPUT   

CHECK_RESULT nand_check(int mode){
    
    const int result[4] = {1, 1, 1, 0}; //NANDの真理値表の結果
        
    COM_INPUT_TRIS = 0;

    /* モードによって、出力、入力を割り当てる */
        IC3_TRIS = NAND_OUTPUT;
        
    /* 入力を00?11まで切り替え、出力が真理値表通りであることを確かめる */
    for(int i = 0;i <= 3; i++){
        LCD_Number(i);
        COM_INPUT = i % 2;                 //入力の1桁目の値を共通部分に出力
        IC3_LAT = (i & 0x02) != 0 ? NAND_INPUT : 0;  //2桁目は、それぞれの入力部に出力
        __delay_ms(WAIT_TIME);

        unsigned char data = IC3_PORT & NAND_OUTPUT;
        LCD_HNumber(IC3_PORT,2);
        if(result[i] == 0 && data != 0){  //出力の真理値が0の時に、出力が0ならOK
            return NG;
        } else if(result[i] == 1 && data != NAND_OUTPUT){    //真理値1の時、出力が1ならOK
            return NG;
        }
    }
    return OK;      //0?3で間違った出力が1つもなかったらOK
}


CHECK_RESULT nor_check(int mode){
   
    const int result[4] =  {1, 0, 0, 0};  //NAND,NORの真理値表の結果
       
    COM_INPUT_TRIS = 0;

    /* モードによって、出力、入力を割り当てる */
    IC3_TRIS = NOR_OUTPUT;
        
    /* 入力を00?11まで切り替え、出力が真理値表通りであることを確かめる */
    for(int i = 0;i <= 3; i++){
        LCD_Number(i);
        COM_INPUT = i % 2;                 //入力の1桁目の値を共通部分に出力
        IC3_LAT = (i & 0x02) != 0 ? NOR_INPUT : 0;  //2桁目は、それぞれの入力部に出力
        __delay_ms(WAIT_TIME);
        LCD_HNumber(IC3_PORT,2);
        unsigned char data = IC3_PORT & NOR_OUTPUT;
        if(result[i] == 0 && data != 0){  //出力の真理値が0の時に、出力が全て0ならOK
            return NG;
        } else if(result[i] == 1 && data != NOR_OUTPUT){    //真理値1の時、出力が全て1ならOK
            return NG;
        }
    }
    return OK;      //0?3で間違った出力が1つもなかったらOK
}