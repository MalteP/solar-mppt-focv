#include <Arduino.h>
#include "pins.h"
#include "potentiometer.h"
#include "vmpp.h"

// Initialize MCP4151 digital potentiometer ports
void POT_init() {
  pinMode(PIN_POT_CS, OUTPUT);
  digitalWrite(PIN_POT_CS, HIGH);
  pinMode(PIN_POT_DA, OUTPUT);
  pinMode(PIN_POT_CLK, OUTPUT);
  digitalWrite(PIN_POT_CLK, LOW);
}

// Set potentiometer position for given MPP voltage
void POT_set_voltage(uint16_t vmpp) {
  uint16_t steps;
  for(steps = 0; steps < POT_STEPS; steps++) {
    if(vmpp >= pgm_read_word(vmpp_table + steps)) {
      break;
    }
  }
  POT_set_steps(steps);
}

// Set potentiometer to specific position
void POT_set_steps(uint16_t steps) {
  // Address 0h (wiper 0), Command 00 (write data)
  uint16_t data = 0x0000;

  // Valid wiper positions are 0..POT_STEPS; clamp out-of-range callers
  if(steps > POT_STEPS) {
    steps = POT_STEPS;
  }

  // Wiper position is 9 bits long
  data |= steps & 0x01FF;

  // Select chip by pulling /CS low
  digitalWrite(PIN_POT_CS, LOW);

  // Shift out 16 bits of data
  for(uint8_t i = 0; i < 16; i++) {
    // Write MSB
    digitalWrite(PIN_POT_DA, ((data & 0x8000) ? HIGH : LOW));
    // Shift data to the left
    data = data << 1;
    // Pulse clock pin
    digitalWrite(PIN_POT_CLK, HIGH);
    digitalWrite(PIN_POT_CLK, LOW);
  }

  // Deselect chip by pulling /CS high again
  digitalWrite(PIN_POT_CS, HIGH);
}