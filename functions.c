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
//CHECK_RESULT results[10] = {NO_CHECK};  //�`�F�b�N���ڂ̌��ʂ�\��
CHECK_RESULT results[11] = {NO_CHECK}; ///�f�o�b�O�p

const char* mode_names[] = {    //�`�F�b�N���郂�[�h��\�����镶����
    " ALL����",
    " ��������"
};

static const char dot = 0xa5;     //LCD�ŁA�I�����ڂ�\���h�b�g�̕����R�[�h
static const char blank = 0xa0;   //LCD�ŁA�󔒂�\��������f�[�^
static const char allow = 0x7e;   //�E���V

int ng_count = 0;                       //�`�F�b�N�����s�������ڂ̐����J�E���g
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
    if(kind == 2){
        LCD_String("3�ݼREG"); 
    } else if(kind == 4){
        LCD_String("7���LED"); 
    } else{
        LCD_String(ic_names[kind]);
    }
    LCD_String(" check...\n");
    LCD_CursorOn();
    //result = check_funcs[kind]();
    /* �I�����ꂽIC��P�̃`�F�b�N���āA���ʂ�ϐ��Ɋi�[ */
    if(kind > 10){
        LCD_String("ERROR...");
        return ;
    }
    switch(kind){
        case 0 : result = seg7_decode(1); break;
        case 1 : result = count2_check(1);    break;
        case 2 : result = reg_check(1);   break;
        
        default : rx_buf = 0;  TXREG = kind; 
        WAIT_RECEIVE(SINGLE_TEST);
           if((rx_buf >> 4) == kind) {
                result = rx_buf & 0x0f;
            } else { 
               result = ERROR;
            }
            rx_buf = 0;  break;
    }
    
    LCD_CursorOff();
    /* �`�F�b�N�̌��ʂ�\�� */
    switch(result){
        case OK : LED_BLUE = 1; LED_GREEN = 0;LED_RED = 1; 
            LCD_Locate(1,0); LCD_String("OK!!!       ");  break;      //OK�Ȃ��2�s�ڂ�OK��\��
        case NG : LCD_Clear(); LCD_String(ic_names[select_item]);
            LED_BLUE = 1; LED_GREEN = 1;LED_RED = 0; 
            LCD_String(" : NG!!");
            LCD_Locate(0,0);
            LCD_String("��ò : �����      ");  break;  
        case ERROR :   LCD_String("ERROR...");  break;
        default : now_mode = CHECK_SELECT; return;
    }
    if(now_mode != SINGLE_TEST)     return;
    now_mode = SINGLE_RESULT;
    while(now_mode == SINGLE_RESULT){
        T0_WAIT;
        sw_check();         
        if(sw2_flg != 0){
            sw2_flg = 0;
            if(result == NG)
                now_mode = SINGLE_TEST;
            else
                now_mode = CHECK_SELECT;
            return;
        }
        if(sw1_flg != 0){
            sw1_flg = 0;
        }
    }
}

/* �d������l�𒴂����Ƃ��Ɍx����\�����ăv���O�������I������B */
void current_over(){
    LCD_Clear();
    SW_TRIS = 0;
    POWER_SW = 0;
    LCD_String("����ح���ݼ��\n�¿޸����ݼ� ؾ��");
    
    LED_BLUE = 1;
    LED_GREEN = 1;
    LED_RED = 0;
    while(1){       //���̃v���O�����ւ͈ڍs���Ȃ�
        __delay_ms(125);
        LED_RED = ~LED_RED;   //�ԐFLED��_��
    }
}

void cancel(){
    LCD_Clear();
    LCD_String("��ݾټϼ�...");
    __delay_ms(700);
    return;
}

/* sw3�A�߂�{�^���������ꂽ���ɁA�O�̃��[�h�ɖ߂鏈�� */
void mode_change(){
    //HOME, ALL_CHECK, CHECK_SELECT, ALL_RESULT, SINGLE_RESULT, SINGLE_TEST
    switch(now_mode){
        case ALL_CHECK    : cancel();
        case CHECK_SELECT :                 // �S�̃`�F�b�N�A�I����ʁA�S�̃`�F�b�N�̎��̓z�[����ʂɖ߂�
        case ALL_RESULT   : now_mode = HOME; LCD_Clear(); break;
        
        case SINGLE_TEST  : cancel();
        case SINGLE_RESULT: now_mode = CHECK_SELECT; LCD_Clear();break; //�P�̃e�X�g���A�P�̃e�X�g�̌��ʕ\���̎��͍��ڑI����ʂɖ߂�
        case RETRY        : cancel(); now_mode = ALL_RESULT; LCD_Clear(); break;      
        default : break;
    }
}

void all_check(){
    LCD_Clear();
    LCD_String("�������...");
    rx_buf = 0;
    TXREG = 0xff;
    results[0] = seg7_decode(0);
    results[2] = reg_check(0);
    
    
    WAIT_RECEIVE(ALL_CHECK);
    
    
    if(rx_buf == 0xaa){
        LCD_Clear();
        LCD_String("IC� �ض�ø�޻�\n��ò : ���");
        while(sw2_flg == 0){ T0_WAIT; sw_check();  }
        LCD_Clear();
        LCD_String("�������...");
    } else {
        LCD_String("ERROR...\n�ƶ��� ��ø�޻�");
        while(sw2_flg == 0 && sw1_flg == 0 && now_mode == ALL_CHECK) {
            T0_WAIT; sw_check();  
        }
        sw2_flg = 0;
        sw1_flg = 0;
        now_mode = HOME;
        return;
    }
    rx_buf = 0;
    TXREG = 0xfe;
    results[1] = count2_check(0);
    if(results[2] != OK){
        results[2] = reg_check(0);
    }
    WAIT_RECEIVE(ALL_CHECK);
    for(int i = 3; i <= 10;i++){
        results[i] = (rx_buf & 0x01) ? OK : NG;
        rx_buf >>= 1;
    }
    
    now_mode = ALL_RESULT;
    return;
}

int next(int index){
    for(int i = index + 1;i <= 10;i++){
        if(results[i] != OK){
            return i;
        }
    }
    for(int i = 0;i < index;i++){
        if(results[i] != OK){
            return i;
        }
    }
    return -1;
}

void all_results(){
    LCD_Clear();
    int cur = 0;
    int index = -1;
    ng_count = 0;
    for(int i = 0;i < 11;i++){
        if(results[i] != OK){
            if(ng_count == 0){
                LED_BLUE = 1;
                LED_RED = 0;
                LED_GREEN = 1;
                LCD_String("NG���� ��� : \n");
                LCD_Character(dot);
                LCD_String(ic_names[i]);
            } else if(ng_count == 1){
                LCD_Locate(1,15); LCD_Character(allow);
            }
            ng_count++;
        }
    }
    
    if(ng_count == 0){
        LCD_String("ALL_OK!!!!");
        LED_BLUE = 1;
        LED_RED = 1;
        LED_GREEN = 0;
        sw2_flg = 0;
        sw1_flg = 0;
        while(sw2_flg == 0 && sw1_flg == 0 && now_mode == ALL_CHECK) {
            T0_WAIT; sw_check();  
        }
        sw2_flg = 0;
        sw1_flg = 0;
        now_mode = HOME;
        return;
    }
    
    LCD_Locate(0,13);
    LCD_Number(ng_count);
    LCD_Locate(1,0);
    sw2_flg = 0;
    sw1_flg = 0;
    while(now_mode == ALL_RESULT){
        T0_WAIT;
        sw_check();
        
        if(sw2_flg != 0){
            sw2_flg = 0;
            if(index == -1){
                index = next(index);
            }
            results[index] = retry(index);
            now_mode = ALL_RESULT;
            return;
        }
        
        if(sw1_flg == 0 || ng_count == 1)
            continue;
        
        sw1_flg = 0;
        if(++cur > ng_count){
            cur = 1;
        }
        
        if(cur < ng_count){
            LCD_Clear();
            
            index = next(index);
            LCD_Character(dot);
            LCD_String(ic_names[index]);
            LCD_String("\n ");
            LCD_String(ic_names[next(index)]);
            if(cur < ng_count - 1){
                LCD_Locate(1,15);
                LCD_Character(allow);       //�܂����ɍ��ڂ�����Ƃ��́A����\��
            }
                /* �ŏI���ڂł́A���̒i��I�� */
        } else if(cur == ng_count){
            index = next(index);
            LCD_Locate(0,0);
            LCD_Character(blank);
            LCD_Locate(1,0);
            LCD_Character(dot);
        }
    }
}


/* �P�̃`�F�b�N���A�S�Ă��`�F�b�N���邩��I������ */
void menu_mode(){
    LCD_Clear();        
    static unsigned int cur = 0;     //�I�����[�h���ǂ��炩���i�[
    SW1_TRIS = 1;           
    SW2_TRIS = 1;
    
    /* �����͑S�ă`�F�b�N���[�h */       
    LCD_String(mode_names[0]);
    LCD_String("\n");
    LCD_String(mode_names[1]);
    LCD_Locate(cur,0);
    LCD_Character(dot);
    while(now_mode == HOME){
        T0_WAIT
        sw_check();     //1ms���ƂɁA�X�C�b�`�������ꂽ���ǂ����`�F�b�N
        /* �X�C�b�`1�������ꂽ��A�I�����[�h��ύX */
        if(sw1_flg != 0){       
            sw1_flg = 0;
            LCD_Locate(cur,0);
            LCD_Character(blank);
            cur = !cur;
            LCD_Locate(cur,0);
            LCD_Character(dot);
        }
        /* �X�C�b�`2�������ꂽ��A�I�����ꂽ���[�h�Ɉڍs */
        if(sw2_flg != 0){
            sw2_flg = 0;
            now_mode = (cur == 0) ? ALL_CHECK : CHECK_SELECT;
            return;
        }
    }
}

void view_names(int cur){
    if(cur < ic_kinds - 1){
        LCD_Character(dot);
        LCD_String(ic_names[cur]);
        LCD_String("\n ");
        LCD_String(ic_names[cur + 1]);
        if(cur < ic_kinds - 2){
            LCD_Locate(1,15);
            LCD_Character(allow);       //�܂����ɍ��ڂ�����Ƃ��́A����\��
        }
                /* �ŏI���ڂł́A���̒i��I�� */
    } else if(cur == ic_kinds - 1){
        LCD_Character(blank);
        LCD_String(ic_names[cur - 1]);
        LCD_String("\n");
        LCD_Character(dot);
        LCD_String(ic_names[cur]);
    } else {
        cur = 0;
        LCD_Character(dot);
        LCD_String(ic_names[0]);
        LCD_String("\n ");
        LCD_String(ic_names[1]);
        LCD_Locate(1,15);
        LCD_Character(allow); 
    }    
}

CHECK_RESULT retry(int kind){
    now_mode = RETRY;
    CHECK_RESULT result = NO_CHECK;
    LCD_Clear();
    LCD_Number(kind);
    if(kind == 2){
        LCD_String("3�ݼREG"); 
    } else if(kind == 4){
        LCD_String("7���LED"); 
    } else{
        LCD_String(ic_names[kind]);
    }
    LCD_String(" check...\n");
    //result = check_funcs[kind]();
    /* �I�����ꂽIC��P�̃`�F�b�N���āA���ʂ�ϐ��Ɋi�[ */
    if(kind > 10){
        LCD_String("ERROR...");
        return NG;
    }
    switch(kind){
        case 0 : result = seg7_decode(1); break;
        case 1 : result = count2_check(1);    break;
        case 2 : result = reg_check(1);   break;
        
        default : rx_buf = 0;  TXREG = kind; 
             while(rx_buf == 0) { if(now_mode != RETRY) return NG; }
            if((rx_buf >> 4) == kind) {
                result = rx_buf & 0x0f;
            } else { 
               result = ERROR;
            }
            rx_buf = 0;  break;
    }
    /* �`�F�b�N�̌��ʂ�\�� */
    switch(result){
        case OK : 
            LED_BLUE = 1; LED_GREEN = 0;LED_RED = 1; 
            LCD_Locate(1,0);
            now_mode = ALL_RESULT;
            LCD_String("OK!!!       "); 
            __delay_ms(700);
            return OK;
            break;
        case NG : 
            LED_BLUE = 1; LED_GREEN = 1;LED_RED = 0; 
            now_mode = ALL_RESULT;
            LCD_String(" : NG!!");
            __delay_ms(1000);
            return NG;
            break;
        /*default : LCD_Clear();  LCD_String("ERROR...");  
            __delay_ms(1000);
            return NG;
            break;*/
    }
}



void comp_init(void) {
    ADCON1 = 0x0d;
    TRISAbits.RA0 = 1;
    TRISAbits.RA1 = 1;
    CVRCON = 0;
    CMCON = 0;
    CVRCONbits.CVR = 0b1000;
    CVRCONbits.CVREN = 1;
    CMCONbits.CIS = 0;      //RA0,1���R���p���[�^�̓��͂ɂ���
    CMCONbits.C1INV = 1;    //C1OUT�̒l�𔽓](CIN�̂ق����傫������COUT=1)
    
    PIE2bits.CMIE = 1;      //COUT�̒l���ς�����Ƃ��Ɋ��荞��
    CMCONbits.CM = 0b110;
}