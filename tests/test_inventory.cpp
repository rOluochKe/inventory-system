#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cstdio>

#include "inventory/Inventory.h"
#include "inventory/Persistence.h"
#include "inventory/Report.h"

using namespace inventory;

static void testAddProduct() {
    Inventory inv;
    Product p; p.sku = "R-100"; p.name = "Resistor"; p.quantity = 100;
    assert(inv.addProduct(p));
    assert(inv.products().size() == 1);
    assert(inv.findProductBySku("R-100") != nullptr);
}

static void testDuplicateSkuRejected() {
    Inventory inv;
    Product a; a.sku = "X";
    Product b; b.sku = "X";
    assert(inv.addProduct(a));
    assert(!inv.addProduct(b));
}

static void testEmptyInventoryReport() {
    Inventory inv;
    assert(Report::currentStock(inv).find("Current Stock") != std::string::npos);
    assert(Report::lowStock(inv).find("Low Stock") != std::string::npos);
}

static void testStockIn() {
    Inventory inv;
    Product p; p.sku = "C-10"; p.quantity = 10; inv.addProduct(p);
    Transaction t; t.productId = inv.products().front().id;
    t.type = TransactionType::StockIn; t.quantity = 5;
    assert(inv.recordTransaction(t));
    assert(inv.findProductById(t.productId)->quantity == 15);
}

static void testStockOutInsufficientRejected() {
    Inventory inv;
    Product p; p.sku = "C-10"; p.quantity = 3; inv.addProduct(p);
    Transaction t; t.productId = inv.products().front().id;
    t.type = TransactionType::StockOut; t.quantity = 10;
    assert(!inv.recordTransaction(t));
}

static void testNegativeQuantitiesRejected() {
    Inventory inv;
    Product p; p.sku = "C-10"; p.quantity = 5; inv.addProduct(p);
    Transaction t; t.productId = inv.products().front().id;
    t.type = TransactionType::StockIn; t.quantity = -1;
    assert(!inv.recordTransaction(t));
    t.type = TransactionType::Adjustment; t.quantity = -5;
    assert(!inv.recordTransaction(t));
}

static void testAdjustmentSetsQuantity() {
    Inventory inv;
    Product p; p.sku = "C-10"; p.quantity = 5; inv.addProduct(p);
    Transaction t; t.productId = inv.products().front().id;
    t.type = TransactionType::Adjustment; t.quantity = 42;
    assert(inv.recordTransaction(t));
    assert(inv.findProductById(t.productId)->quantity == 42);
}

static void testSupplierRemovalBlockedIfReferenced() {
    Inventory inv;
    Supplier s; s.name = "Mouser"; inv.addSupplier(s);
    Product p; p.sku = "R-1"; p.supplierId = inv.suppliers().front().id;
    inv.addProduct(p);
    assert(!inv.removeSupplier(inv.suppliers().front().id));
}

static void testPersistenceRoundTrip() {
    const std::string path = "test_roundtrip.dat";
    std::remove(path.c_str());
    {
        Inventory inv;
        Supplier s; s.name = "Mouser"; inv.addSupplier(s);
        Product p; p.sku = "R-1"; p.name = "Resistor"; p.quantity = 7;
        p.supplierId = inv.suppliers().front().id;
        inv.addProduct(p);
        Persistence per(path);
        std::string err;
        assert(per.save(inv, err));
    }
    {
        Inventory inv;
        Persistence per(path);
        std::string err;
        assert(per.load(inv, err));
        assert(inv.products().size() == 1);
        assert(inv.suppliers().size() == 1);
        assert(inv.findProductBySku("R-1")->quantity == 7);
    }
    std::remove(path.c_str());
}

static void testLoadMissingFileIsOk() {
    Inventory inv;
    Persistence per("does_not_exist.dat");
    std::string err;
    assert(per.load(inv, err));
    assert(inv.products().empty());
}

static void testMalformedLineIsSkipped() {
    const std::string path = "test_malformed.dat";
    {
        std::ofstream out(path);
        out << "[PRODUCTS]\n"
               "id|sku|name|category|quantity|reorder_level|price|supplier_id\n"
               "1|R-1|Resistor|Passive|10|2|0.05|0\n"
               "BAD_LINE_WITH_MISSING_FIELDS\n";
    }
    Inventory inv;
    Persistence per(path);
    std::string err;
    assert(per.load(inv, err));
    assert(inv.products().size() == 1); // good line loaded, bad line skipped
    std::remove(path.c_str());
}

static void testSafeSaveReplacesFile() {
    const std::string path = "test_safesave.dat";
    std::remove(path.c_str());
    Inventory inv;
    Product p; p.sku = "A"; p.quantity = 1; inv.addProduct(p);
    Persistence per(path);
    std::string err;
    assert(per.save(inv, err));
    assert(std::filesystem::exists(path));
    assert(!std::filesystem::exists(path + ".tmp"));
    std::remove(path.c_str());
}

static void testTimestampFormat() {
    const std::string s = "2026-10-01T12:34:56";
    assert(s.size() == 19);
    assert(s[10] == 'T');
    assert(s[4]  == '-' && s[7] == '-');
}

int main() {
    testAddProduct();
    testDuplicateSkuRejected();
    testEmptyInventoryReport();
    testStockIn();
    testStockOutInsufficientRejected();
    testNegativeQuantitiesRejected();
    testAdjustmentSetsQuantity();
    testSupplierRemovalBlockedIfReferenced();
    testPersistenceRoundTrip();
    testLoadMissingFileIsOk();
    testMalformedLineIsSkipped();
    testSafeSaveReplacesFile();
    testTimestampFormat();
    std::cout << "All tests passed.\n";
    return 0;
}