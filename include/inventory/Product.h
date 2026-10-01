#pragma once
#include <string>

namespace inventory {

struct Product {
    int         id{0};
    std::string sku;
    std::string name;
    std::string category;
    int         quantity{0};
    int         reorderLevel{0};
    double      price{0.0};
    int         supplierId{0};
};

} // namespace inventory