# Code Book — Inventory Management System

## Public API

The public surface is the headers under `include/inventory/`. Nothing else
is intended for external use.

### Types

- `Product`     — a sellable item (id, sku, name, category, quantity,
                   reorderLevel, price, supplierId).
- `Supplier`    — a supplier (id, name, contact, email).
- `Transaction` — a stock movement (id, productId, type, quantity,
                   timestamp, note). `type` ∈ {StockIn, StockOut, Adjustment}.

### Class `Inventory`

Single owner of all entities. Every mutation goes through here.

| Method | Effect |
|---|---|
| `addProduct(Product)` | Inserts if id and sku are unused; returns false otherwise |
| `editProduct(Product)` | Replaces the record with the same id |
| `removeProduct(int)` | Erases by id |
| `findProductById / BySku` | Returns `const Product*` or `nullptr` |
| `addSupplier/editSupplier/removeSupplier` | Analogous; removal refused if referenced |
| `recordTransaction(Transaction)` | Applies the movement and logs it; rejects invalid movements |
| `restore(...)` | Used only by `Persistence` to populate from disk |

### Class `Persistence`

`load(Inventory&, std::string& err)` and `save(const Inventory&, std::string& err)`.
Missing file → empty inventory (not an error). Malformed line → skipped with
message in `err`.

### Class `Report`

`currentStock`, `lowStock`, `transactionHistory` — pure functions returning
formatted strings.

## Data structures & why

- `std::vector` — contiguous storage, cache-friendly iteration for reports.
- `std::unordered_map<int, size_t>` — O(1) lookup by id for products and suppliers.
- `std::unordered_map<string, size_t>` — O(1) lookup by SKU.
- Indexes store **positions**, not pointers → remain valid after vector growth
  (rebuilt by `rebuildIndexes()` on structural changes).

## Memory model

- Value semantics everywhere; no owning raw pointers; no manual `new`/`delete`.
- Foreign keys are integer IDs, not pointers → trivially serialisable and
  immune to dangling pointers.
- RAII via `std::string`, `std::vector`, `std::ifstream`/`ofstream`.
- `std::unique_ptr` reserved for future polymorphic reports (not currently needed).

## File format `data/inventory.dat`

Line-based, `|`-delimited, `#`-comments and blank lines ignored, three
sections: `[SUPPLIERS]`, `[PRODUCTS]`, `[TRANSACTIONS]`. Each section begins
with a header row (`id|...`). See README for a full example.
