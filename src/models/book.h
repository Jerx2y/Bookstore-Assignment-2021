#ifndef BOOKSOTRE_ASSIGNMENT_BOOK_H_
#define BOOKSOTRE_ASSIGNMENT_BOOK_H_

#include <string>

#include "../../lib/varchar.h"
#include "../storage.h"
#include "../ull.h"
#include "account.h"

using std::string;

class Book {
 public:
  Varchar<20> isbn_;
  Varchar<60> name_;
  Varchar<60> author_;
  long long stock_;
  double price_;
  Book();
};

#endif