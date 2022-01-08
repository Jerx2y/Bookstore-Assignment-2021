#include "book.h"

Book::Book() { stock_ = price_ = 0, name_ = author_ = Varchar<2>(); }