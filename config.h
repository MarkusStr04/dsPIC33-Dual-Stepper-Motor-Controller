#ifndef CONFIG_H
#define CONFIG_H

#include "p33Fxxxx.h"

// Select Internal FRC at POR
_FOSCSEL(FNOSC_FRC);
// Enable Clock Switching and Configure
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF);

// Valori PR calculate pentru Fcy = 40 MIPS (Prescaler 1:256)
#define PR1_VALUE   31250   // Timer1: 0.2s per overflow (Toggle LED, perioada totala 0.4s)
#define PR3_VALUE   39062   // Timer3: 0.25s per overflow (2 intreruperi ADC = 0.5s)

// Limite pentru perioadele motoarelor (PR2 pentru Motor 1, PR4 pentru Motor 2)
#define MOTOR_PR_MAX     60000   // Viteza minima (perioada mare)
#define MOTOR_PR_MIN      5000   // Viteza maxima (perioada mica)
#define MOTOR_PR_STEP     5000   // Pasul de modificare la apasarea butonului

extern volatile unsigned int  g_adcValue;      
extern volatile unsigned char g_adcReady;       
extern volatile unsigned int  g_motor1Period;   
extern volatile unsigned int  g_motor2Period;   

// Alocare pini
#define MOTOR1_A    _RB8
#define MOTOR1_B    _RB9
#define MOTOR1_C    _RB10
#define MOTOR1_D    _RB11

#define TRIS_M1_A   _TRISB8
#define TRIS_M1_B   _TRISB9
#define TRIS_M1_C   _TRISB10
#define TRIS_M1_D   _TRISB11

#define MOTOR2_A    _RB12
#define MOTOR2_B    _RB13
#define MOTOR2_C    _RB14
#define MOTOR2_D    _RB6

#define TRIS_M2_A   _TRISB12
#define TRIS_M2_B   _TRISB13
#define TRIS_M2_C   _TRISB14
#define TRIS_M2_D   _TRISB6

void initPLL(void);

#endif