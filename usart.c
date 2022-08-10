/*
 * File:   usart.c
 * Author: gorin
 *
 * Created on 2022/08/10, 9:43
 */
#include <xc.h>

void usart_init(){
    TRISC = 0x80;
    TXSTA = 0b00100100;
    RCSTA  = 0b10010000 ;    
    SPBRG = 5;
    SPBRGH = 5;
    //PIE1bits.TXIE = 1;
    PIE1bits.RCIE = 1;
}

void hoge(){
    
    
}
