#include "transaction.h"

Transaction::Transaction() { opt_ = var_ = 0; }

Transaction::Transaction(double var) {
  if (var < 0)
    opt_ = -1, var_ = -var;
  else
    opt_ = 1, var_ = var;
}