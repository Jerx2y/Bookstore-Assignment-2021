#include "transaction.h"

Transaction::Transaction() { opt = var = 0; }

Transaction::Transaction(double var_) {
    if (var_ < 0) opt = -1, var = -var_;
    else opt = 1, var = var_;
}