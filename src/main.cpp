#include <Arduino.h>
#include "pins.h"
#include "adc.h"
#include "potentiometer.h"

// K factor in percent (Open circuit voltage to MPP voltage ratio)
#define K_PERCENT 80

// Delay between VOC measurements in milliseconds
#define MEAS_DELAY_MS 2000

// Delay between setting potentiometer and ADC capture in milliseconds
#define ADC_DELAY_MS 25

// Set VMPP by using a lookup table
// #define ENABLE_VMPP_LOOKUP

// Enable debug output on LED / TX pin
// #define ENABLE_DEBUG

// Variables
uint16_t voc, vmpp;
#ifndef ENABLE_VMPP_LOOKUP
uint16_t vmpp_now;
uint16_t steps = POT_STEPS / 2;
#endif

// Arduino setup function
void setup() {
#ifndef ENABLE_DEBUG
  pinMode(PIN_LED, OUTPUT);
#else
  Serial.begin(9600, (SERIAL_8N1 | SERIAL_TX_ONLY));
#endif
  ADC_init();
  POT_init();
}

// Arduino main loop
void loop() {
#ifndef ENABLE_DEBUG
  // Turn off LED
  digitalWrite(PIN_LED, LOW);
#endif

  // Set max. MPP voltage to disable charging
  POT_set_steps(0);
  delay(ADC_DELAY_MS);

  // Read solar panel open circuit voltage
  voc = ADC_read_voltage();

  // Calculate maximum power point voltage (VMPP)
  vmpp = (uint16_t)(((uint32_t)voc * K_PERCENT) / 100);

#ifndef ENABLE_DEBUG
  // Enable LED again, delay until next measurement
  digitalWrite(PIN_LED, HIGH);
#endif

#ifdef ENABLE_VMPP_LOOKUP
  // Set potentiometer to calculated MPP voltage
  POT_set_voltage(vmpp);
#else
  // Set MPP voltage by regulation loop
  for(uint16_t i = 0; i < (MEAS_DELAY_MS / ADC_DELAY_MS); i++) {

    // Set current potentiometer steps
    POT_set_steps(steps);
    delay(ADC_DELAY_MS);

    // Measure actual voltage
    vmpp_now = ADC_read_voltage();

    // Regulate potentiometer steps
    if(vmpp_now > vmpp) {
      if(steps < POT_STEPS) {
        steps++;
      }
    } else {
      if(steps > 0) {
        steps--;
      }
    }
  }
#endif

#ifdef ENABLE_DEBUG
  // Print VOC and VMPP if debug enabled
  Serial.print(voc);
  Serial.print(";");
  Serial.print(vmpp);
  #ifndef ENABLE_VMPP_LOOKUP
  Serial.print(";");
  Serial.print(steps);
  #endif
  Serial.println(";");
  Serial.flush();
#endif

  delay(MEAS_DELAY_MS);
}