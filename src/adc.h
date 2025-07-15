#ifndef ADC_H
#define ADC_H

// Parameters for analog to digital conversion
#define ADC_MAX_MV 27800 // Input voltage (in mV) at max. ADC value, here: 18K / 3.3K voltage divider and 4.3V ADC ref
#define ADC_STEPS 0x3FF  // Total number of ADC steps (10bit)

void ADC_init();
uint16_t ADC_read_voltage();

#endif