/*
 * File:   analog.c
 * Author: kouda
 *
 * Created on 2022/08/15, 21:46
 */

#include "datas.h"
#include <xc.h>

#define R_CHENGE LATAbits.LATA6
#define CHENGE_TRIS TRISAbits.TRISA6

#define REG_POWER LATAbits.LA3


void convert(int an_chan);

CHECK_RESULT opamp_check(int mode){
    ADCON1bits.PCFG = 0b1100;       //AN0~3���A�i���O����
    CHENGE_TRIS = 0;
    R_CHENGE = 0;
    ADCON0bits.ADON = 1;        //A/D�R���o�[�^��L��
    ADCON2bits.ADFM = 0;        //�E�l�߂ŁA���8�r�b�g�݂̂��g��
    ADCON2bits.ACQT = 0;        //GO��1�ɂ����Ƃ��ɕϊ��J�n
    /* �ϊ��J�n�A�I���܂ő҂� */
    convert(1);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    convert(2);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    
    R_CHENGE = 1;
    convert(1);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    convert(2);
    if(ADRESH <= 0 || ADRESH >= 255){
        return NG;
    }
    R_CHENGE = 0;
    return OK;
}

CHECK_RESULT reg_check(int mode){
    
}


void convert(int an_chan){
    ADCON0bits.CHS = an_chan;
    __delay_ms(1);
    /* �ϊ��J�n�A�I���܂ő҂� */
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO) {;}
}