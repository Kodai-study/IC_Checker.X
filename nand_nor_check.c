/*
 * File:   nand_nor_check.c
 * Author: kouda
 *
 * Created on 2022/07/20, 23:01
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"

#define COM_INPUT       LAT(C,5)
#define COM_INPUT_TRIS  TRIS(C,5)
#define IC3_TRIS TRISD
#define IC3_LAT LATD
#define IC3_PORT PORTD
#define NAND_INPUT 0x0f     //NAND�̃`�F�b�N�ŁAIC�ւ̓��͂Ɋ��蓖�Ă���r�b�g
#define NAND_OUTPUT 0xf0    //NAND�̃`�F�b�N�ŁAIC�̂̏o�͂̓ǂݎ��Ɋ��蓖�Ă���r�b�g
#define NOR_INPUT NAND_OUTPUT   //NOR�̓��́A�o�͒[�q�́ANAND�̒[�q�ݒ�Ƌt
#define NOR_OUTPUT NAND_INPUT   

CHECK_RESULT nand_check(int mode){
    
    const int result[4] = {1, 1, 1, 0}; //NAND�̐^���l�\�̌���
        
    COM_INPUT_TRIS = 0;

    /* ���[�h�ɂ���āA�o�́A���͂����蓖�Ă� */
        IC3_TRIS = NAND_OUTPUT;
        
    /* ���͂�00?11�܂Ő؂�ւ��A�o�͂��^���l�\�ʂ�ł��邱�Ƃ��m���߂� */
    for(int i = 0;i <= 3; i++){
        LCD_Number(i);
        COM_INPUT = i % 2;                 //���͂�1���ڂ̒l�����ʕ����ɏo��
        IC3_LAT = (i & 0x02) != 0 ? NAND_INPUT : 0;  //2���ڂ́A���ꂼ��̓��͕��ɏo��
        __delay_ms(WAIT_TIME);

        unsigned char data = IC3_PORT & NAND_OUTPUT;
        LCD_HNumber(IC3_PORT,2);
        if(result[i] == 0 && data != 0){  //�o�͂̐^���l��0�̎��ɁA�o�͂�0�Ȃ�OK
            return NG;
        } else if(result[i] == 1 && data != NAND_OUTPUT){    //�^���l1�̎��A�o�͂�1�Ȃ�OK
            return NG;
        }
    }
    return OK;      //0?3�ŊԈ�����o�͂�1���Ȃ�������OK
}


CHECK_RESULT nor_check(int mode){
   
    const int result[4] =  {1, 0, 0, 0};  //NAND,NOR�̐^���l�\�̌���
       
    COM_INPUT_TRIS = 0;

    /* ���[�h�ɂ���āA�o�́A���͂����蓖�Ă� */
    IC3_TRIS = NOR_OUTPUT;
        
    /* ���͂�00?11�܂Ő؂�ւ��A�o�͂��^���l�\�ʂ�ł��邱�Ƃ��m���߂� */
    for(int i = 0;i <= 3; i++){
        LCD_Number(i);
        COM_INPUT = i % 2;                 //���͂�1���ڂ̒l�����ʕ����ɏo��
        IC3_LAT = (i & 0x02) != 0 ? NOR_INPUT : 0;  //2���ڂ́A���ꂼ��̓��͕��ɏo��
        __delay_ms(WAIT_TIME);
        LCD_HNumber(IC3_PORT,2);
        unsigned char data = IC3_PORT & NOR_OUTPUT;
        if(result[i] == 0 && data != 0){  //�o�͂̐^���l��0�̎��ɁA�o�͂��S��0�Ȃ�OK
            return NG;
        } else if(result[i] == 1 && data != NOR_OUTPUT){    //�^���l1�̎��A�o�͂��S��1�Ȃ�OK
            return NG;
        }
    }
    return OK;      //0?3�ŊԈ�����o�͂�1���Ȃ�������OK
}