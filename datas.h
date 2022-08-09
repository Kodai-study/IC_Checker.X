#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#define PLUS_TIME 100
#define WAIT_TIME 500
#define _XTAL_FREQ 4000000
//#define CLOCK(x) {(x) = 0; __delay_ms(PLUS_TIME); (x) = 1;}
#define LAT(sym,num) LAT##sym ##bits.L##sym ##num 
#define P(sym,num) PORT##sym ##bits.R##sym##num 
#define TRIS(sym,num) TRIS##sym ##bits.TRIS##sym ##num 
#define CLOCK(a)  a = 1; __delay_ms(PLUS_TIME); a = 0;
#define DOWN_CLOCK(a)  a = 0; __delay_ms(PLUS_TIME); a = 1;
typedef enum __portnumber{
    A,
    B,
    C,
    D,
    E
}PORT;

typedef enum __checkstats{
    NO_CHECK, OK, NG, NOTFOUND
}CHECK_RESULT;

unsigned char bitPattern[8] = {
    0x01, 0x02, 0x04, 0x08, 
    0x10, 0x20 ,0x40, 0x80
};

CHECK_RESULT dff_Check(void);
CHECK_RESULT nand_check(int mode);
CHECK_RESULT count_check(void);

void clock(PORT portnumber,int bitNum);
void downClock(PORT portnumber,int bitNum);
CHECK_RESULT TMchecker();
void viewResults();
#endif	/* XC_HEADER_TEMPLATE_H */
