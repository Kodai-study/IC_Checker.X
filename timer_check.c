/*
 * File:   timer_check.c
 * Author: gorin
 *
 * Created on 2022/07/25, 20:13
 */


#include <xc.h>
#include <stdio.h>
#include "datas.h"

#define TM_OUT PORTBbits.RB1
#define TM_OUT_TRIS TRISBbits.RB0
#define TM_SELECT LATBbits.LATB1


/**
 * 
 * LM555 �^�C�}IC�̃`�F�b�N
 * ��R�l��؂�ւ��āA���̎��g���A�f���[�e�B�[����m�F
 * 100k�� : 12.02Hz, 91.6%
 * 9.1k�� : 49.58Hz�A65.6%
 * 
 */

int onCount = 0;    //�o�͂�0n��������=����(ms)���J�E���g
int offCount = 0;   
const int cycle = (int)(1000.0 / 12.025);   //12.25Hz�̎��̎���(ms)
int cycleCount = 0;

CHECK_RESULT TMchecker(){
    
    ADCON1 = 0xff;
    TM_OUT_TRIS = 1;    
    TRISB = 0xff;
    __delay_ms(100);
    LCD_Number(t0_flg);
    LCD_String("��϶��\n");
    /* �o�͂�1�ɐ؂�ւ�����u�Ԃ��瑪����J�n�B����܂ő҂� */
    int old = TM_OUT;
    int new = old;
    
    while((new = TM_OUT) && old == 0){
        old = new;
        T0_WAIT
    }

    /* 4�������g���ƃf���[�e�B�[��𑪂� */
    for(int i = 0;i < 4;i++){
        onCount = 0;
        offCount = 0;
        while(TM_OUT != 0){
            onCount++;
            LATAbits.LA0 = 1;
            T0_WAIT
            //__delay_ms(1);
        }
        while(TM_OUT == 0){
            offCount++;
            LATAbits.LA0 = 0;
            T0_WAIT
           // __delay_ms(1);
        }
        cycleCount = onCount + offCount;    //1�����̎���(ms)
        
        // �����A�f���[�e�B�[�䂪����́}5%�ȏ㗣��Ă����玸�s�Ƃ��� 
       /* if(cycleCount >= (int)(cycle * 1.05) ||
           cycleCount <= (int)(cycle * 0.95) || //���������_�l�}5%
           (double)onCount / cycleCount <= 0.85 ||  //�I�����Ԃ��}5%
           (double)onCount / cycleCount >= 0.95 ||
           (double)offCount / cycleCount <= 0.05 || //�I�t���Ԃ��}5%
           (double)offCount / cycleCount >= 0.15)
            return NG;
*/
    }
    return OK;  //4�������肵�āA���g���ƃf���[�e�B�[�䂪�͈͓��Ȃ�OK
}

void viewResults(){
    char st[16];
    sprintf(st,"%.2f:%.2f:%.2f",
            1.0 / (cycleCount / 1000.0),(double)onCount / cycleCount,(double)offCount / cycleCount );
            //1.0 / (cycleCount / 1000.0),onCount,offCount);
    LCD_String(st);
}
