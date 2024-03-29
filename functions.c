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
//CHECK_RESULT results[10] = {NO_CHECK};  //チェック項目の結果を表す
CHECK_RESULT results[11] = {NO_CHECK}; ///デバッグ用

const char* mode_names[] = {    //チェックするモードを表示する文字列
    " ALLﾁｪｯｸ",
    " ﾀﾝﾀｲﾁｪｯｸ"
};

static const char dot = 0xa5;     //LCDで、選択項目を表すドットの文字コード
static const char blank = 0xa0;   //LCDで、空白を表示させるデータ
static const char allow = 0x7e;   //右矢印〃

int ng_count = 0;                       //チェックが失敗した項目の数をカウント
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
    if(kind == 2){
        LCD_String("3ﾀﾝｼREG"); 
    } else if(kind == 4){
        LCD_String("7ｾｸﾞLED"); 
    } else{
        LCD_String(ic_names[kind]);
    }
    LCD_String(" check...\n");
    LCD_CursorOn();
    //result = check_funcs[kind]();
    /* 選択されたICを単体チェックして、結果を変数に格納 */
    if(kind > 10){
        LCD_String("ERROR...");
        return ;
    }
    switch(kind){
        case 0 : result = seg7_decode(1); break;
        case 1 : result = count2_check(1);    break;
        case 2 : result = reg_check(1);   break;
        
        default : rx_flg = 0;  TXREG = kind; 
        WAIT_RECEIVE(SINGLE_TEST);
           if((rx_buf >> 4) == kind) {
                result = rx_buf & 0x0f;
            } else { 
               result = ERROR;
            }
            rx_buf = 0;  break;
    }
    
    LCD_CursorOff();
    /* チェックの結果を表示 */
    switch(result){
        case OK : LED_BLUE = 1; LED_GREEN = 0;LED_RED = 1; 
            LCD_Locate(1,0); LCD_String("OK!!!       ");  break;      //OKならば2行目にOKを表示
        case NG : LCD_Clear(); LCD_String(ic_names[select_item]);
            LED_BLUE = 1; LED_GREEN = 1;LED_RED = 0; 
            LCD_String(" : NG!!");
            LCD_Locate(0,0);
            LCD_String("ｹｯﾃｲ : ｻｲｼｹﾝ      ");  break;  
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

/* 電流が基準値を超えたときに警告を表示してプログラムを終了する。 */
void current_over(){
    LCD_Clear();
    SW_TRIS = 0;
    POWER_SW = 0;
    LCD_String("ｶﾃﾞﾝﾘｭｳｦｹﾝｼｭﾂ\nｾﾂｿﾞｸｦｶｸﾆﾝｼﾃ ﾘｾｯﾄ");
    
    LED_BLUE = 1;
    LED_GREEN = 1;
    LED_RED = 0;
    while(1){       //他のプログラムへは移行しない
        __delay_ms(125);
        LED_RED = ~LED_RED;   //赤色LEDを点滅
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
        case ALL_RESULT   : now_mode = HOME; LCD_Clear(); break;
        
        case SINGLE_TEST  : cancel();
        case SINGLE_RESULT: now_mode = CHECK_SELECT; LCD_Clear();break; //単体テスト中、単体テストの結果表示の時は項目選択画面に戻る
        case RETRY        : cancel(); now_mode = ALL_RESULT; LCD_Clear(); break;      
        default : break;
    }
}

void all_check(){
    LCD_Clear();
    LCD_String("ﾁｪｯｸﾁｭｳ...");
    rx_flg = 0;
    TXREG = 0xff;
    results[0] = seg7_decode(0);
    results[2] = reg_check(0);
    
    WAIT_RECEIVE(ALL_CHECK);
    
    if(rx_buf == 0xaa){
        LCD_Clear();
        LCD_String("ICｦ ﾄﾘｶｴﾃｸﾀﾞｻｲ\nｹｯﾃｲ : ｽｽﾑ");
        while(sw2_flg == 0){ T0_WAIT; sw_check();  }
        LCD_Clear();
        LCD_String("ﾁｪｯｸﾁｭｳ...");
    } else {
        LCD_String("ERROR...\nﾅﾆｶｷｰｦ ｵｼﾃｸﾀﾞｻｲ");
        while(sw2_flg == 0 && sw1_flg == 0 && now_mode == ALL_CHECK) {
            T0_WAIT; sw_check();  
        }
        sw2_flg = 0;
        sw1_flg = 0;
        now_mode = HOME;
        return;
    }
    rx_flg = 0;
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
                LCD_String("NGｲﾁﾗﾝ ｺｽｳ : \n");
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
                LCD_Character(allow);       //まだ下に項目があるときは、矢印を表示
            }
                /* 最終項目では、下の段を選択 */
        } else if(cur == ng_count){
            index = next(index);
            LCD_Locate(0,0);
            LCD_Character(blank);
            LCD_Locate(1,0);
            LCD_Character(dot);
        }
    }
}


/* 単体チェックか、全てをチェックするかを選択する */
void menu_mode(){
    LCD_Clear();        
    static unsigned int cur = 0;     //選択モードがどちらかを格納
    SW1_TRIS = 1;           
    SW2_TRIS = 1;
    
    /* 初期は全てチェックモード */       
    LCD_String(mode_names[0]);
    LCD_String("\n");
    LCD_String(mode_names[1]);
    LCD_Locate(cur,0);
    LCD_Character(dot);
    while(now_mode == HOME){
        T0_WAIT
        sw_check();     //1msごとに、スイッチが押されたかどうかチェック
        /* スイッチ1が押されたら、選択モードを変更 */
        if(sw1_flg != 0){       
            sw1_flg = 0;
            LCD_Locate(cur,0);
            LCD_Character(blank);
            cur = !cur;
            LCD_Locate(cur,0);
            LCD_Character(dot);
        }
        /* スイッチ2が押されたら、選択されたモードに移行 */
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
            LCD_Character(allow);       //まだ下に項目があるときは、矢印を表示
        }
                /* 最終項目では、下の段を選択 */
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
        LCD_String("3ﾀﾝｼREG"); 
    } else if(kind == 4){
        LCD_String("7ｾｸﾞLED"); 
    } else{
        LCD_String(ic_names[kind]);
    }
    LCD_String(" check...\n");
    //result = check_funcs[kind]();
    /* 選択されたICを単体チェックして、結果を変数に格納 */
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
    /* チェックの結果を表示 */
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
    CMCONbits.CIS = 0;      //RA0,1をコンパレータの入力にする
    CMCONbits.C1INV = 1;    //C1OUTの値を反転(CINのほうが大きい時にCOUT=1)
    
    PIE2bits.CMIE = 1;      //COUTの値が変わったときに割り込み
    CMCONbits.CM = 0b110;
}