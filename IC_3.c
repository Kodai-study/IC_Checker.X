/*
 * File:   IC_3.c
 * Author: kouda
 *
 * Created on 2022/08/14, 22:26
 */
#include "datas.h"

#define DATA_B      LAT(D,2)
#define Q_B         P(D,1)
#define Q_A         P(D,0)
#define C_ENABLE    LAT(C,3)
#define D_U         LAT(C,2)
#define Q_C         P(C,1)
#define Q_D         P(C,0)
#define DATA_D      LAT(D,7)
#define DATA_C      LAT(D,6)
#define LOAD        LAT(D,5)
#define MAX_MIN     P(D,4)
#define R_CK        P(C,5)
#define CLK         LAT(C,4)
#define DATA_A      LAT(D,3)

#define COUNT_TRIS_C 0b00100011
#define COUNT_TRIS_D 0b00010011

#define DATA_2      LAT(D,2)
#define DATA_3      LAT(D,1)
#define LANP_T      LAT(D,0)
#define BLANK       LAT(C,3)
#define L_ENABLE    LAT(C,2)
#define DATA_4      LAT(C,1)
#define DATA_1      LAT(C,0)
#define LED_E       P(D,7)
#define LED_D       P(D,6)
#define LED_C       P(D,5)
#define LED_B       P(D,4)
#define LED_A       P(C,5)
#define LED_G       P(C,4)
#define LED_F       P(D,3)

#define SEG7_TRIS_C 0b00110000
#define SEG7_TRIS_D 0b11111000

#define ALL_ON 10
#define ALL_OFF 11


#include <xc.h>
int value_check(int value);
void num_input(int value);
void set_7seg(int value);

CHECK_RESULT count2_check(int mode){
    TRISC = (TRISC & 0xc0) | COUNT_TRIS_C;  //6,7ビット目をそのままに、ポートCの入出力設定
    TRISD = COUNT_TRIS_D;                   
    LATC = 0;
    LATD = 0;
    //出力を0で初期化
    num_input(0);       
    DOWN_CLOCK(LOAD);
    D_U = 0;  
    /* カウントアップで、値が0→15まで変化することを確かめる */
    for(int i = 0;i < 16;i++){
        if(!value_check(i)){    //値が違っていたら検査終了
            return NG;
        }
        CLOCK(CLK);
    }
    if(!value_check(0)){
        return NG;
    }
    LCD_String("ｶｳﾝﾄﾀﾞｳﾝ");
    __delay_ms(500);
    D_U = 1;        //カウントダウンモードに
    CLOCK(CLK);
    for(int i = 15;i >= 0;i--){ //値が15?0に変化する
        if(!value_check(i)){
           return NG;
        }
        CLOCK(CLK);
    }
    if(!value_check(15)){
        return NG;
    }
    num_input(0);       //値を0にする(1?0に変化するか)
    DOWN_CLOCK(LOAD);
    if(!value_check(0)){
       return NG;
    }
    num_input(15);      //値を15に(0?1の変化)
    DOWN_CLOCK(LOAD);
    if(value_check(15)){
        return OK;
    } else {
        return NG;
    }
}

CHECK_RESULT seg7_decode(int mode){
    TRISC = (TRISC & 0xc0) | SEG7_TRIS_C;  //6,7ビット目をそのままに、ポートCの入出力設定
    TRISD = SEG7_TRIS_D;  
    LATC = 0;
    LATD = 0;
    LANP_T = 1;
    L_ENABLE = 0;
    BLANK = 1;
    
    for(int i = 0;i <= 15;i++){
        LCD_Clear();
        set_7seg(i);
        if(!check_pattern(i < 10 ? i : ALL_OFF)){    //10以上の数字を入れるとALL_OFFになるように
            return NG;
        }
    }
    /* ランプテスト入力を入れて、ALL_ONを確認 */
    LANP_T = 0;
    LCD_Clear();
    if(!check_pattern(ALL_ON))  return NG;
       
    LANP_T = 1;
    /* BLANK入力を入れて、ALL_OFFを確認 */
    BLANK = 0;
    LCD_Clear();
    if(!check_pattern(ALL_OFF))  return NG;
    
    BLANK = 1;
    /* ENABLE入力を入れたときに入力が変化しない */
    set_7seg(0);
    L_ENABLE = 1;
    LCD_String("\n");
    set_7seg(1);
    if(!check_pattern(0))  return NG;
    return OK;
}

int value_check(int value){
    unsigned char data = 0;
    if(Q_A) data |= 1;
    if(Q_B) data |= 2;
    if(Q_C) data |= 4;
    if(Q_D) data |= 8;
    
    LCD_Clear();
    LCD_HNumber(data,1);
    __delay_ms(300);
    if(MAX_MIN != 0) LCD_String("MAX/MIN");
    if(R_CK == 0)  LCD_String("RCO");
    
    if(value == 0){
        return (data == value) && (MAX_MIN == D_U) && (R_CK != D_U);
    } else if(value == 15){
        return (data == value) && (MAX_MIN != D_U) && (R_CK == D_U);
    } else {
        return (data == value) && (MAX_MIN == 0) && (R_CK != 0);
    }
}

void num_input(int value){
    DATA_A = ((value & 1) != 0);
    DATA_B = ((value & 2) != 0);
    DATA_C = ((value & 4) != 0);
    DATA_D = ((value & 8) != 0);
    __delay_ms(1);
}

int check_pattern(int value){
    const unsigned char patterns[] = {  //表示する数字と、その表示パターン
        0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d,
        0x7c, 0x07, 0x7f, 0x67, 0x7f, 0     //10，11はALL_ON、ALL_OFF
    };
    unsigned char data = 0;
    if(LED_A) data |= 0x01;
    if(LED_B) data |= 0x02;
    if(LED_C) data |= 0x04;
    if(LED_D) data |= 0x08;
    if(LED_E) data |= 0x10;
    if(LED_F) data |= 0x20;
    if(LED_G) data |= 0x40;
    LCD_String(" d ");
    LCD_HNumber(data, 2);
    __delay_ms(500);
    return (data == patterns[value]);
    
}

void set_7seg(int value){
    LCD_String("set ");
    LCD_Number(value);
    DATA_1 = ((value & 1) != 0);
    DATA_2 = ((value & 2) != 0);
    DATA_3 = ((value & 4) != 0);
    DATA_4 = ((value & 8) != 0);
    __delay_ms(1);
}

