#include "inventory/Inventory.h"

#include <algorithm>

namespace inventory {

void Inventory::rebuildIndexes() {
    productIndexById_.clear();
    productIndexBySku_.clear();
    supplierIndexById_.clear();

    for (std::size_t i = 0; i < products_.size(); ++i) {
        productIndexById_[products_[i].id] = i;
        productIndexBySku_[products_[i].sku] = i;
    }
    for (std::size_t i = 0; i < suppliers_.size(); ++i) {
        supplierIndexById_[suppliers_[i].id] = i;
    }
}

bool Inventory::addProduct(Product p) {
    if (p.id == 0) p.id = nextProductId();
    if (productIndexById_.count(p.id)) return false;
    if (productIndexBySku_.count(p.sku)) return false;
    products_.push_back(std::move(p));
    rebuildIndexes();
    return true;
}

bool Inventory::editProduct(const Product& p) {
    auto it = productIndexById_.find(p.id);
    if (it == productIndexById_.end()) return false;
    products_[it->second] = p;
    rebuildIndexes();
    return true;
}

bool Inventory::removeProduct(int id) {
    auto it = productIndexById_.find(id);
    if (it == productIndexById_.end()) return false;
    products_.erase(products_.begin() + static_cast<long>(it->second));
    rebuildIndexes();
    return true;
}

const Product* Inventory::findProductById(int id) const {
    auto it = productIndexById_.find(id);
    if (it == productIndexById_.end()) return nullptr;
    return &products_[it->second];
}

const Product* Inventory::findProductBySku(const std::string& sku) const {
    auto it = productIndexBySku_.find(sku);
    if (it == productIndexBySku_.end()) return nullptr;
    return &products_[it->second];
}

std::vector<Product> Inventory::listProducts() const { return products_; }

bool Inventory::addSupplier(Supplier s) {
    if (s.id == 0) s.id = nextSupplierId();
    if (supplierIndexById_.count(s.id)) return false;
    suppliers_.push_back(std::move(s));
    rebuildIndexes();
    return true;
}

bool Inventory::editSupplier(const Supplier& s) {
    auto it = supplierIndexById_.find(s.id);
    if (it == supplierIndexById_.end()) return false;
    suppliers_[it->second] = s;
    return true;
}

bool Inventory::removeSupplier(int id) {
    auto it = supplierIndexById_.find(id);
    if (it == supplierIndexById_.end()) return false;
    // Refuse if any product references this supplier
    for (const auto& p : products_) {
        if (p.supplierId == id) return false;
    }
    suppliers_.erase(suppliers_.begin() + static_cast<long>(it->second));
    rebuildIndexes();
    return true;
}

const Supplier* Inventory::findSupplierById(int id) const {
    auto it = supplierIndexById_.find(id);
    if (it == supplierIndexById_.end()) return nullptr;
    return &suppliers_[it->second];
}

std::vector<Supplier> Inventory::listSuppliers() const { return suppliers_; }

bool Inventory::recordTransaction(Transaction t) {
    auto it = productIndexById_.find(t.productId);
    if (it == productIndexById_.end()) return false;

    Product& p = products_[it->second];

    switch (t.type) {
        case TransactionType::StockIn:
            if (t.quantity <= 0) return false;
            p.quantity += t.quantity;
            break;
        case TransactionType::StockOut:
            if (t.quantity <= 0) return false;
            if (p.quantity < t.quantity) return false;
            p.quantity -= t.quantity;
            break;
        case TransactionType::Adjustment:
            if (t.quantity < 0) return false;
            p.quantity = t.quantity;
            break;
    }

    if (t.id == 0) t.id = nextTransactionId();
    transactions_.push_back(std::move(t));
    return true;
}

std::vector<Transaction> Inventory::listTransactions() const { return transactions_; }

std::vector<Transaction> Inventory::listTransactionsForProduct(int productId) const {
    std::vector<Transaction> result;
    for (const auto& t : transactions_) {
        if (t.productId == productId) result.push_back(t);
    }
    return result;
}

void Inventory::clear() {
    products_.clear();
    suppliers_.clear();
    transactions_.clear();
    rebuildIndexes();
}

void Inventory::restore(std::vector<Supplier>    suppliers,
                        std::vector<Product>     products,
                        std::vector<Transaction> transactions) {
    suppliers_    = std::move(suppliers);
    products_     = std::move(products);
    transactions_ = std::move(transactions);
    rebuildIndexes();
}

int Inventory::nextProductId() const {
    int maxId = 0;
    for (const auto& p : products_) maxId = std::max(maxId, p.id);
    return maxId + 1;
}

int Inventory::nextSupplierId() const {
    int maxId = 0;
    for (const auto& s : suppliers_) maxId = std::max(maxId, s.id);
    return maxId + 1;
}

int Inventory::nextTransactionId() const {
    int maxId = 0;
    for (const auto& t : transactions_) maxId = std::max(maxId, t.id);
    return maxId + 1;
}

} // namespace inventory