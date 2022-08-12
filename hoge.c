/*
 * File:   hoge.c
 * Author: gorin
 *
 * Created on 2022/08/12, 9:59
 */


#include <xc.h>
#include "datas.h"
int hoge_count = 0;

void hoge(void) {
    T0_WAIT;
        sw_check();
        if(++hoge_count >= 500){
            hoge_count = 0;
            LATAbits.LA3 = ~LATAbits.LA3;
        }
        if(sw1_flg){
            sw1_flg = 0;
            LCD_String("sw1");
        } 
        if(sw2_flg){
            sw2_flg = 0;
            LCD_String("sw2");
        }
}
