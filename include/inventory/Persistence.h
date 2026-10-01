#pragma once
#include <string>

#include "inventory/Inventory.h"

namespace inventory {

class Persistence {
public:
    explicit Persistence(std::string path);

    // Returns true on success. If the file does not exist, returns true
    // and leaves the inventory empty.
    bool load(Inventory& inv, std::string& errorMessage) const;

    // Returns true on success. Creates parent directories if needed.
    bool save(const Inventory& inv, std::string& errorMessage) const;

    const std::string& path() const { return path_; }

private:
    std::string path_;
};

} // namespace inventory