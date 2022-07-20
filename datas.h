#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#define PLUS_TIME 5
#define WAIT_TIME 500
#define _XTAL_FREQ 1000000
#define CLOCK(x) {(x) = 0; __delay_ms(PLUS_TIME); (x) = 1;}
#define LAT(sym,num) LAT##sym ##bits.L ##sym ##num 
#define PORT(sym,num) PORT##sym ##bits.R ##sym ##num 
#define TRIS(sym,num) TRIS##sym ##bits.TRIS ##sym ##num 
typedef enum __portnumber{
    A,
    B,
    C,
    D,
    E
}PORT;

typedef enum __checkstats{
    OK, NG, NOTFOUND
}CHECK_RESULT;

unsigned char bitPattern[8] = {
    0x01, 0x02, 0x04, 0x08, 
    0x10, 0x20 ,0x40, 0x80
};

void dff_Check(void);

void clock(PORT portnumber,int bitNum);
void downClock(PORT portnumber,int bitNum);
#endif	/* XC_HEADER_TEMPLATE_H */
