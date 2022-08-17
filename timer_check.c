/*
 * File:   timer_check.c
 * Author: gorin
 *
 * Created on 2022/07/25, 20:13
 */


#include <xc.h>
#include <stdio.h>
#include "datas.h"

#define TM_OUT P(B,3)
#define TM_OUT_TRIS TRIS(B,3)
#define TM_SELECT LAT(B,4)
#define TM_SL_TRIS TRIS(B,4)


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
const int cycle[] = {(int)(1000.0 / 12.025), (int)(1000.0 / 49.58),};   //12.25Hz�̎��̎���(ms)
const double on_time[] = {0.916, 0.656};
int cycleCount = 0;

CHECK_RESULT TMchecker(int mode){
    
    ADCON1 = 0xff;
    TM_OUT_TRIS = 1; 
    TM_SL_TRIS = 0;
    __delay_ms(100);
    LCD_Number(t0_flg);
    LCD_String("��϶��\n");
    /* �o�͂�1�ɐ؂�ւ�����u�Ԃ��瑪����J�n�B����܂ő҂� */
    int old = TM_OUT;
    int new = old;
    
    for(int i = 0;i <= 1;i++){
        TM_SELECT = i;
        while((new = TM_OUT) == 0 || old != 0){
            old = new;
        }

        /* 4�������g���ƃf���[�e�B�[��𑪂� */
        for(int j = 0;j < 4;j++){
            LCD_Clear();
            onCount = 1;
            offCount = 0;
            while(TM_OUT != 0){
                onCount++;
                T0_WAIT;
            }
            onCount++;
            while(TM_OUT == 0){
                offCount++;
                T0_WAIT;
            }
            offCount++;
            cycleCount = onCount + offCount;    //1�����̎���(ms)

            // �����A�f���[�e�B�[�䂪����́}20%�ȏ㗣��Ă����玸�s�Ƃ��� 
           if(cycleCount >= (int)(cycle[i] * 1.2) ||
               cycleCount <= (int)(cycle[i] * 0.8) || //���������_�l�}20%
               ((double)onCount / cycleCount) >= on_time[i] * 1.2  ||  //�I�����Ԃ��}20%
               ((double)onCount / cycleCount) <= on_time[i] * 0.8 )
           {
                return NG;
           }
        }
        viewResults();
        __delay_ms(500);
    }
    return OK;  //4�������肵�āA���g���ƃf���[�e�B�[�䂪�͈͓��Ȃ�OK
}

void viewResults(){
    char st[16];
    sprintf(st,"%d:%d\n%.2f:%.2f",
            cycleCount,cycle[0],((double)onCount / cycleCount),on_time[0] );

    LCD_String(st);

}
