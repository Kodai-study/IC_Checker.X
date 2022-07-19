#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  
#define PLUS_TIME 5
#define WAIT_TIME 500
#define _XTAL_FREQ 1000000
#define CLOCK(x) {(x) = 0; __delay_ms(PLUS_TIME); (x) = 1;}
typedef enum __portnumber{
    A,
    B,
    C,
    D,
    E
}PORT;
unsigned char bitPattern[8] = {
    0x01, 0x02, 0x04, 0x08, 
    0x10, 0x20 ,0x40, 0x80
};

void dff_Check(void);

void clock(PORT portnumber,int bitNum);
#endif	/* XC_HEADER_TEMPLATE_H */
