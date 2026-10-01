#pragma once
#include <string>

#include "inventory/Inventory.h"

namespace inventory {

class Report {
public:
    static std::string currentStock(const Inventory& inv);
    static std::string lowStock(const Inventory& inv);
    static std::string transactionHistory(const Inventory& inv);
};

} // namespace inventory