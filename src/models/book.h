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
  Varchar<20> isbn;
  Varchar<60> name;
  Varchar<60> author;
  int stock;
  double price;
  Book();
};

#endif