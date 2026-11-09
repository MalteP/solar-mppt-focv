# BOM, rev1

Parts fitted on the board, grouped by part type. Generated from `bom-rev1.csv`, the EasyEDA export.

13 lines, 15 parts per board.

## Integrated circuits

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| U1 | 1 | ATtiny412 | `ATTINY412-SSF` | MICROCHIP | SOIC-8 | [C1338345](https://www.lcsc.com/product-detail/C1338345.html) |
| U2 | 1 | 5 V LDO | `MCP1804T-5002I/OT` | MICROCHIP | SOT-23-5 | [C625416](https://www.lcsc.com/product-detail/C625416.html) |
| U3 | 1 | 100 kΩ digital pot | `MCP4151T-104E/SN` | MICROCHIP | SOIC-8 | [C623372](https://www.lcsc.com/product-detail/C623372.html) |

## Diodes and LEDs

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| D1 | 1 | Green | `NCD0805G1` | NATIONSTAR | LED0805 | [C84260](https://www.lcsc.com/product-detail/C84260.html) |

## Capacitors

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| C1, C2 | 2 | 1 µF 50 V X7R | `CL21B105KBFNNNE` | SAMSUNG | C0805 | [C28323](https://www.lcsc.com/product-detail/C28323.html) |
| C3, C4 | 2 | 100 nF 50 V X7R | `CL21B104KBCNNNC` | SAMSUNG | C0805 | [C1711](https://www.lcsc.com/product-detail/C1711.html) |
| C5 | 1 | 10 nF 50 V X7R | `CL21B103KBANNNC` | SAMSUNG | C0805 | [C1710](https://www.lcsc.com/product-detail/C1710.html) |

## Resistors

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| R1 | 1 | 18 kΩ 1% | `RC0805FR-0718KL` | YAGEO | R0805 | [C114522](https://www.lcsc.com/product-detail/C114522.html) |
| R2 | 1 | 3.3 kΩ 1% | `RC0805FR-073K3L` | YAGEO | R0805 | [C114531](https://www.lcsc.com/product-detail/C114531.html) |
| R3 | 1 | 1 kΩ 1% | `RC0805FR-071KL` | YAGEO | R0805 | [C95781](https://www.lcsc.com/product-detail/C95781.html) |

## Connectors

| Refs | Qty | Value | Part | Manufacturer | Package | LCSC |
|---|---|---|---|---|---|---|
| K1 | 1 | VIN, 1×1 header | LCSC catalogue part | Not listed | 2.54 mm | [C81276](https://www.lcsc.com/product-detail/C81276.html) |
| K2 | 1 | POT, 1×3 right-angle header | `PZ254R-11-03P` | XFCN | 2.54 mm | [C492411](https://www.lcsc.com/product-detail/C492411.html) |
| K3 | 1 | UPDI, 1×3 header | `77311-102-03LF` | Not listed | 2.54 mm | [C180248](https://www.lcsc.com/product-detail/C180248.html) |

## Assembly notes

Parts sit on both sides. Top: C3, C5, D1, R1, R2, R3, U3. Bottom: C1, C2, C4, U1, U2. K1, K2 and K3 are through-hole headers with the housing on the bottom side, which faces the CN3722 module.

K2 is a right-angle header. Its pins point past the bottom board edge by 5.4 mm. Pin 1 is the potentiometer wiper, and pins 2 and 3 are GND.

The part number for K3 comes from the EasyEDA library record for the LCSC code. The project itself holds only the LCSC code.

## Not on the BOM

| Item | Qty | Note |
|---|---|---|
| Wire from `K1` to the panel input of the CN3722 module | 1 | Fitted when the board is installed |

## Substitutions

- Ceramic capacitors are all Samsung CL, chip resistors are all Yageo RC. Changed on 2026-11-09 from mixed YAGEO CC and UNI-ROYAL parts with the same values, footprints and ratings. Only part attributes changed in EasyEDA; footprints, placement and Gerbers are unchanged.
- `C3`, `C4` (100 nF 0805) and `D1` are the same parts as on `solar-power-module-CN3722` rev2. `R1`, `R2`, `R3` use the same Yageo RC0805 series; those values are not on the other boards.
- `C1`, `C2` (1 µF) and `C5` (10 nF) were already Samsung CL and are unchanged. The other boards have these values only in 0603 or 1206.
- The new parts are JLCPCB Extended parts; the replaced UNI-ROYAL resistors were Basic.

Stock and prices were not checked.
