# Release rev1

Prototype release, exported from EasyEDA Pro 3.2.209 on 2026-11-07. **Not validated and not ordered.** It has not been built or measured.

| File | Content |
|---|---|
| `gerber-rev1.zip` | Gerber and drill files, two layers. Upload this to the fab |
| `BOM.md` | Parts list with order links |
| `bom-rev1.csv` | EasyEDA BOM export, 15 parts in 13 lines, UTF-16 |
| `placement-rev1.csv` | Part positions, all 15 parts, UTF-16 |
| `SHA256SUMS` | Checksums |
| `assembly-rev1.step` | 3D assembly, 7 MB, kept local and not in git; export it again from EasyEDA if missing |

The order instruction is in the [root README](../../Readme.md#ordering).

## Checks on these files

- DRC on the saved PCB: 0 errors, after closing and reopening the document and immediately before export.
- Drill: 18 vias of 0.3 mm, 4 header holes of 1.0 mm for K1 and K3, and 3 header holes of 1.1 mm for K2.
- Outline: 22.73 × 13.92 mm.
- Part bounding boxes are at least 1.0 mm from the board edge, except K2. The K2 housing is 0.6 mm from the edge and its pins extend past it. Designators are at least 0.6 mm from the edge.
- The BOM holds U2 as `MCP1804T-5002I/OT`, C625416, and K2 as `PZ254R-11-03P`, C492411.

## Known differences

- The Device column in `placement-rev1.csv` was corrected by hand for U2. The project still names that library device `MCP1804T-1802I/OT`, so a new export brings the old name back. The BOM part number and LCSC code are correct.
- `assembly-rev1.step` shows a 0.09 mm board, because the project stack-up has a core thickness of 0. Do not use it to check heights. The images in `docs/images` were scaled to a 1.6 mm board. The Gerbers are not affected.

- The K2 footprint draws its pins on the bottom silkscreen beyond the board edge. Only the part on the board can be printed.

## Not verified

- K2 has not been matched to the potentiometer pads of a real CN3722 module, and the board has not been fitted to one. The wiper is on the right pin when viewed from the top side.
- D1 polarity is taken from the footprint markings and has not been compared with the datasheet.
- The fabricator's DFM check has not been run.
- ERC has not been run.
- LCSC stock and prices have not been checked.
