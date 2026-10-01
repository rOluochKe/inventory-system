# Data File Format — `data/inventory.dat`

A single plain-text file. UTF-8. Fields separated by `|`. Blank lines and
lines beginning with `#` are ignored. Sections appear in this order:
`[SUPPLIERS]`, `[PRODUCTS]`, `[TRANSACTIONS]`.

## [SUPPLIERS]
Header: id|name|contact|email
1|Mouser|John Doe|john@mouser.com

## [PRODUCTS]
Header: id|sku|name|category|quantity|reorder_level|price|supplier_id
1|R-100|Resistor 100Ohm|Passive|500|100|0.05|1

## [TRANSACTIONS]
Header: id|product_id|type|quantity|timestamp|note
1|1|IN|500|2026-09-18T10:00:00|Initial stock

`type` ∈ {IN, OUT, ADJ}. `timestamp` is ISO 8601. `note` may be empty but
the field must still be present (trailing `|`).

## Error handling
- Missing file: application starts with an empty inventory.
- Malformed line: skipped; a warning is written to stderr; parsing continues.
- Unknown section header: ignored.