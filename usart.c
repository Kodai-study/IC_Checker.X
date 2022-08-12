/*
 * File:   usart.c
 * Author: gorin
 *
 * Created on 2022/08/10, 9:43
 */
#include <xc.h>
#include "datas.h"

void usart_init(){
    TRISCbits.TRISC7 = 1;   //C_7ビットはRXのインプット
    TRISCbits.TRISC6 = 0;   //TX
    TXSTA = 0b00100100;     //非同期、8ビットで送信許可
    RCSTA  = 0b10010000 ;    //TX,RXを通信ピンに割り当て
    SPBRG = 5;              //通信速度を9600Hzに設定 (4000000 / (64*5))
    SPBRGH = 5;
    //PIE1bits.TXIE = 1;
    PIE1bits.RCIE = 1;
}

void req_check(int kind){
    
    TXREG = 0x01;
    while(1){;}
    
    
}

void echo(){
    
}