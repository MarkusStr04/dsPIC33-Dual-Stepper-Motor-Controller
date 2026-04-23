#ifndef ADC_H
#define ADC_H

void initAdc(void);

unsigned int scaleAdcToMotorPeriod(unsigned int adcValue);
 
#endif