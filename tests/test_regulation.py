"""Run the actual firmware sources with a deterministic clock and panel model."""
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]

ARDUINO_H = '''#pragma once
#include <stdint.h>
#include <avr/io.h>
#define OUTPUT 1
#define INPUT 0
#define HIGH 1
#define LOW 0
#define PIN_A6 6
#define PIN_A7 7
#define PIN_A1 1
#define PIN_A2 2
#define PIN_A3 3
#define INTERNAL4V3 2
#define PROGMEM
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
uint32_t millis();
void delay(uint32_t);
void pinMode(int, int);
void digitalWrite(int, int);
uint16_t analogRead(int);
void analogReference(int);
'''

AVR_IO_H = '''#pragma once
static struct { int CTRLA; int CTRLB; int CTRLD; } ADC0;
static struct { int CTRLA; } WDT;
#define ADC_INITDLY_DLY64_gc 1
#define ADC_SAMPNUM_ACC64_gc 1
#define WDT_PERIOD_8KCLK_gc 1
#define _PROTECTED_WRITE(reg, value) reg = value
'''

AVR_WDT_H = '#define wdt_reset() ((void)0)\n'

LOOP_TEST = '''#include <cassert>
#include <cstdlib>
#include "main.cpp"
uint32_t clock_ms;
uint16_t position;
int writes, reads;
bool dark;
int base;
uint16_t max_pos;
uint32_t millis() { return clock_ms; }
void delay(uint32_t ms) { clock_ms += ms; }
void pinMode(int, int) {}
void digitalWrite(int, int) {}
void ADC_init() {}
void POT_init() {}
void POT_set_steps(uint16_t s) { assert(s <= 256); position = s; if(s > max_pos) max_pos = s; ++writes; }
void POT_set_voltage(uint16_t) { position = 60; }
uint16_t ADC_read_voltage() {
  clock_ms += 3; // Conversion time must count toward the cycle budget.
  if(++reads == 1) return dark ? 0 : 22063; // target = 17650
  return base + (60 - (int)position) * 104;
}
void run(uint32_t start, uint16_t initial, uint16_t expected, bool is_dark=false,
         int base_mv=17697) {
  clock_ms = start; reads = writes = 0; dark = is_dark; base = base_mv; max_pos = 0;
#ifndef ENABLE_VMPP_LOOKUP
  steps = initial;
#endif
  loop();
  assert((uint32_t)(clock_ms - start) == 2000);
  assert(position == expected);
#ifndef ENABLE_VMPP_LOOKUP
  if(!dark) assert(steps == position);
#endif
}
int main() {
  run(0, 60, 60);
  run(0, 61, 60); // Cross target from the other direction.
  run(UINT32_MAX - 100, 60, 60); // millis() wraparound.
  run(0, 60, 0, true);
#ifndef ENABLE_VMPP_LOOKUP
  run(0, 150, 81); // Time budget expires before convergence.
  // Symmetric crossing: |previous error| == |error| keeps the previous position.
  run(0, 60, 60, false, 17702);
  assert(max_pos == 61); // The search must actually cross the target.
#endif
}
'''

MODULE_TEST = '''#include <cassert>
#include "Arduino.h"
#include "pins.h"
#include "adc.h"
#include "potentiometer.h"
#include "vmpp.h"

// Decode the bit-banged SPI stream: sample SDI on every SCK rising edge
// inside a /CS-low frame and reassemble the 16-bit command word.
static int clk_level = 0, da_level = 0, bits = 0, frames = 0;
static bool in_frame = false;
static uint16_t shift_word = 0, last_frame = 0xFFFF;
static int adc_raw = 0;

void pinMode(int, int) {}
void digitalWrite(int pin, int value) {
  if(pin == PIN_POT_CS) {
    if(value == LOW && !in_frame) { in_frame = true; shift_word = 0; bits = 0; }
    else if(value == HIGH && in_frame) {
      in_frame = false;
      assert(bits == 16);
      last_frame = shift_word;
      ++frames;
    }
  } else if(pin == PIN_POT_DA) {
    da_level = value;
  } else if(pin == PIN_POT_CLK) {
    if(in_frame && clk_level == LOW && value == HIGH) {
      shift_word = (shift_word << 1) | (da_level ? 1 : 0);
      ++bits;
    }
    clk_level = value;
  }
}
uint16_t analogRead(int) { return (uint16_t)adc_raw; }
void analogReference(int) {}
uint32_t millis() { return 0; }
void delay(uint32_t) {}

static uint16_t drive_steps(uint16_t s) {
  const int before = frames;
  POT_set_steps(s);
  assert(frames == before + 1);
  assert((last_frame >> 9) == 0); // Address 0h, command 00 (write data)
  return last_frame & 0x01FF;
}

int main() {
  // ADC math against the real divider constant.
  ADC_init();
  adc_raw = 0;
  assert(ADC_read_voltage() == 0);
  adc_raw = 64 * 512;
  assert(ADC_read_voltage() == 13900); // 512 * ADC_MAX_MV / ADC_STEPS
  adc_raw = 64 * 1023;
  assert(ADC_read_voltage() == 27772); // truncated full-scale-minus-one

  // Wiper writes decode to a zero command byte plus the position,
  // clamped to the 0..POT_STEPS range; idle levels stay deterministic.
  POT_init();
  assert(frames == 0);
  assert(clk_level == LOW);
  assert(!in_frame);
  assert(drive_steps(0) == 0);
  assert(drive_steps(123) == 123);
  assert(drive_steps(POT_STEPS) == POT_STEPS);
  assert(drive_steps(511) == POT_STEPS); // out-of-range input clamps

  // Table lookup brackets VMPP between adjacent entries.
  const struct { uint16_t vmpp; uint16_t pos; } cases[] = {
      {28000, 0},  // above the first entry
      {17650, 61}, // mid-table
      {8522, 256}, // exactly the last entry
      {8000, 256}, // below the last entry
  };
  for(unsigned i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    const int before = frames;
    POT_set_voltage(cases[i].vmpp);
    assert(frames == before + 1);
    assert((last_frame & 0x01FF) == cases[i].pos);
    if(pgm_read_word(vmpp_table + POT_STEPS) <= cases[i].vmpp) {
      assert(pgm_read_word(vmpp_table + cases[i].pos) <= cases[i].vmpp);
      assert(cases[i].pos == 0 ||
             pgm_read_word(vmpp_table + cases[i].pos - 1) > cases[i].vmpp);
    }
  }
  return 0;
}
'''


def write_stubs(path):
    (path / 'avr').mkdir()
    (path / 'Arduino.h').write_text(ARDUINO_H)
    (path / 'avr' / 'io.h').write_text(AVR_IO_H)
    (path / 'avr' / 'wdt.h').write_text(AVR_WDT_H)


def compile_and_run(path, sources, name, flags=()):
    binary = path / name
    subprocess.run(['c++', '-std=c++11', *flags, '-I' + str(path),
                    '-I' + str(ROOT / 'src'), *[str(s) for s in sources],
                    '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)


class RegulationLoopTest(unittest.TestCase):
    def test_loop(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp)
            write_stubs(path)
            (path / 'loop_test.cpp').write_text(LOOP_TEST)
            for flags in ([], ['-DENABLE_VMPP_LOOKUP']):
                compile_and_run(path, [path / 'loop_test.cpp'],
                                'loop_test' + ('_lookup' if flags else ''), flags)


class ModuleTest(unittest.TestCase):
    def test_adc_and_potentiometer(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp)
            write_stubs(path)
            (path / 'module_test.cpp').write_text(MODULE_TEST)
            compile_and_run(path, [path / 'module_test.cpp',
                                   ROOT / 'src' / 'adc.cpp',
                                   ROOT / 'src' / 'potentiometer.cpp'],
                            'module_test')


if __name__ == '__main__':
    unittest.main()
