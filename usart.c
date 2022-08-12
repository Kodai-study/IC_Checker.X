/*
 * File:   usart.c
 * Author: gorin
 *
 * Created on 2022/08/10, 9:43
 */
#include <xc.h>
#include "datas.h"

void usart_init(){
    TRISCbits.TRISC7 = 1;   //C_7�r�b�g��RX�̃C���v�b�g
    TRISCbits.TRISC6 = 0;   //TX
    TXSTA = 0b00100100;     //�񓯊��A8�r�b�g�ő��M����
    RCSTA  = 0b10010000 ;    //TX,RX��ʐM�s���Ɋ��蓖��
    SPBRG = 5;              //�ʐM���x��9600Hz�ɐݒ� (4000000 / (64*5))
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