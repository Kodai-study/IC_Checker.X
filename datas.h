#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H
#define PLUS_TIME 100
#define WAIT_TIME 500
#define _XTAL_FREQ 4000000

#include <xc.h> // include processor files - each processor file is guarded.  
#include "lcdlib_xc8_v03.h"

//#define CLOCK(x) {(x) = 0; __delay_ms(PLUS_TIME); (x) = 1;}
#define LAT(sym,num) LAT##sym ##bits.L##sym ##num 
#define P(sym,num) PORT##sym ##bits.R##sym ##num 
#define TRIS(sym,num) TRIS##sym ##bits.TRIS##sym ##num 

#define SW1 P(A,0)
#define SW1_TRIS TRIS(A,0)
#define SW2 P(A,1)
#define SW2_TRIS TRIS(A,1)

#define CLOCK(a)  a = 1; __delay_ms(PLUS_TIME); a = 0;
#define DOWN_CLOCK(a)  a = 0; __delay_ms(PLUS_TIME); a = 1;
#define T0_WAIT while(t0_flg == 0) {;} t0_flg = 0;

/*typedef enum __portnumber{
    A,
    B,
    C,
    D,
    E
}PORT_NUM;*/

typedef enum _now_mode{
    HOME, ALL_CHECK, CHECK_SELECT, RESULT, IS_TEST
};

typedef enum __checkstats{
    NO_CHECK, OK, NG, NOTFOUND
}CHECK_RESULT;

unsigned char bitPattern[8] = {
    0x01, 0x02, 0x04, 0x08, 
    0x10, 0x20 ,0x40, 0x80
};

extern _Bool t0_flg;
extern _Bool sw1_flg;
extern _Bool sw2_flg;
extern _Bool stop;
extern int mode;

CHECK_RESULT dff_Check(void);
CHECK_RESULT nand_check(int mode);
CHECK_RESULT count_check(void);

//void clock(PORT_NUM portnumber,int bitNum);
//void downClock(PORT_NUM portnumber,int bitNum);
CHECK_RESULT TMchecker();
void viewResults();
void usart_init();
void sw_check(void);
void single_check(int kind);
void current_over(void);
#endif	/* XC_HEADER_TEMPLATE_H */
