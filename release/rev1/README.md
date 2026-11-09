# Release rev1

Prototype, exported from EasyEDA Pro 3.2.209 on 2026-11-07. BOM exported again on 2026-11-09 after the passives change; Gerber and placement unchanged. **Not validated and not ordered.**

| File | Content |
|---|---|
| `gerber-rev1.zip` | Gerber and drill, 2 layers. Upload this to the fab |
| `BOM.md` | Parts list with order links |
| `bom-rev1.csv` | EasyEDA BOM export, 15 parts, UTF-16 |
| `placement-rev1.csv` | Part positions, 15 parts, UTF-16 |
| `SHA256SUMS` | Checksums |
| `assembly-rev1.step` | 3D assembly, 7 MB, local only. Shows a 0.09 mm board: do not use it for heights |

The order instruction is in the [root README](../../README.md#ordering).

## Checks on these files

- DRC on the reopened, saved PCB: 0 errors, immediately before export.
- Drill: 18 vias of 0.3 mm, 4 holes of 1.0 mm (K1, K3), 3 holes of 1.1 mm (K2).
- Outline 22.73 × 13.92 mm. Parts at least 1.0 mm from the edge, except K2: housing 0.6 mm, pins past the edge.

## Known differences

- The `Device` column of `placement-rev1.csv` was corrected by hand for U2. A new export brings back the old library name `MCP1804T-1802I/OT`; the BOM is correct.
- The K2 footprint draws its pins on the bottom silkscreen beyond the edge; only the part on the board is printed.

## Not verified

- K2 against the potentiometer pads of a real CN3722 module. The wiper is on the right pin, seen from the top.
- D1 polarity against its data sheet.
- ERC, fabricator DFM, LCSC stock and prices.
