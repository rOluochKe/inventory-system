#include "inventory/Report.h"

#include <sstream>

namespace inventory {

std::string Report::currentStock(const Inventory& inv) {
    std::ostringstream os;
    os << "=== Current Stock ===\n";
    for (const auto& p : inv.products()) {
        os << p.sku << "  " << p.name << "  qty=" << p.quantity << '\n';
    }
    return os.str();
}

std::string Report::lowStock(const Inventory& inv) {
    std::ostringstream os;
    os << "=== Low Stock Items ===\n";
    for (const auto& p : inv.products()) {
        if (p.quantity <= p.reorderLevel) {
            os << p.sku << "  " << p.name << "  qty=" << p.quantity
               << "  reorder=" << p.reorderLevel << '\n';
        }
    }
    return os.str();
}

std::string Report::transactionHistory(const Inventory& inv) {
    std::ostringstream os;
    os << "=== Transaction History ===\n";
    for (const auto& t : inv.transactions()) {
        os << t.id << "  product=" << t.productId
           << "  " << toString(t.type) << "  qty=" << t.quantity
           << "  " << t.timestamp << "  " << t.note << '\n';
    }
    return os.str();
}

} // namespace inventory