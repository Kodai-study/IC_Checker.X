/*
 * File:   functions.c
 * Author: gorin
 *
 * Created on 2022/07/19, 18:04
 */


#include <xc.h>
#include "datas.h"


void clock(PORT portnumber,int bitNum){
    unsigned char clockBit = (1 << bitNum);
    switch(portnumber){
        case A : LATA |= clockBit; __delay_ms(PLUS_TIME); LATA &= ~clockBit; break;
        case B : LATB |= clockBit; __delay_ms(PLUS_TIME); LATB &= ~clockBit; break;
        case C : LATC |= clockBit; __delay_ms(PLUS_TIME); LATC &= ~clockBit; break;
        case D : LATD |= clockBit; __delay_ms(PLUS_TIME); LATD &= ~clockBit; break;
        case E : LATE |= clockBit; __delay_ms(PLUS_TIME); LATE &= ~clockBit; break;
    }
}

