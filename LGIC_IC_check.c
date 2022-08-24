/*
 * File:   IC_check.c
 * Author: gorin
 *
 * Created on 2022/07/15, 19:22
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"
#include "lcdlib_xc8_v03.h"

#define Q_PORT PORTD
#define Q_LAT  LATD
#define Q_TRIS TRISD

/* 74393�̏o��Q0?3��0?3�ԃ|�[�g�ɁA2�ڂ�Q0?3��4?8�ԃ|�[�g�ɐڑ� */

#define DFF_CL1 LAT(D,0)
#define DFF_PR1 LAT(D,1)
#define DFF_Q1  P(D,2)
#define DFF_nQ1 P(D,3)
#define DFF_nQ2 P(D,4)
#define DFF_Q2  P(D,5)
#define DFF_PR2 LAT(D,6)
#define DFF_CL2 LAT(D,7)



#define DFF_COMCLR LAT(C,0)
#define DFF_COMD   LAT(C,1)

#define COMCLR_TRIS TRIS(C,0)
#define COMD_TRIS TRIS(C,1)

#define CO_CLR DFF_COMD
#define CO_CLK DFF_COMCLR
#define CO_CLR_TRIS COMD_TRIS
#define CO_CLK_TRIS COMCLR_TRIS

#define DFF_CHECK(b) \
    if(DFF_Q1 != b || DFF_nQ1 != (!b) || DFF_Q2 != b || DFF_nQ2 != (!b))  {return NG;}

extern unsigned char bitPattern[8]; //�Y�����̐��̃r�b�g�݂̂�1�ɂȂ����f�[�^�z��
/*
 * D-�t���b�v�t���b�v�̃`�F�b�N
 * �N���A�A�v���Z�b�g�AD��0���́�1����  �ŏo�͂�
 * 0,1,0,1 �̏��ɕω����邱�Ƃ��m���߂�
 * 
 */
CHECK_RESULT dff_Check(int mode){
        LCD_Clear();
        Q_TRIS = 0b00111100;
        COMD_TRIS = 0;
        COMCLR_TRIS = 0;
        LATB = 0;
        Q_LAT = 0x00;  //CLR.PRC��1(off)��
        DFF_PR1 = 1;
        DFF_PR2 = 1;
        DFF_COMCLR = 1;
        DOWN_CLOCK(DFF_PR1);        //�v���Z�b�g�[�q������H�ŏ�����
        DOWN_CLOCK(DFF_PR2);
        
        LCD_Number(0);
        DFF_CHECK(1)
        
        DOWN_CLOCK(DFF_COMCLR);       //�N���A�[�q��0��
        __delay_ms(WAIT_TIME);
        LCD_Number(1);
        DFF_CHECK(0)
                
        DOWN_CLOCK(DFF_PR1);       //�v���Z�b�g�[�q��0��
        DOWN_CLOCK(DFF_PR2);
        LCD_Number(2);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
                
        /* D�[�q��0�ɂ��ăN���b�N */
        DFF_COMD = 0;
        __delay_ms(WAIT_TIME);
        CLOCK(DFF_CL1)
        CLOCK(DFF_CL2)
        LCD_Number(3);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(0)
        
        /* D�[�q��1�ɂ��ăN���b�N */
        DFF_COMD = 1;
        __delay_ms(WAIT_TIME);
        CLOCK(DFF_CL1);
        CLOCK(DFF_CL2);
        LCD_Number(4);
        __delay_ms(WAIT_TIME);
        DFF_CHECK(1)
        
        return OK;
}

int num_check(int value);

CHECK_RESULT count_check(int mode){
    LCD_Clear();
    Q_TRIS = 0xff;  
    CO_CLK_TRIS = 0;
    CO_CLR_TRIS = 0;
    CO_CLR = 0;
    CO_CLK = 0;
    
    CLOCK(CO_CLR)           //�N���A�[�q��1�ɂ��ďo�͂��N���A
    /* 1?15�܂Œl���J�E���g�A�b�v�����A�o�͂��`�F�b�N */
    for(int i = 0;i < 16;i++){
        if(num_check(i) == 0){
            return NG;
        }
        CLOCK(CO_CLK)
    }
    if(num_check(0) != 0){      //16��ڂ̃J�E���g�A�b�v�ŁA0�ɂȂ邱�Ƃ��`�F�b�N
        return OK;
    }
}

/* �J�E���gIC�̏o�͂������Ɠ����������`�F�b�N */
int num_check(int value){
    LCD_Clear();
    if(value != (Q_PORT & 0x0f)){
        return 0;
    } 
    unsigned char num = P(D,7) ? 1 : 0;
    if(P(D,6)) num |= 2;
    if(P(D,5)) num |= 4;
    if(P(D,4)) num |= 8;
    if(value != num){
        return 0;
    }
    LCD_Number(Q_PORT);
    __delay_ms(500);
    return 1;
}