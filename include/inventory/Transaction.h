#pragma once
#include <string>

namespace inventory {

enum class TransactionType { StockIn, StockOut, Adjustment };

inline const char* toString(TransactionType t) {
    switch (t) {
        case TransactionType::StockIn:    return "IN";
        case TransactionType::StockOut:   return "OUT";
        case TransactionType::Adjustment: return "ADJ";
    }
    return "UNKNOWN";
}

inline bool fromString(const std::string& s, TransactionType& out) {
    if (s == "IN")  { out = TransactionType::StockIn;    return true; }
    if (s == "OUT") { out = TransactionType::StockOut;   return true; }
    if (s == "ADJ") { out = TransactionType::Adjustment; return true; }
    return false;
}

struct Transaction {
    int             id{0};
    int             productId{0};
    TransactionType type{TransactionType::StockIn};
    int             quantity{0};
    std::string     timestamp;  // ISO 8601
    std::string     note;
};

} // namespace inventory