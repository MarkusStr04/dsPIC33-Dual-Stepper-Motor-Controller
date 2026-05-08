#ifndef CONFIG_H
#define CONFIG_H
 
#include "p33Fxxxx.h"

// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching and Configure
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF);
      // 0=1:1, 1=1:8, 2=1:64, 3=1:256
#define PR1_VALUE       31250   // comutare LED la 0.2s (2 apeluri = 0.4s)
//#define T3_PRESCALER 3 // 1:256
#define PR3_VALUE 39062   // Timer3 expira la 0.25s

#define PR2_MAX 60000   //Viteza max motor
#define PR2_MIN 5000    //VIteza min motor
#define PR2_STEP 5000   //Pasul de crestere a vitezei motorului


extern volatile unsigned int  g_adcValue;       // valoarea bruta ADC (0..4095 pentru 12 biti)
extern volatile unsigned char g_adcReady;        // flag: conversie noua disponibila
extern volatile unsigned int  g_motor1Period;    // perioada pasului motor 1 (din ADC)
extern volatile unsigned int  g_motor2Period;

#endif 
