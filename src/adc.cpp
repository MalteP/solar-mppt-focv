#include <Arduino.h>
#include "pins.h"
#include "adc.h"

// Initialize analog to digital converter
void ADC_init() {
  pinMode(ADC_INP_VOLT, INPUT);      // Set pin to input
  analogReference(INTERNAL4V3);      // Set 4.3V reference
  ADC0.CTRLD = ADC_INITDLY_DLY64_gc; // 64 cycles initial delay
  ADC0.CTRLB = ADC_SAMPNUM_ACC64_gc; // Accumulate 64 samples
}

// Read ADC input and calculate voltage
uint16_t ADC_read_voltage() {
  uint16_t adc;
  // Read ADC
  adc = (uint16_t)analogRead(ADC_INP_VOLT) / 64;
  // Calculate voltage and return result
  return (uint16_t)(((uint32_t)adc * ADC_MAX_MV) / ADC_STEPS);
}