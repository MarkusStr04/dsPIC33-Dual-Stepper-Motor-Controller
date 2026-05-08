#include "config.h"
#include "timer.h"

void initTimer1(void)
{
    T1CON = 0;                  // resetam registrul de control
 
    T1CONbits.TCKPS = 3;
    PR1 = PR1_VALUE;            // setam perioada pentru Timer1 (0.2s)
    _T1IF = 0;                  // resetam flagul de intrerupere
    _T1IE = 1;

    T1CONbits.TON = 1;          // pornim Timer1
}

void initTimer2(unsigned int period){

T2CON = 0;
T2CONbits.TCKPS = 3; // prescaler 1:256
PR2 = period; // setam perioada pentru Timer2 (din ADC)
_T2IF = 0; // reset flag de intrerupere
_T2IE = 1; // permitem intreruperea pentru Timer2
T2CONbits.TON = 1; // pornim Timer2
PR2 =period;

}

void initTimer3(void){

T3CON = 0;
T3CONbits.TCKPS = 3; // prescaler 1:256
PR3 = PR3_VALUE; // setam perioada pentru Timer3 (0.25s)


T3CONbits.TON = 1; // pornim Timer3


}
void setTimer2Period(unsigned int period)
{
    
    if (period < PR2_MIN) period = PR2_MIN;
    if (period > PR2_MAX) period = PR2_MAX;
 
    TMR2 = 0;   // resetam contorul pentru a aplica noua perioada imediat
    PR2  = period;
}

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    stepperMotor1();   // avanseaza motor 1 cu un pas (viteza controlata de PR2)
    stepperMotor2();   // avanseaza motor 2 cu un pas (viteza controlata intern)
 
    _T2IF = 0;
}