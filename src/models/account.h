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
  Varchar<30> userId;
  Varchar<30> password;
  Varchar<30> name;
  Privilege privilege;
  Account() = default;
  Account(const string&, const string&, const string&, Privilege);
  void check();
};

#endif