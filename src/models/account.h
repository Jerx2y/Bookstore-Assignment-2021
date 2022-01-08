#ifndef BOOKSTORE_ASSIGNMENT_ACCOUNT_H_
#define BOOKSTORE_ASSIGNMENT_ACCOUNT_H_

#include <stack>
#include <string>
#include <unordered_map>

#include "../../lib/varchar.h"
#include "../storage.h"
#include "../ull.h"
#include "book.h"

using std::string;

enum Privilege { CUSTOMER = 1, WORKER = 3, ROOT = 7 };

class Account {
 public:
  Varchar<30> userId_;
  Varchar<30> password_;
  Varchar<30> name_;
  Privilege privilege_;
  Account() = default;
  Account(const string&, const string&, const string&, Privilege);
  void check();
};

#endif