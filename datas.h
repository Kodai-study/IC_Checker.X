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

void dff_Check(void);

void clock(PORT portnumber,int bitNum);
#endif	/* XC_HEADER_TEMPLATE_H */
