#include "config.h"
#include "timer.h"

void initTimer1(void)
{
    T1CON = 0;                  // resetam registrul de control
 
    T1CONbits.TCKPS = 3;

}

void initTimer2(unsigned int period){



}

void initTimer3(void){



}