/*
 * File:   d-ff.c
 * Author: gorin
 *
 * Created on 2022/07/15, 19:22
 */


/** TODO?????????????? */


#include <xc.h>
#define _XTAL_FREQ 1000000

#define CLOCK(x) x = 0; __delay_ms(5); x = 1;

void dff_Check(){
    /* ???????????? */
        LATAbits.LA0 = 0;   //?????
        __delay_ms(5);
        LATAbits.LA0 = 1;
        
        __delay_ms(500);
        
        LATAbits.LA3 = 0;   //???????
        __delay_ms(5);
        LATAbits.LA3 = 1;
        __delay_ms(1000);
        
        /*?1????????????D-0??????*/
        LATAbits.LA1 = 0;
        LATAbits.LA4 = 1;
        __delay_ms(5);
        LATAbits.LA4 = 0;
        __delay_ms(500);
        
        /*?1????????????D=1??????*/
        LATAbits.LA1 = 1;
        LATAbits.LA4 = 1;   //??????(??)
        __delay_ms(5);
        LATAbits.LA4 = 0;
        __delay_ms(500);
        
        /*?2???D=0?*/
        LATAbits.LA2 = 0;

        __delay_ms(500);
        
        /*?2???D=1?*/
        LATAbits.LA2 = 1;
        LATAbits.LA4 = 1;
        __delay_ms(5);
        LATAbits.LA4 = 0;
       
        __delay_ms(500);
}

void D_ffclock(void){
         LATAbits.LA4 = 1;  
        __delay_ms(5);
        LATAbits.LA4 = 0;
}
