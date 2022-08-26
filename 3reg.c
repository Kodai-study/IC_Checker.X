/*
 * File:   3reg.c
 * Author: kouda
 *
 * Created on 2022/08/25, 21:24
 */
#define V_SOURCE LAT(B,1)
#define V_TRIS   TRIS(B,1)

#define R_CHANGE  LAT(A,2)
#define R_CH_TRIS TRIS(A,2)

#define CHECK_VOLT if(ADRESH <= 50 || ADRESH >= 200) {V_SOURCE = 0; R_CHANGE = 0; return NG; }

#include "datas.h"

void convert(int an_chan){
    ADCON0bits.CHS = an_chan;
    __delay_ms(1);
    /* �ϊ��J�n�A�I���܂ő҂� */
    ADCON0bits.GO = 1;
    while(ADCON0bits.GO) {;}
}


CHECK_RESULT reg_check(int mode){
    V_TRIS = 0;
    R_CH_TRIS = 0;
    ADCON1bits.PCFG = 0b1101;       //AN0~2���A�i���O����
    ADCON0bits.ADON = 1;        //A/D�R���o�[�^��L��
    ADCON2bits.ADFM = 0;        //�E�l�߂ŁA���8�r�b�g�݂̂��g��
    ADCON2bits.ACQT = 0;        //GO��1�ɂ����Ƃ��ɕϊ��J�n
    
    V_SOURCE = 0;
    R_CHANGE = 0;
    convert(1);      // �ϊ��J�n�A�I���܂ő҂� 
    CHECK_VOLT;
    R_CHANGE = 1;
    convert(1);
    CHECK_VOLT;
    __delay_ms(500);
    CANCEL_CHECK;
    
    V_SOURCE = 0;
    R_CHANGE = 1;
    convert(1);      // �ϊ��J�n�A�I���܂ő҂� 
    CHECK_VOLT;
    R_CHANGE = 1;
    convert(1);
    CHECK_VOLT;
    __delay_ms(500);
    CANCEL_CHECK;
    
    V_SOURCE = 1;
    R_CHANGE = 0;
    convert(1);
    CHECK_VOLT;
    __delay_ms(1);
    __delay_ms(500);
    CANCEL_CHECK;
    
    R_CHANGE = 1;
    CHECK_VOLT;
    R_CHANGE = 0;
    V_SOURCE = 0;
    __delay_ms(500);
    CANCEL_CHECK;
    return OK;
}
