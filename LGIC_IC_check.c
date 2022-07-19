/*
 * File:   d-ff.c
 * Author: gorin
 *
 * Created on 2022/07/15, 19:22
 */


/** TODO?????????????? */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"

//#define TM_OUT LATBbits.LATB0
#define TM_OUT PORTBbits.RB0
#define TM_SELECT LATBbits.LATB1

extern unsigned char bitPattern[8];
/*
 *?D-????????
 * 
 * 
 * 
 */
void dff_Check(){
        LATA = 0b00000101;  //??????????
    /* ???????????? */
        LATAbits.LA0 = 0;   //?????
        __delay_ms(PLUS_TIME);
        LATAbits.LA0 = 1;
        __delay_ms(WAIT_TIME);
        
        LATAbits.LA2 = 0;   //???????
        __delay_ms(PLUS_TIME);
        LATAbits.LA2 = 1;
        __delay_ms(WAIT_TIME);
        
        /*?1????????????D-0??????*/
        LATAbits.LA1 = 0;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        
        
        /*?1????????????D=1??????*/
        LATAbits.LA1 = 1;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        

}

int TMchecker(){
    int old = TM_OUT;
    const int cycle = (int)((1 / 12.025) * 1000);
    while(1){
        if(old != TM_OUT){
            if(old == 0){
                break;
            }
            old = TM_OUT;
        }
        __delay_ms(1);
    }
    
    int onCount = 0;
    int offCount = 0;
    for(int i = 0;i < 4;i++){
        while(TM_OUT != 0){
            onCount++;
            __delay_ms(1);
        }
        while(TM_OUT == 0){
            offCount++;
            __delay_ms(1);
        }
        int cycleCount = onCount + offCount;
        if(cycleCount <= cycle * 1.05 ||
           cycleCount >= cycle * 0.95 ||
           (double)onCount / cycleCount >= 0.85 ||
           (double)onCount / cycleCount <= 0.95 ||
           (double)offCount / cycleCount >= 0.05 ||
           (double)offCount / cycleCount >= 0.15)
            return -1;
    }
    return 1;
}
    
int nand_check(){
    TRISC = 0b10101010;
    const unsigned char outBit = 0b00011001;
    const unsigned char readBit = 0b11010100;
    LATC &= ~outBit;
    __delay_ms(PLUS_TIME);
    unsigned char data = PORTC & readBit;
}


void portInit(){
    
}
