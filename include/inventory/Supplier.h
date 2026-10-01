#pragma once
#include <string>

namespace inventory {

struct Supplier {
    int         id{0};
    std::string name;
    std::string contact;
    std::string email;
};

} // namespace inventory