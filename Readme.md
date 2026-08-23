# Solar MPPT FOCV Tracker

Implementation of a Fractional Open Circuit Voltage (FOCV) Solar Tracker for my CN3722 Solar MPPT charge controller, which was (like most analog charger ICs) only tracking a fixed input voltage. Tracking is done by an ATTiny412 microcontroller and an MCP4151 digital potentiometer instead of the precision potentiometer on the circuit board.

See the following website for further details:

https://www.maltepoeggel.de/?site=solar-mppt-focv

## Wiring

ATtiny412 (8-pin), see `src/pins.h`:

| ATtiny412 pin | Function          | Connects to                                    |
|----------------|-------------------|-------------------------------------------------|
| PA0            | UPDI              | Programmer (xplainedmini_updi / atmelice_updi)  |
| PA1            | `PIN_POT_DA`       | MCP4151 SDI (SPI data)                          |
| PA2            | `PIN_POT_CS`       | MCP4151 CS                                      |
| PA3            | `PIN_POT_CLK`      | MCP4151 SCK                                     |
| PA6            | `PIN_LED` / UART TX | Status LED, or debug serial TX (see below)     |
| PA7            | `ADC_INP_VOLT`     | Panel voltage divider (18K / 3.3K) tap          |

PA6 is shared between the status LED and the debug UART TX line because
the 8-pin ATtiny412 has no free pin for a dedicated debug output — hence
`ENABLE_DEBUG` and the LED are mutually exclusive at build time.

## Build-time options (`src/main.cpp`)

Both are disabled by default; uncomment the `#define` to enable:

- `ENABLE_VMPP_LOOKUP` — derive the potentiometer position for VMPP from
  the precomputed `vmpp_table` (`src/vmpp.h`) instead of the default
  iterative regulation loop. Useful for a faster single-shot response,
  at the cost of accuracy between table entries.
- `ENABLE_DEBUG` — print `VOC;VMPP[;steps];` over UART (9600 baud) on
  PA6 instead of driving the status LED.

## Calibration constants

- `ADC_MAX_MV` (`src/adc.h`) — input voltage corresponding to a
  full-scale ADC reading, derived from the 18K/3.3K input voltage
  divider and the 4.3V internal ADC reference:
  `4.3V * (18K + 3.3K) / 3.3K ≈ 27.75V`. Re-derive this if the divider
  resistors or ADC reference change, and verify against a multimeter.
- `K_PERCENT` (`src/main.cpp`) — ratio of VMPP to VOC for the panel in
  use (80% is a common default for crystalline silicon panels).
- `ADC_DELAY_MS` (`src/main.cpp`) — settling time after a potentiometer
  step before the ADC is read; must be long enough for the panel/input
  capacitance to settle, particularly when measuring VOC with the load
  removed. Verify with a scope if the panel or cabling changes.
- `VOC_MIN_MV` (`src/main.cpp`) — minimum plausible VOC reading; below
  this, the input is treated as disconnected/dark and charging is left
  disabled rather than regulating off a bogus reading.
- `VMPP_DEADBAND_MV` (`src/main.cpp`) — tolerance band around VMPP
  within which the regulation loop leaves the potentiometer alone, to
  avoid dithering by one step every cycle once converged.
