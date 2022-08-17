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

/* スイッチ1，2を監視して、新たに押されたらフラグを立てる */
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

/* 引数で受け取った種類のICを単体チェックする。 */
void single_check(int kind){
    CHECK_RESULT result = NO_CHECK;
    LCD_Clear();
    LCD_String(ic_names[select_item]);
    LCD_String(" : \nchecking...");
    LCD_CursorOn();
    //result = check_funcs[kind]();
    /* 選択されたICを単体チェックして、結果を変数に格納 */
    switch(kind){
        case 0 : result = opamp_check(1); break;
        case 1 : result = nand_check(1);    break;
        case 2 : result = nor_check(1);   break;
        default : result = ERROR;   break;
    }
    LCD_CursorOff();
    /* チェックの結果を表示 */
    switch(result){
        case OK : LCD_Locate(1,0); LCD_String("OK!!!       ");  break;      //OKならば2行目にOKを表示
        case NG : LCD_Clear(); LCD_String(ic_names[select_item]);
            LCD_String(" : NG!!\nｹｯﾃｲ : ｻｲｼｹﾝ");  break;             //NGなら、2行目に次の指示を表示
        case ERROR : LCD_Locate(1,0);  LCD_String("ERROR...");  break;
        default : break;
    }
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

/* 電流が基準値を超えたときに警告を表示してプログラムを終了する。 */
void current_over(){
    LCD_Clear();
    SW_TRIS = 0;
    POWER_SW = 0;
    LCD_String("ｶﾃﾞﾝﾘｭｳｦｹﾝｼｭﾂ\nｾﾂｿﾞｸｦｶｸﾆﾝｼﾃ ﾘｾｯﾄ");
    TRISAbits.TRISA2 = 0;
    
    LATB = 0;
    while(1){       //他のプログラムへは移行しない
        __delay_ms(125);
        LATAbits.LA2 = ~LATAbits.LA2;   //赤色LEDを点滅
    }
}

void cancel(){
    LCD_Clear();
    LCD_String("ｷｬﾝｾﾙｼﾏｼﾀ...");
    __delay_ms(700);
    return;
}

/* sw3、戻るボタンが押された時に、前のモードに戻る処理 */
void mode_change(){
    //HOME, ALL_CHECK, CHECK_SELECT, ALL_RESULT, SINGLE_RESULT, SINGLE_TEST
    switch(now_mode){
        case ALL_CHECK    : cancel();
        case CHECK_SELECT :                 // 全体チェック、選択画面、全体チェックの時はホーム画面に戻る
        case ALL_RESULT   : now_mode = HOME; break; 
        case SINGLE_TEST  : cancel();
        case SINGLE_RESULT: now_mode = CHECK_SELECT; break; //単体テスト中、単体テストの結果表示の時は項目選択画面に戻る
        default : break;
    }
}

void all_check(){
    LCD_Clear();
    LCD_String("ﾁｪｯｸﾁｭｳ...");
    TXREG = 0xff;
    
    TXREG = 0xfe;
}