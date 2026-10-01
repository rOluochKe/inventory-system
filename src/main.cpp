#include <iostream>
#include <limits>
#include <string>

#include "inventory/Inventory.h"
#include "inventory/Persistence.h"
#include "inventory/Report.h"
#include <algorithm>

using namespace inventory;

namespace {

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readInt(const std::string& prompt) {
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) { clearInput(); return v; }
        std::cout << "  ! Please enter a whole number.\n";
        clearInput();
    }
}

double readDouble(const std::string& prompt) {
    double v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) { clearInput(); return v; }
        std::cout << "  ! Please enter a number.\n";
        clearInput();
    }
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

void printProducts(const std::vector<Product>& ps) {
    if (ps.empty()) { std::cout << "  (no products)\n"; return; }
    std::cout << "  id | sku        | name                 | cat       | qty | reorder | price | sup\n";
    for (const auto& p : ps) {
        std::printf("  %-3d| %-10s | %-20s | %-9s | %-3d | %-7d | %-5.2f | %d\n",
                    p.id, p.sku.c_str(), p.name.c_str(), p.category.c_str(),
                    p.quantity, p.reorderLevel, p.price, p.supplierId);
    }
}

void productsMenu(Inventory& inv) {
    while (true) {
        std::cout << "\n--- Products ---\n"
                     "1. Add  2. Edit  3. Remove  4. List  5. Search  6. Sort  0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;

        if (c == 1) {
            Product p;
            p.sku          = readLine("SKU: ");
            p.name         = readLine("Name: ");
            p.category     = readLine("Category: ");
            p.quantity     = readInt("Quantity: ");
            p.reorderLevel = readInt("Reorder level: ");
            p.price        = readDouble("Price: ");
            p.supplierId   = readInt("Supplier id (0 = none): ");
            if (p.quantity < 0 || p.reorderLevel < 0 || p.price < 0) {
                std::cout << "  ! Negative values are not allowed.\n";
            } else if (inv.addProduct(p)) {
                std::cout << "  + Added.\n";
            } else {
                std::cout << "  ! Duplicate SKU.\n";
            }
        } else if (c == 2) {
            int id = readInt("Product id: ");
            const Product* existing = inv.findProductById(id);
            if (!existing) { std::cout << "  ! Not found.\n"; continue; }
            Product p = *existing;
            std::string s;
            s = readLine("Name [" + p.name + "]: ");        if (!s.empty()) p.name = s;
            s = readLine("Category [" + p.category + "]: ");if (!s.empty()) p.category = s;
            p.reorderLevel = readInt(("Reorder level [" + std::to_string(p.reorderLevel) + "]: ").c_str());
            p.price        = readDouble(("Price [" + std::to_string(p.price) + "]: ").c_str());
            if (inv.editProduct(p)) std::cout << "  + Updated.\n";
            else                    std::cout << "  ! Update failed.\n";
        } else if (c == 3) {
            int id = readInt("Product id: ");
            std::cout << (inv.removeProduct(id) ? "  + Removed.\n" : "  ! Not found.\n");
        } else if (c == 4) {
            printProducts(inv.listProducts());
        } else if (c == 5) {
            std::string q = readLine("Search (sku/name/category): ");
            std::vector<Product> hits;
            for (const auto& p : inv.products()) {
                if (p.sku.find(q) != std::string::npos ||
                    p.name.find(q) != std::string::npos ||
                    p.category.find(q) != std::string::npos) hits.push_back(p);
            }
            printProducts(hits);
        } else if (c == 6) {
            auto ps = inv.listProducts();
            std::cout << "Sort by: 1. name  2. quantity  3. price\n";
            int k = readInt("Choice: ");
            std::sort(ps.begin(), ps.end(), [k](const Product& a, const Product& b) {
                if (k == 2) return a.quantity < b.quantity;
                if (k == 3) return a.price    < b.price;
                return a.name < b.name;
            });
            printProducts(ps);
        }
    }
}

void suppliersMenu(Inventory& inv) {
    while (true) {
        std::cout << "\n--- Suppliers ---\n"
                     "1. Add  2. Remove  3. List  0. Back\n";
        int c = readInt("Choice: ");
        if (c == 0) return;
        if (c == 1) {
            Supplier s;
            s.name    = readLine("Name: ");
            s.contact = readLine("Contact: ");
            s.email   = readLine("Email: ");
            std::cout << (inv.addSupplier(s) ? "  + Added.\n" : "  ! Duplicate id.\n");
        } else if (c == 2) {
            int id = readInt("Supplier id: ");
            if (!inv.removeSupplier(id))
                std::cout << "  ! Not found, or still referenced by a product.\n";
            else
                std::cout << "  + Removed.\n";
        } else if (c == 3) {
            for (const auto& s : inv.suppliers())
                std::cout << "  " << s.id << " | " << s.name
                          << " | " << s.contact << " | " << s.email << '\n';
        }
    }
}

void transactionsMenu(Inventory& inv) {
    std::cout << "\n--- Transactions ---\n"
                 "1. Stock-in  2. Stock-out  3. Adjustment  4. History  5. Per product  0. Back\n";
    int c = readInt("Choice: ");
    if (c == 0) return;

    if (c >= 1 && c <= 3) {
        Transaction t;
        t.productId = readInt("Product id: ");
        t.quantity  = readInt("Quantity: ");
        t.note      = readLine("Note: ");
        t.timestamp = "2026-09-29T12:00:00"; // replace with a real clock later
        if (c == 1) t.type = TransactionType::StockIn;
        if (c == 2) t.type = TransactionType::StockOut;
        if (c == 3) t.type = TransactionType::Adjustment;
        std::cout << (inv.recordTransaction(t) ? "  + Recorded.\n" : "  ! Rejected.\n");
    } else if (c == 4) {
        for (const auto& t : inv.transactions())
            std::cout << "  " << t.id << " | p" << t.productId << " | " << toString(t.type)
                      << " | " << t.quantity << " | " << t.timestamp << " | " << t.note << '\n';
    } else if (c == 5) {
        int id = readInt("Product id: ");
        for (const auto& t : inv.listTransactionsForProduct(id))
            std::cout << "  " << t.id << " | " << toString(t.type)
                      << " | " << t.quantity << " | " << t.timestamp << " | " << t.note << '\n';
    }
}

void reportsMenu(const Inventory& inv) {
    std::cout << "\n--- Reports ---\n1. Current stock  2. Low stock  3. History  0. Back\n";
    int c = readInt("Choice: ");
    if (c == 1) std::cout << Report::currentStock(inv);
    if (c == 2) std::cout << Report::lowStock(inv);
    if (c == 3) std::cout << Report::transactionHistory(inv);
}

} // namespace

int main(int argc, char** argv) {
    const std::string dataPath = (argc > 1) ? argv[1] : "data/inventory.dat";
    Inventory inv;
    Persistence persistence(dataPath);

    std::string err;
    persistence.load(inv, err);
    if (!err.empty()) std::cerr << "[load warning] " << err << '\n';

    while (true) {
        std::cout << "\n=== Inventory Management System ===\n"
                     "1. Products  2. Suppliers  3. Transactions\n"
                     "4. Reports   5. Save       6. Exit\n";
        int c = readInt("Choice: ");
        if (c == 6) break;
        if (c == 1) productsMenu(inv);
        if (c == 2) suppliersMenu(inv);
        if (c == 3) transactionsMenu(inv);
        if (c == 4) reportsMenu(inv);
        if (c == 5) {
            if (persistence.save(inv, err)) std::cout << "  + Saved.\n";
            else { std::cerr << "  ! " << err << '\n'; }
        }
    }

    if (persistence.save(inv, err)) std::cout << "Saved. Bye.\n";
    else                            std::cerr << "Save failed: " << err << '\n';
    return 0;
}