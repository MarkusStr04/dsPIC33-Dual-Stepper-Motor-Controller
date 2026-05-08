#include "config.h"
#include "adc.h"

void initAdc(void)
{
 AD1CON1bits.ADON=0;
 AD1CON1bits.AD12B = 1; // conversie AD pe 12 bi?i
 AD1CON1bits.SSRC = 2; // timerul 3 starteaz� conversia
 AD1CON1bits.ASAM = 1; // �ncepe e?antionarea dup� terminarea unei conversii
 AD1CON2bits.CSCNA = 1; // scaneaz� intr�rile pe CH0+

 AD1CON3bits.ADRC = 0; // folose?te ceasul de magistral�
 AD1CON3bits.ADCS = 63; // Tad=Tcy*(adcs+1)=25ns*64=1.6us

 AD1CSSLbits.CSS5 = 1; // este scanat� intrarea analogic� AN4(RB2)
 AD1PCFGL=0xFFFF; // seteaz� pinii portului ADC1 ca fiind digitali
 AD1PCFGLbits.PCFG5 = 0; // seteaz� pinul AN4(RB2) ca intrare analogic�

 _AD1IF = 0; // reseteaza flag-ul �ntreruperii convertorului AD
 _AD1IE = 1; // permite �ntreruperea convertorului AD

 AD1CON1bits.ADON = 1; // porne?te convertorul AD
}
	
unsigned int scaleAdcToMotorPeriod(unsigned int adcValue)
{
    unsigned long range;
    unsigned int  period;
 
    range  = (unsigned long)adcValue * (PR2_MAX - PR2_MIN);
    period = PR2_MAX - (unsigned int)(range / 4095UL);
 
    return period;
}
 


void __attribute__((interrupt, no_auto_psv)) _ADC1Interrupt(void)
{
	static unsigned char convCount = 0; // contor static

    convCount++;

	 if (convCount >= 2 )  // ADC_SAMPLE_COUNT = 2 -> la 2 * 0.25s = 0.5s
    {
        convCount    = 0;
        g_adcValue   = ADC1BUF0;        // citim rezultatul conversiei (0..4095)
        g_adcReady   = 1;               // semnalam bucla principala ca avem date noi
    }
 
    _AD1IF = 0; 

}