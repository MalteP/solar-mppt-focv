"""Run the actual firmware loop with a deterministic clock and panel model."""
import pathlib
import subprocess
import tempfile
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]


class RegulationTest(unittest.TestCase):
    def test_loop(self):
        with tempfile.TemporaryDirectory() as tmp:
            path = pathlib.Path(tmp)
            (path / 'avr').mkdir()
            (path / 'Arduino.h').write_text('''#pragma once
#include <stdint.h>
#define OUTPUT 1
#define HIGH 1
#define LOW 0
#define PIN_A6 6
#define PIN_A7 7
#define PIN_A1 1
#define PIN_A2 2
#define PIN_A3 3
uint32_t millis();
void delay(uint32_t);
void pinMode(int, int);
void digitalWrite(int, int);
''')
            (path / 'avr/io.h').write_text('''#pragma once
struct { int CTRLA; } WDT;
#define WDT_PERIOD_8KCLK_gc 1
#define _PROTECTED_WRITE(reg, value) reg = value
''')
            (path / 'avr/wdt.h').write_text('#define wdt_reset() ((void)0)\n')
            (path / 'test.cpp').write_text('''#include <cassert>
#include <cstdlib>
#include "main.cpp"
uint32_t clock_ms;
uint16_t position;
int writes, reads;
bool dark;
uint32_t millis() { return clock_ms; }
void delay(uint32_t ms) { clock_ms += ms; }
void pinMode(int, int) {}
void digitalWrite(int, int) {}
void ADC_init() {}
void POT_init() {}
void POT_set_steps(uint16_t s) { assert(s <= 256); position = s; ++writes; }
void POT_set_voltage(uint16_t) { position = 60; }
uint16_t ADC_read_voltage() {
  clock_ms += 3; // Conversion time must count toward the cycle budget.
  if(++reads == 1) return dark ? 0 : 22063; // target = 17650
  return 17697 + (60 - (int)position) * 104;
}
void run(uint32_t start, uint16_t initial, uint16_t expected, bool is_dark=false) {
  clock_ms = start; reads = writes = 0; dark = is_dark;
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
#endif
}
''')
            for flags in ([], ['-DENABLE_VMPP_LOOKUP']):
                subprocess.run(['c++', '-std=c++11', *flags, '-I'+tmp,
                                '-I'+str(ROOT / 'src'), str(path / 'test.cpp'),
                                '-o', str(path / 'test')], check=True)
                subprocess.run([str(path / 'test')], check=True)


if __name__ == '__main__':
    unittest.main()
