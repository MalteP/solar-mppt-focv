# BOM, rev1

Fitted parts, grouped by type. Source: `bom-rev1.csv`, the EasyEDA export. 13 lines, 15 parts per board.

## Integrated circuits and modules

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| U1 | 1 | ATtiny412 | `ATTINY412-SSF` | MICROCHIP | SOIC-8 | [C1338345](https://www.lcsc.com/product-detail/C1338345.html) |
| U2 | 1 | 5 V LDO | `MCP1804T-5002I/OT` | MICROCHIP | SOT-23-5 | [C625416](https://www.lcsc.com/product-detail/C625416.html) |
| U3 | 1 | Digital potentiometer, 100 kΩ | `MCP4151T-104E/SN` | MICROCHIP | SOIC-8 | [C623372](https://www.lcsc.com/product-detail/C623372.html) |

## Diodes and LEDs

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| D1 | 1 | Green LED | `NCD0805G1` | NATIONSTAR | 0805 | [C84260](https://www.lcsc.com/product-detail/C84260.html) |

## Capacitors

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| C1, C2 | 2 | 1 µF 50 V X7R | `CL21B105KBFNNNE` | SAMSUNG | 0805 | [C28323](https://www.lcsc.com/product-detail/C28323.html) |
| C3, C4 | 2 | 100 nF 50 V X7R | `CL21B104KBCNNNC` | SAMSUNG | 0805 | [C1711](https://www.lcsc.com/product-detail/C1711.html) |
| C5 | 1 | 10 nF 50 V X7R | `CL21B103KBANNNC` | SAMSUNG | 0805 | [C1710](https://www.lcsc.com/product-detail/C1710.html) |

## Resistors

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| R1 | 1 | 18 kΩ 1% | `RC0805FR-0718KL` | YAGEO | 0805 | [C114522](https://www.lcsc.com/product-detail/C114522.html) |
| R2 | 1 | 3.3 kΩ 1% | `RC0805FR-073K3L` | YAGEO | 0805 | [C114531](https://www.lcsc.com/product-detail/C114531.html) |
| R3 | 1 | 1 kΩ 1% | `RC0805FR-071KL` | YAGEO | 0805 | [C95781](https://www.lcsc.com/product-detail/C95781.html) |

## Connectors and mechanics

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| K1 | 1 | Header 1×1: VIN | — | — | 2.54 mm | [C81276](https://www.lcsc.com/product-detail/C81276.html) |
| K2 | 1 | Right-angle header 1×3: POT | `PZ254R-11-03P` | XFCN | 2.54 mm | [C492411](https://www.lcsc.com/product-detail/C492411.html) |
| K3 | 1 | Header 1×3: UPDI | `77311-102-03LF` | — | 2.54 mm | [C180248](https://www.lcsc.com/product-detail/C180248.html) |

## Not on the BOM

| Item | Qty | Note |
|---|---|---|
| Wire from `K1` to the panel input of the CN3722 module | 1 | Fitted at installation |

## Notes

- Top side: C3, C5, D1, R1, R2, R3, U3. Bottom side: C1, C2, C4, U1, U2. K1–K3 are through-hole, housings on the bottom side, which faces the CN3722 module.
- `K2` pins point 5.4 mm past the bottom edge. Pin 1 is the wiper, pins 2 and 3 are GND.
- `K3` part number comes from the EasyEDA library record for its LCSC code.
- Stock and prices were not checked.
