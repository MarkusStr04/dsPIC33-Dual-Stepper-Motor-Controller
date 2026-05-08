#include "intreruperi.h"
#include "config.h"
#include "timere.h"


void initINT0(void){

_TRISB7 = 1;
INT2CONbits.INT0EP = 1;
_INT0IF =0;
_INTOIE = 1;


}


void __attribute__ ((interrupt, no_auto_psv)) _INT0Interrupt(void) {

     if (g_motor2Period > PR2_MIN)
    {
        g_motor2Period -= MOTOR2_SPEED_STEP;
 
        /* Clampare: nu lasam sa scada sub minimul fizic */
        if (g_motor2Period < PR2_MIN)
            g_motor2Period = PR2_MIN;
    }
    else
    {
        /* Am atins viteza maxima => resetam la viteza minima (ciclic) */
        g_motor2Period = PR2_MAX;
    }
 
    _INT0IF = 0;    // resetam flagul la finalul 
                 


    INT0IF = 0;

}