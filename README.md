# Inventory Management System

A CLI inventory management application written in C++17 for a small
electronics spare-parts shop. Stores products, suppliers, and stock
transactions, and persists all data to disk so that state survives across
runs.

## Requirements

- C++17 compiler (g++ >= 9 or clang++ >= 10)
- CMake >= 3.15

## Build

- cmake -S . -B build
- cmake --build build

ctest --test-dir build --output-on-failure

```bash
The build produces:
- `build/libinventory_core.a` — the reusable library
- `build/inventory_app` — the CLI application
- `build/inventory_tests` — the test runner
```

## Run

./build/inventory_app

```bash
By default the application reads and writes `data/inventory.dat`. Pass a
different path as the first argument:
```

The application presents a two-level numbered menu:

```bash
=== Inventory Management System ===

1. Products 2. Suppliers 3. Transactions
2. Reports 5. Save 6. Exit
```

```bash
Products → add, edit, remove, list, search (by SKU/name/category), sort
(by name/quantity/price).
Suppliers → add, remove, list.
Transactions → stock-in, stock-out, adjustment, full history, per-product
history.
Reports → current stock, low-stock items, transaction history.
Save → writes the current state to the data file (also done automatically
on exit).
```

## Test

ctest --test-dir build --output-on-failure

```bash
The suite covers: adding products, duplicate SKU rejection, empty-inventory
reports, stock-in, insufficient stock-out rejection, negative quantity
rejection, adjustment, blocked supplier removal, persistence round-trip,
missing file, and malformed file.
```

## Project structure

```bash
inventory-system/
├── CMakeLists.txt
├── README.md
├── include/inventory/ # public headers (Product, Supplier, Transaction,
│ # Inventory, Persistence, Report)
├── src/ # implementation (Inventory.cpp, Persistence.cpp,
│ # Report.cpp, main.cpp)
├── tests/ # unit tests (test_inventory.cpp)
├── docs/
│ ├── CODEBOOK.md # public API and data structures
│ └── FILE_FORMAT.md # on-disk format specification
└── data/
│ ├── inventory.dat # sample data file
└── component-diagram.pdf # System flow diagram
└── uml-class-diagram.pdf # System ER Diagram
```

## Documentation

- `docs/CODEBOOK.md` — public API, data structures, memory model.
- `docs/FILE_FORMAT.md` — on-disk format specification and error handling.

### File format at a glance

[SUPPLIERS]
id|name|contact|email

[PRODUCTS]
id|sku|name|category|quantity|reorder_level|price|supplier_id

[TRANSACTIONS]
id|product_id|type|quantity|timestamp|note

`type` ∈ {IN, OUT, ADJ}. A missing file yields an empty inventory; a
malformed line is skipped with a warning and the rest of the file still
loads.

## Architecture

Three layers plus persistence:

- **Interface** (`src/main.cpp`) — CLI menu; holds no state.
- **Logic** (`Inventory`, `Report`) — all operations and reporting.
- **Data** (`Product`, `Supplier`, `Transaction`) — plain structs.
- **Persistence** (`Persistence`) — the only filesystem touchpoint.

Dependencies flow strictly downward: interface → logic → data;
logic ↔ persistence.

## Memory model

Value semantics and RAII throughout. Entities live in `std::vector`s owned
by `Inventory`; lookup is via `std::unordered_map` indexes that store
positions, not pointers. Foreign keys are integer IDs. There are no owning
raw pointers and no manual `new`/`delete`.

## Status

- Phase 1 — conception document and initial build skeleton. ✔
- Phase 2 — full CLI, persistence, expanded tests, code book, and file-format
  spec. ✔
- Phase 3 — hardening, final abstract, demo video, and ZIP submission. ✔
  
## Changes since Phase 1

- Added `docs/CODEBOOK.md` and `docs/FILE_FORMAT.md`.
- Persistence loader now collects a warning string instead of aborting on the
  first malformed line — a defensive choice that more directly matches the
  "handle a malformed file gracefully" requirement.
- Expanded the test suite to 11 tests covering all required edge cases.
- Completed the two-level CLI menu and added a typed-input helper that
  re-prompts on non-numeric input.

## License

Coursework submission for IU Internationale Hochschule. Not for redistribution.
