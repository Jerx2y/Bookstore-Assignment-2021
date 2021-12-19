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

bool getCommand(string&, std::vector<string> &);

void multiKeywordCheck(const string &);

void multiVarCheck(const vector<string> &, Ull&, const string&);

void checkint(const string&, int);

void checkdouble(const string&, int);

void checkstring1(const string&, int);

void checkstring2(const string&, int, char = 0);

string inttostring(int);

long long toint(const string&, int);

#endif
