/*
 * File:   IC_check.c
 * Author: gorin
 *
 * Created on 2022/07/15, 19:22
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"

//#define TM_OUT LATBbits.LATB0
#define TM_OUT PORTBbits.RB0
#define TM_SELECT LATBbits.LATB1

#define DFF_Q1 PORTAbits.RA0
#define DFF_nQ1 PORTAbits.RA1
#define DFF_Q2 PORTAbits.RA2
#define DFF_nQ2 PORTAbits.RA3
#define DFF_CHECK(b) \
    if(DFF_Q1 != b || DFF_nQ1 != (!b) || DFF_Q2 != b || DFF_nQ2 != (!b))  return NG;
#define DFF_D1 LATAbits.LA4
#define DFF_D2 LATAbits.LA5
#define DFF_CLR LATAbits.LA6
#define DRR_PRE LATAbits.LA7

#define CLOCK(a) a = 1; __delay_ms(PLUS_TIME); a = 0;

extern unsigned char bitPattern[8]; //�Y�����̐��̃r�b�g�݂̂�1�ɂȂ����f�[�^�z��
/*
 * D-�t���b�v�t���b�v�̃`�F�b�N
 * �N���A�A�v���Z�b�g�AD��0���́�1����  �ŏo�͂�
 * 0,1,0,1 �̏��ɕω����邱�Ƃ��m���߂�
 * 
 */
CHECK_RESULT dff_Check(){
        TRISA = 0x00;
        LATA = 0b00000101;  //CLR.PRC��1(off)��
        downClock(A, 2);        //�v���Z�b�g�[�q������H�ŏ�����
        DFF_CHECK(1)
        CLOCK(DFF_CLR)
                
        downClock(A, 0);       //�N���A�[�q��0��
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
                
        downClock(A, 2);       //�v���Z�b�g�[�q��0��
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
                
        /* D�[�q��0�ɂ��ăN���b�N */
        DFF_D1 = 0;
        DFF_D2 = 0;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
        
        /* D�[�q��1�ɂ��ăN���b�N */
        DFF_D1 = 1;
        DFF_D2 = 1;
        clock(A, 3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
        
        return OK;
}

/**
 * 
 * LM555 �^�C�}IC�̃`�F�b�N
 * ��R�l��؂�ւ��āA���̎��g���A�f���[�e�B�[����m�F
 * 100k�� : 12.02Hz, 91.6%
 * 9.1k�� : 49.58Hz�A65.6%
 * 
 */
CHECK_RESULT TMchecker(){
    /* �o�͂�1�ɐ؂�ւ�����u�Ԃ��瑪����J�n�B����܂ő҂� */
    int old = TM_OUT;
    int new = old;
    const int cycle = (int)(1000.0 / 12.025);   //12.25Hz�̎��̎���(ms)
    while(old != (new = TM_OUT) && old == 0){
        if(old != new){
            old = new;
        }
        __delay_ms(1);
    }
    
    int onCount;    //�o�͂�0n��������=����(ms)���J�E���g
    int offCount;   
    /* 4�������g���ƃf���[�e�B�[��𑪂� */
    for(int i = 0,onCount = 0,offCount = 0;i < 4;i++){
        while(TM_OUT != 0){
            onCount++;
            __delay_ms(1);
        }
        while(TM_OUT == 0){
            offCount++;
            __delay_ms(1);
        }
        int cycleCount = onCount + offCount;    //1�����̎���(ms)
        
        /* �����A�f���[�e�B�[�䂪����́}5%�ȏ㗣��Ă����玸�s�Ƃ��� */
        if(cycleCount <= (int)(cycle * 1.05) ||
           cycleCount >= (int)(cycle * 0.95) || //���������_�l�}5%
           (double)onCount / cycleCount >= 0.85 ||  //�I�����Ԃ��}5%
           (double)onCount / cycleCount <= 0.95 ||
           (double)offCount / cycleCount >= 0.05 || //�I�t���Ԃ��}5%
           (double)offCount / cycleCount >= 0.15)
            return NG;
    }
    return OK;  //4�������肵�āA���g���ƃf���[�e�B�[�䂪�͈͓��Ȃ�OK
}
