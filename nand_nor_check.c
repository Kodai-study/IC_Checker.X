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
#define NAND_INPUT 0x0f     //NAND�̃`�F�b�N�ŁAIC�ւ̓��͂Ɋ��蓖�Ă���r�b�g
#define NAND_OUTPUT 0xf0    //NAND�̃`�F�b�N�ŁAIC�̂̏o�͂̓ǂݎ��Ɋ��蓖�Ă���r�b�g
#define NOR_INPUT NAND_OUTPUT   //NOR�̓��́A�o�͒[�q�́ANAND�̒[�q�ݒ�Ƌt
#define NOR_OUTPUT NAND_INPUT   

CHECK_RESULT nand_check(int mode){
    
    const int result[2][4] =            //NAND,NOR�̐^���l�\�̌���
    { {1, 1, 1, 0}, {1, 0, 0, 0} };     //[0]��NAND�A[1]��NOR 
    unsigned char mask;
    COM_INPUT_TRIS = 0;
    if(mode){
        IC3_TRIS = NAND_OUTPUT;
        mask = NAND_OUTPUT;
    } else {
        IC3_TRIS = NOR_OUTPUT;
        mask = NOR_OUTPUT;
    }
    __delay_ms(PLUS_TIME);
    
    for(int i = 0;i <= 3; i++){
        LATCbits.LATC0 = i % 2;
        
        unsigned char data = PORTC & mask;
        if(result[mode][i] == 0 && data != 0){
            return NG;
        } else if(result[mode][i] == 1 && data != mask){
            return NG;
        }
    }
    return OK;
}
