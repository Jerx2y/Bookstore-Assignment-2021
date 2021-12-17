#ifndef BOOKSTORE_ASSIGNMENT_UTILS_H_
#define BOOKSTORE_ASSIGNMENT_UTILS_H_

#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <unordered_map>

#include "../lib/exception.h"
#include "ull.h"

using std::string;

enum Option { ISBN, NAME, AUTHOR, KEYWORD, PRICE };

void getCommand(const string &, Option &, string &);

void checkLen(const string &, const int &);

bool getCommand(std::vector<string> &);

void multiKeywordCheck(const string &);

void multiVarCheck(const std::vector<string> &, Ull&);

#endif