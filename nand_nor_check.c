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
#define IC3_TRIS TRISB
#define IC3_LAT LATB
#define IC3_PORT PORTB
#define NAND_INPUT 0x0f     //NAND�̃`�F�b�N�ŁAIC�ւ̓��͂Ɋ��蓖�Ă���r�b�g
#define NAND_OUTPUT 0xf0    //NAND�̃`�F�b�N�ŁAIC�̂̏o�͂̓ǂݎ��Ɋ��蓖�Ă���r�b�g
#define NOR_INPUT NAND_OUTPUT   //NOR�̓��́A�o�͒[�q�́ANAND�̒[�q�ݒ�Ƌt
#define NOR_OUTPUT NAND_INPUT   

CHECK_RESULT nand_check(int mode){
    
    const int result[2][4] =            //NAND,NOR�̐^���l�\�̌���
    { {1, 1, 1, 0}, {1, 0, 0, 0} };     //[0]��NAND�A[1]��NOR 
    unsigned char mask;                 //IC����̏o�͂�ǂݎ��r�b�g
    COM_INPUT_TRIS = 0;

    /* �`�F�b�N����IC�ɍ��킹�āA�|�[�g�̏o�́A���͂̐ݒ� */
    if(mode){
        IC3_TRIS = NAND_OUTPUT;
        mask = NAND_OUTPUT;
    } else {
        IC3_TRIS = NOR_OUTPUT;
        mask = NOR_OUTPUT;
    }
    __delay_ms(PLUS_TIME);
    
    /* �^���l�\��0?3����͂��ďo�͂��m�F */
    for(int i = 0;i <= 3; i++){
        LATCbits.LATC0 = i % 2;                 //���͂�1�̌��͋��ʂ̓��͕����ɏo��
        IC3_LAT = (i & 0x02) == 2 ? ~mask : 0;  //���͂�2�̌��͂��ꂼ��̓��͒[�q�ɏo��
        __delay_ms(WAIT_TIME);
        unsigned char data = IC3_PORT & mask;
        if(result[mode][i] == 0 && data != 0){  //�^���l�\�̌��ʂ�0�̎��́A�o�͂����ׂ�0�łȂ���΂����Ȃ�
            return NG;
        } else if(result[mode][i] == 1 && data != mask){    //�^���l�\�̌��ʂ�1
            return NG;
        }
    }
    return OK;
}
