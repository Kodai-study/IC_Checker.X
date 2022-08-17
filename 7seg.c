/*
 * File:   7seg.c
 * Author: gorin
 *
 * Created on 2022/08/17, 15:30
 */
#include "datas.h"

#define RCL LAT(D,2)
#define INPUT LAT(D,3)
#define CLK LAT(D,4)

#define RCL_TRIS   TRIS(D,2)
#define INPUT_TRIS TRIS(D,3)
#define CLK_TRIS   TRIS(D,4)

CHECK_RESULT seg7_check(int mode){
    RCL_TRIS = 0;
    INPUT_TRIS = 0;
    CLK_TRIS = 0;
    INPUT = 0;
    for(int i = 0;i < 8;i++){
        CLOCK(CLK);
    }
    CLOCK(RCL);
    INPUT = 1;
    CLOCK(CLK);
}
        

