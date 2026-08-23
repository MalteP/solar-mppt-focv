#include <Arduino.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "pins.h"
#include "adc.h"
#include "potentiometer.h"

// K factor in percent (Open circuit voltage to MPP voltage ratio)
#define K_PERCENT 80

// Delay between VOC measurements in milliseconds
#define MEAS_DELAY_MS 2000

// Delay between setting potentiometer and ADC capture in milliseconds.
// Must be long enough for the panel/input capacitance to settle after
// the potentiometer step change (particularly the VOC measurement,
// where the load is removed entirely) - verify with a scope if the
// panel or cabling changes.
#define ADC_DELAY_MS 25

// Minimum plausible VOC reading in millivolts. Below this the input is
// treated as disconnected/invalid and charging is left disabled instead of
// regulating off a bogus reading.
#define VOC_MIN_MV 1000

// Tolerance band (in millivolts) around VMPP within which the regulation
// loop leaves the potentiometer alone, to avoid dithering by +-1 step
// every cycle once the setpoint has been reached.
#define VMPP_DEADBAND_MV 20

// Set VMPP by using a lookup table instead of the regulation loop below.
// Both approaches derive VMPP from the same VOC measurement but reach the
// potentiometer position independently (table lookup vs. iterative
// search) - keep them in sync if the pot-to-voltage mapping changes.
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

  // Enable watchdog reset in case the main loop ever hangs.
  // ATtiny412 uses the newer WDT.CTRLA peripheral (CCP-protected write),
  // not the classic WDTCSR that <avr/wdt.h>'s wdt_enable() targets.
  _PROTECTED_WRITE(WDT.CTRLA, WDT_PERIOD_8KCLK_gc); // ~8.2s timeout
}

// Arduino main loop
void loop() {
  // Confirm to the watchdog that the loop is still running
  wdt_reset();

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

  // Only drive the potentiometer off a plausible VOC reading; otherwise
  // leave charging disabled (input likely disconnected or in the dark)
  if(voc >= VOC_MIN_MV) {
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

      // Regulate potentiometer steps, ignoring differences within the deadband
      if(vmpp_now > vmpp + VMPP_DEADBAND_MV) {
        if(steps < POT_STEPS) {
          steps++;
        }
      } else if(vmpp_now + VMPP_DEADBAND_MV < vmpp) {
        if(steps > 0) {
          steps--;
        }
      }
    }
#endif
  }

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