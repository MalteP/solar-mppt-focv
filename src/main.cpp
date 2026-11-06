#include <Arduino.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include "pins.h"
#include "adc.h"
#include "potentiometer.h"

// K factor in percent (Open circuit voltage to MPP voltage ratio)
#define K_PERCENT 80

// Period between VOC measurement cycles in milliseconds
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
  const uint32_t cycle_start = millis();

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
    // Search until the target is bracketed by two adjacent positions.
    // Hold the closer position for the remainder of this measurement period.
    int32_t previous_error = 0;
    uint16_t previous_steps = steps;
    bool have_previous = false;
    POT_set_steps(steps);
    while((uint32_t)(millis() - cycle_start) + ADC_DELAY_MS < MEAS_DELAY_MS) {
      delay(ADC_DELAY_MS);
      vmpp_now = ADC_read_voltage();
      const int32_t error = (int32_t)vmpp_now - vmpp;
      const uint32_t magnitude = error < 0 ? -error : error;

      if(have_previous && ((error < 0 && previous_error > 0) ||
                           (error > 0 && previous_error < 0))) {
        const uint32_t previous_magnitude = previous_error < 0 ? -previous_error : previous_error;
        if(previous_magnitude <= magnitude) {
          steps = previous_steps;
          POT_set_steps(steps);
        }
        break;
      }
      if(magnitude <= VMPP_DEADBAND_MV) {
        break;
      }
      if((error > 0 && steps == POT_STEPS) || (error < 0 && steps == 0)) {
        break;
      }
      // Do not change position unless there is time to settle and measure it.
      if((uint32_t)(millis() - cycle_start) + ADC_DELAY_MS >= MEAS_DELAY_MS) {
        break;
      }
      previous_error = error;
      previous_steps = steps;
      have_previous = true;
      steps = error > 0 ? steps + 1 : steps - 1;
      POT_set_steps(steps);
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

  const uint32_t elapsed = millis() - cycle_start;
  if(elapsed < MEAS_DELAY_MS) {
    delay(MEAS_DELAY_MS - elapsed);
  }
}
