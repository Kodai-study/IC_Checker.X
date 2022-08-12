/*
 * File:   comp_sample.c
 * Author: gorin
 *
 * Created on 2022/08/10, 13:51
 */


#include <xc.h>
#include "datas.h"

void comp_init(void) {
    ADCON1 = 0x0d;
    TRISAbits.RA0 = 1;
    TRISAbits.RA1 = 1;
    CVRCON = 0;
    CMCON = 0;
    CVRCONbits.CVR = 0b1000;
    CVRCONbits.CVREN = 1;
    CMCONbits.CIS = 0;      //RA0,1をコンパレータの入力にする
    CMCONbits.C1INV = 1;    //C1OUTの値を反転(CINのほうが大きい時にCOUT=1)
    
    PIE2bits.CMIE = 1;      //COUTの値が変わったときに割り込み
    CMCONbits.CM = 0b110;
}
