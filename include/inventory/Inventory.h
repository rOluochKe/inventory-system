#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "inventory/Product.h"
#include "inventory/Supplier.h"
#include "inventory/Transaction.h"

namespace inventory {

class Inventory {
public:
    // Product operations
    bool addProduct(Product p);
    bool editProduct(const Product& p);
    bool removeProduct(int id);
    const Product* findProductById(int id) const;
    const Product* findProductBySku(const std::string& sku) const;
    std::vector<Product> listProducts() const;

    // Supplier operations
    bool addSupplier(Supplier s);
    bool editSupplier(const Supplier& s);
    bool removeSupplier(int id);
    const Supplier* findSupplierById(int id) const;
    std::vector<Supplier> listSuppliers() const;

    // Transaction operations
    bool recordTransaction(Transaction t); // updates product quantity
    std::vector<Transaction> listTransactions() const;
    std::vector<Transaction> listTransactionsForProduct(int productId) const;

    // Direct access (for persistence)
    const std::vector<Product>&     products()     const { return products_; }
    const std::vector<Supplier>&    suppliers()    const { return suppliers_; }
    const std::vector<Transaction>& transactions() const { return transactions_; }

    void clear();

    // Loaded by persistence layer
    void restore(std::vector<Supplier>    suppliers,
                 std::vector<Product>     products,
                 std::vector<Transaction> transactions);

    int nextProductId()     const;
    int nextSupplierId()    const;
    int nextTransactionId() const;

private:
    std::vector<Product>     products_;
    std::vector<Supplier>    suppliers_;
    std::vector<Transaction> transactions_;

    std::unordered_map<int, std::size_t>         productIndexById_;
    std::unordered_map<std::string, std::size_t> productIndexBySku_;
    std::unordered_map<int, std::size_t>         supplierIndexById_;

    void rebuildIndexes();
};

} // namespace inventory