#include "inventory/Persistence.h"

#include <filesystem>
#include <fstream>
#include <sstream>

namespace inventory {

Persistence::Persistence(std::string path) : path_(std::move(path)) {}

namespace {

std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> out;
    std::string cur;
    std::istringstream ss(line);
    while (std::getline(ss, cur, delim)) out.push_back(cur);
    return out;
}

} // namespace

bool Persistence::load(Inventory& inv, std::string& errorMessage) const {
    inv.clear();
    std::ifstream in(path_);
    if (!in.is_open()) {
        // Missing file is acceptable: start empty.
        return true;
    }

    std::vector<Supplier>    suppliers;
    std::vector<Product>     products;
    std::vector<Transaction> transactions;

    std::string line;
    enum class Section { None, Suppliers, Products, Transactions };
    Section section = Section::None;

    while (std::getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        if (line == "[SUPPLIERS]")    { section = Section::Suppliers;    continue; }
        if (line == "[PRODUCTS]")     { section = Section::Products;     continue; }
        if (line == "[TRANSACTIONS]") { section = Section::Transactions; continue; }

        // Skip header row of each section
        if (line.rfind("id|", 0) == 0) continue;

        auto parts = split(line, '|');
        try {
            switch (section) {
                case Section::Suppliers: {
                    if (parts.size() < 4) break;
                    Supplier s;
                    s.id      = std::stoi(parts[0]);
                    s.name    = parts[1];
                    s.contact = parts[2];
                    s.email   = parts[3];
                    suppliers.push_back(std::move(s));
                    break;
                }
                case Section::Products: {
                    if (parts.size() < 8) break;
                    Product p;
                    p.id           = std::stoi(parts[0]);
                    p.sku          = parts[1];
                    p.name         = parts[2];
                    p.category     = parts[3];
                    p.quantity     = std::stoi(parts[4]);
                    p.reorderLevel = std::stoi(parts[5]);
                    p.price        = std::stod(parts[6]);
                    p.supplierId   = std::stoi(parts[7]);
                    products.push_back(std::move(p));
                    break;
                }
                case Section::Transactions: {
                    if (parts.size() < 6) break;
                    Transaction t;
                    t.id        = std::stoi(parts[0]);
                    t.productId = std::stoi(parts[1]);
                    if (!fromString(parts[2], t.type)) break;
                    t.quantity  = std::stoi(parts[3]);
                    t.timestamp = parts[4];
                    t.note      = parts[5];
                    transactions.push_back(std::move(t));
                    break;
                }
                case Section::None:
                    break;
            }
        } catch (const std::exception& e) {
            errorMessage = std::string("Malformed line skipped: ") + line
                         + " (" + e.what() + ")";
            // Continue; a single bad line should not abort the whole load.
        }
    }

    inv.restore(std::move(suppliers), std::move(products), std::move(transactions));
    return true;
}

bool Persistence::save(const Inventory& inv, std::string& errorMessage) const {
    std::filesystem::path p(path_);
    if (p.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(p.parent_path(), ec);
    }

    std::ofstream out(path_, std::ios::trunc);
    if (!out.is_open()) {
        errorMessage = "Cannot open file for writing: " + path_;
        return false;
    }

    out << "[SUPPLIERS]\n";
    out << "id|name|contact|email\n";
    for (const auto& s : inv.suppliers()) {
        out << s.id << '|' << s.name << '|' << s.contact << '|' << s.email << '\n';
    }

    out << "\n[PRODUCTS]\n";
    out << "id|sku|name|category|quantity|reorder_level|price|supplier_id\n";
    for (const auto& p : inv.products()) {
        out << p.id << '|' << p.sku << '|' << p.name << '|' << p.category << '|'
            << p.quantity << '|' << p.reorderLevel << '|' << p.price << '|'
            << p.supplierId << '\n';
    }

    out << "\n[TRANSACTIONS]\n";
    out << "id|product_id|type|quantity|timestamp|note\n";
    for (const auto& t : inv.transactions()) {
        out << t.id << '|' << t.productId << '|' << toString(t.type) << '|'
            << t.quantity << '|' << t.timestamp << '|' << t.note << '\n';
    }

    return static_cast<bool>(out);
}

} // namespace inventory