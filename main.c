#include "config.h"

volatile unsigned int  g_adcValue     = 0;
volatile unsigned char g_adcReady     = 0;
volatile unsigned int  g_motor1Period = MOTOR_PR_MAX;
volatile unsigned int  g_motor2Period = MOTOR_PR_MAX;

static const unsigned char stepSequence[4][4] = {
    {0, 1, 1, 1},
    {1, 0, 1, 1},
    {1, 1, 0, 1},
    {1, 1, 1, 0}
};

static unsigned char motor1Step = 0;
static unsigned char motor2Step = 0;

void initPLL(void)
{
// Configure PLL prescaler, PLL postscaler, PLL divisor
 _PLLDIV = 41; // M = 43 FRC
 _PLLPRE = 0; // N1 = 2
 _PLLPOST = 0; // N2 = 2
// Initiate Clock Switch to Internal FRC with PLL (NOSC = 0b001)
 __builtin_write_OSCCONH(0x01); // FRC
 __builtin_write_OSCCONL(0x01);
// Wait for Clock switch to occur
 while (OSCCONbits.COSC != 0b001); // FRC
// Wait for PLL to lock
 while(OSCCONbits.LOCK!=1) {};
}

static void applyStep(unsigned char stepIndex, unsigned char motorId)
{
    if (motorId == 1) {
        MOTOR1_A = stepSequence[stepIndex][0];
        MOTOR1_B = stepSequence[stepIndex][1];
        MOTOR1_C = stepSequence[stepIndex][2];
        MOTOR1_D = stepSequence[stepIndex][3];
    } else {
        MOTOR2_A = stepSequence[stepIndex][0];
        MOTOR2_B = stepSequence[stepIndex][1];
        MOTOR2_C = stepSequence[stepIndex][2];
        MOTOR2_D = stepSequence[stepIndex][3];
    }
}

static void initStepper(void)
{
    TRIS_M1_A = 0; TRIS_M1_B = 0; TRIS_M1_C = 0; TRIS_M1_D = 0;
    TRIS_M2_A = 0; TRIS_M2_B = 0; TRIS_M2_C = 0; TRIS_M2_D = 0;

    motor1Step = 0;
    motor2Step = 0;
    applyStep(motor1Step, 1);
    applyStep(motor2Step, 2);
}

static void initTimer1(void)
{
    T1CON = 0;
    T1CONbits.TCKPS = 3;        // Prescaler 1:256
    PR1 = PR1_VALUE;            // Perioada 0.2s
    _T1IF = 0;
    _T1IE = 1;                  // Activare intrerupere T1
    T1CONbits.TON = 1;
}

// Timer 2 se ocupa de controlul vitezei Motorului 1
static void initTimer2(unsigned int period)
{
    T2CON = 0;
    T2CONbits.TCKPS = 3;        // Prescaler 1:256
    PR2 = period;
    _T2IF = 0;
    _T2IE = 1;                  // Activare intrerupere T2
    T2CONbits.TON = 1;
}

// Timer 3 temporizeaza trigger-ul ADC la 0.25s
static void initTimer3(void)
{
    T3CON = 0;
    T3CONbits.TCKPS = 3;        // Prescaler 1:256
    PR3 = PR3_VALUE;            // Perioada 0.25s
    T3CONbits.TON = 1;         
}

// Timer 4 se ocupa de controlul vitezei Motorului 2
static void initTimer4(unsigned int period)
{
    T4CON = 0;
    T4CONbits.TCKPS = 3;        // Prescaler 1:256
    PR4 = period;
    _T4IF = 0;
    _T4IE = 1;                 
    T4CONbits.TON = 1;
}

static void setTimer2Period(unsigned int period) 
{
    if (period < MOTOR_PR_MIN) period = MOTOR_PR_MIN;
    if (period > MOTOR_PR_MAX) period = MOTOR_PR_MAX;
    PR2  = period;
}

static void setTimer4Period(unsigned int period) 
{
    if (period < MOTOR_PR_MIN) period = MOTOR_PR_MIN;
    if (period > MOTOR_PR_MAX) period = MOTOR_PR_MAX;
    PR4  = period;
}

void initAdc1(void)
{
 AD1CON1bits.AD12B = 1; // conversie AD pe 12 biți
 AD1CON1bits.SSRC = 2; // timerul 3 startează conversia
 AD1CON1bits.ASAM = 1; // începe eșantionarea după terminarea unei conversii
 AD1CON2bits.CSCNA = 1; // scanează intrările pe CH0+

 AD1CON3bits.ADRC = 0; // folosește ceasul de magistrală
 AD1CON3bits.ADCS = 63; // Tad=Tcy*(adcs+1)=25ns*64=1.6us

 
 AD1CSSLbits.CSS5 = 1; // este scanată intrarea analogică AN5(RB3)
 AD1PCFGL=0xFFFF; // setează pinii portului ADC1 ca fiind digitali
 AD1PCFGLbits.PCFG5 = 0; // setează pinul AN5(RB3) ca intrare analogică

 _AD1IF = 0; // reseteaza flag-ul întreruperii convertorului AD
 _AD1IE = 1; // permite întreruperea convertorului AD

 AD1CON1bits.ADON = 1; // pornește convertorul AD
} 
// Scalare valoare ADC (0..4095) in perioada Timer (MOTOR_PR_MIN .. MOTOR_PR_MAX)
static unsigned int scaleAdcToMotorPeriod(unsigned int adcValue) 
{
    unsigned long range  = (unsigned long)adcValue * (MOTOR_PR_MAX - MOTOR_PR_MIN);
    unsigned int  period = MOTOR_PR_MAX - (unsigned int)(range / 4095UL);
    return period;
}

// INTRERUPERE EXTERNA INT0 pentru Butonul S2
static void initINT0(void)
{
    _TRISB7 = 1;               // Pinul RB7 setat ca intrare (Buton S2)
    INTCON2bits.INT0EP = 1;     // Detectie pe front cazator (apasare buton)
    _INT0IF = 0;
    _INT0IE = 1;                // Activare intrerupere INT0
}

// ISR
//Led
void __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void)
{
    _RB2  ^= 1;                 // Inversare stare pin LED
    _T1IF  = 0;                 // Stergere flag intrerupere
}

//motor1
void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    motor1Step = (motor1Step + 1) & 0x03; // Incrementare circulara 0..3
    applyStep(motor1Step, 1);
    _T2IF = 0;
}

//motor2
void __attribute__((interrupt, no_auto_psv)) _T4Interrupt(void)
{
    motor2Step = (motor2Step + 1) & 0x03; // Incrementare circulara 0..3
    applyStep(motor2Step, 2);
    _T4IF = 0;
}

// Citire valoare de pe AN5 la fiecare 0.5s (2 * 0.25s)
void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
    static unsigned char Count = 0;
    Count++;

    if (Count >= 2) {           // 2 conversii * 0.25s de la Timer3 = 0.5 secunde
        Count  = 0;
        g_adcValue = ADC1BUF0;  // Citire rezultat conversie din buffer
        g_adcReady = 1;         // Semnalizare catre bucla principala
    }
    _AD1IF = 0;
}

// INT0 – Apasare buton S2 Creste viteza Motorului 2 
void __attribute__((interrupt, no_auto_psv)) _INT0Interrupt(void)
{
    // Micsorarea perioadei inseamna cresterea frecventei (vitezei)
    if (g_motor2Period > MOTOR_PR_MIN) {
        g_motor2Period -= MOTOR_PR_STEP;
    } else {
        g_motor2Period = MOTOR_PR_MAX;   // Reset la viteza minima
    }

    setTimer4Period(g_motor2Period);    // Actualizare registru PR4
    _INT0IF = 0;
}

int main(void)
{
    initPLL();              
    initStepper();          // Configurare pini motoare si faza initiala
    _TRISB2 = 0;            
    _RB2    = 0;           
    initTimer1();           // Pornire Timer LED (0.2s)
    initTimer2(MOTOR_PR_MAX); // Motor 1 porneste initial la viteza minima
    initTimer3();           // Timer pentru trigger ADC (0.25s)
    initTimer4(g_motor2Period); // Motor 2 porneste initial la viteza minima
    initAdc();              // Activare modul ADC pe AN5
    initINT0();             // Activare buton S2 pe INT0

    while (1)
    {
        if (g_adcReady)
        {
            g_adcReady     = 0;
            g_motor1Period = scaleAdcToMotorPeriod(g_adcValue); // Scalare valoare noua
            setTimer2Period(g_motor1Period);                  // Aplicare perioada pe Motor 1
        }
    }
}