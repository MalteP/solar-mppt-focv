# Repository Guidelines

## Project Structure & Module Organization

This project implements fractional open-circuit voltage (FOCV) tracking for a CN3722 solar charger using an ATtiny412 and MCP4151 digital potentiometer.

- `src/main.cpp`: setup, watchdog, VOC sampling, and iterative regulation.
- `src/adc.cpp` and `src/adc.h`: voltage measurement and calibration constants.
- `src/potentiometer.cpp` and `src/potentiometer.h`: software SPI and wiper control.
- `src/pins.h`: hardware pin assignments; `src/vmpp.h`: optional voltage lookup table.
- `tests/test_regulation.py`: host-side regression tests using simulated hardware.
- `platformio.ini`: target, clock, programmer, and serial settings.
- `Readme.md`: wiring, calibration, and build-time options. `.pio/` contains generated build artifacts.

## Build, Test, and Development Commands

Run commands from the repository root:

- `pio run`: build firmware for the configured `ATtiny412` environment.
- `python3 tests/test_regulation.py`: compile and execute host tests; requires Python 3 and a C++ compiler available as `c++`.
- `pio run --target upload`: flash connected hardware after verifying the programmer and upload port in `platformio.ini`.
- `pio device monitor`: open the configured 9600-baud serial monitor. Firmware serial output requires `ENABLE_DEBUG`.

Firmware runs on the microcontroller; host tests provide a simulated development check.

## Coding Style & Naming Conventions

Follow `.clang-format`: LLVM-based C++, two-space indentation, no column limit, preserved include order, and no space before control-statement parentheses. Use `clang-format -i` on changed C++ files when available; avoid unrelated formatting changes.

Keep constants uppercase (`MEAS_DELAY_MS`), variables lowercase with underscores, and module functions consistent with existing prefixes (`ADC_`, `POT_`). Preserve units in names such as `_MV` and `_MS`. Use explicit integer widths and widen intermediate voltage arithmetic where needed.

## Testing Guidelines

Tests use Python `unittest` to compile the actual firmware loop against lightweight Arduino stubs. Name additional tests `test_*.py`. Cover control-loop changes with deterministic scenarios, especially target crossings, invalid VOC, timing budgets, and clock wraparound. There is no enforced coverage percentage. Run host tests and the firmware build before submitting changes.

Simulation does not validate electrical behavior: verify VOC settling time and charging disable behavior on hardware when changing those assumptions.

## Commit & Pull Request Guidelines

Existing commits use short imperative subjects, such as “Fix ADC full-scale divisor” and “Document wiring.” Follow that style and keep commits focused.

Pull requests should explain the behavior change, list build/test results, link relevant issues, and identify hardware assumptions or calibration changes. Update `Readme.md` when wiring or configuration changes. Preserve unrelated local edits and avoid committing generated artifacts or machine-specific port changes unintentionally.
