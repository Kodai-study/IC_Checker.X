/*
 * File:   functions.c
 * Author: gorin
 *
 * Created on 2022/07/19, 18:04
 */


#include <xc.h>
#include "datas.h"

extern const int ic_kinds;
int is_info[] = {0};

//#define SW1 PORTAbits.RA0

//#define SW2 PORTAbits.RA0


/* �|�[�g�ԍ��A�r�b�g��������ƁA���̒[�q�ɗ����オ��N���b�N���o�͂��� */
/*
void clock(PORT_NUM portnumber,int bitNum){
    switch(portnumber){
        case A : LATA |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATA &= ~bitPattern[bitNum]; break;
        case B : LATB |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATB &= ~bitPattern[bitNum]; break;
        case C : LATC |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATC &= ~bitPattern[bitNum]; break;
        case D : LATD |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATD &= ~bitPattern[bitNum]; break;
        case E : LATE |= bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATE &= ~bitPattern[bitNum]; break;
    }
}
*/
/* �|�[�g�ԍ��A�r�b�g������āA���̒[�q�ɗ���������N���b�N���o�͂��� */
/*
void downClock(PORT_NUM portnumber,int bitNum){
    switch(portnumber){
        case A : LATA &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATA |= bitPattern[bitNum]; break;
        case B : LATB &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATB |= bitPattern[bitNum]; break;
        case C : LATC &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATC |= bitPattern[bitNum]; break;
        case D : LATD &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATD |= bitPattern[bitNum]; break;
        case E : LATE &= ~bitPattern[bitNum]; __delay_ms(PLUS_TIME); LATE |= bitPattern[bitNum]; break;
    }
}*/

/* �X�C�b�`1�C2���Ď����āA�V���ɉ����ꂽ��t���O�𗧂Ă� */
void sw_check(){
    static int count1 = 0;
    static int count2 = 0;
    static _Bool old1 = 0;
    static _Bool old2 = 0;
    static _Bool state1 = 0;
    static _Bool state2 = 0;
    
    if(SW1 == old1){
        if(++count1 >= 5){
            if(SW1 != 0 && state1 == 0){
                sw1_flg = 1;
            }
            state1 = SW1;
        } 
    } else {
       count1 = 0;
    }
    
    if(SW2 == old2){
        if(++count2 >= 5){
            if(SW2 != 0 && state2 == 0){
                sw2_flg = 1;
            }
            state2 = SW2;
        } 
    } else {
        count2 = 0;
    }
    old1 = SW1;
    old2 = SW2;
}

/* �����Ŏ󂯎������ނ�IC��P�̃`�F�b�N����B */
void single_check(int kind){
    CHECK_RESULT result = NO_CHECK;
    LCD_Clear();
    LCD_String(ic_names[select_item]);
    LCD_String(" : \nchecking...");
    LCD_CursorOn();
    
    /* �I�����ꂽIC��P�̃`�F�b�N���āA���ʂ�ϐ��Ɋi�[ */
    switch(kind){
        case 1 : result = OK;    break;
        case 2 : result = NG;   break;
        default : result = ERROR;   break;
    }
    LCD_CursorOff();
    /* �`�F�b�N�̌��ʂ�\�� */
    switch(result){
        case OK : LCD_Locate(1,0); LCD_String("OK!!!       ");  break;      //OK�Ȃ��2�s�ڂ�OK��\��
        case NG : LCD_Clear(); LCD_String(ic_names[select_item]);
            LCD_String(" : NG!!\n��ò : �����");  break;             //NG�Ȃ�A2�s�ڂɎ��̎w����\��
        case ERROR : LCD_Locate(1,0);  LCD_String("ERROR...");  break;
        default : break;
    }
    while(1){
        T0_WAIT;
        sw_check();         
        if(sw2_flg != 0){
            now_mode = SINGLE_TEST;
            return;
        }
    }
}

/* �d������l�𒴂����Ƃ��Ɍx����\�����ăv���O�������I������B */
void current_over(){
    LCD_Clear();
    SW_TRIS = 0;
    POWER_SW = 0;
    LCD_String("����ح���ݼ��\n�¿޸����ݼ� ؾ��");
    TRISAbits.TRISA2 = 0;
    
    LATB = 0;
    while(1){       //���̃v���O�����ւ͈ڍs���Ȃ�
        __delay_ms(125);
        LATAbits.LA2 = ~LATAbits.LA2;   //�ԐFLED��_��
    }
}

void cancel(){
    LCD_Clear();
    LCD_String("��ݾټϼ�...");
    for(int i = 0;i < 700;i++){
        T0_WAIT;
    }
    return;
}

/* sw3�A�߂�{�^���������ꂽ���ɁA�O�̃��[�h�ɖ߂鏈�� */
void mode_change(){
    //HOME, ALL_CHECK, CHECK_SELECT, ALL_RESULT, SINGLE_RESULT, SINGLE_TEST
    switch(now_mode){
        case ALL_CHECK    : cancel();
        case CHECK_SELECT :                 // �S�̃`�F�b�N�A�I����ʁA�S�̃`�F�b�N�̎��̓z�[����ʂɖ߂�
        case ALL_RESULT   : now_mode = HOME; break; 
        case SINGLE_TEST  : cancel();
        case SINGLE_RESULT: now_mode = CHECK_SELECT; break; //�P�̃e�X�g���A�P�̃e�X�g�̌��ʕ\���̎��͍��ڑI����ʂɖ߂�
        default : break;
    }
}

void all_check(){
    
}