/*
 * File:   nand_nor_check.c
 * Author: kouda
 *
 * Created on 2022/07/20, 23:01
 */


#include <xc.h>
#include "datas.h"
#include "fucntions.h"

#define COM_INPUT LATBbits.LB0
#define COM_INPUT_TRIS TRIS(B,0)
#define IC3_TRIS TRISA
#define IC3_LAT LATA
#define IC3_PORT PORTA
#define NAND_INPUT 0x0f     //NAND�̃`�F�b�N�ŁAIC�ւ̓��͂Ɋ��蓖�Ă���r�b�g
#define NAND_OUTPUT 0xf0    //NAND�̃`�F�b�N�ŁAIC�̂̏o�͂̓ǂݎ��Ɋ��蓖�Ă���r�b�g
#define NOR_INPUT NAND_OUTPUT   //NOR�̓��́A�o�͒[�q�́ANAND�̒[�q�ݒ�Ƌt
#define NOR_OUTPUT NAND_INPUT   

CHECK_RESULT nand_check(int mode){
    
    const int result[2][4] =            //NAND,NOR�̐^���l�\�̌���
    { {1, 1, 1, 0}, {1, 0, 0, 0} };     //[0]��NAND�A[1]��NOR
    unsigned char mask;
    COM_INPUT_TRIS = 0;

    /* ���[�h�ɂ���āA�o�́A���͂����蓖�Ă� */
    if(mode){
        IC3_TRIS = NAND_OUTPUT;
        mask = NAND_OUTPUT;
    } else {
        IC3_TRIS = NOR_OUTPUT;
        mask = NOR_OUTPUT;
    }
    
    /* ���͂�00?11�܂Ő؂�ւ��A�o�͂��^���l�\�ʂ�ł��邱�Ƃ��m���߂� */
    for(int i = 0;i <= 3; i++){
        COM_INPUT = i % 2;                 //���͂�1���ڂ̒l�����ʕ����ɏo��
        IC3_LAT = (i & 0x02) != 0 ? ~mask : 0;  //2���ڂ́A���ꂼ��̓��͕��ɏo��
        __delay_ms(WAIT_TIME);

        unsigned char data = IC3_PORT & mask;
        if(result[mode][i] == 0 && data != 0){  //�o�͂̐^���l��0�̎��ɁA�o�͂�0�Ȃ�OK
            return NG;
        } else if(result[mode][i] == 1 && data != mask){    //�^���l1�̎��A�o�͂�1�Ȃ�OK
            return NG;
        }
    }
    return OK;      //0?3�ŊԈ�����o�͂�1���Ȃ�������OK
}
