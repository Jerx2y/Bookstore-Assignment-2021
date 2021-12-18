#ifndef BOOKSTORE_ASSIGNMENT_COMMANDS_H_
#define BOOKSTORE_ASSIGNMENT_COMMANDS_H_

#include <stack>
#include <unordered_map>
#include <string>
#include <cassert>

#include "../../lib/varchar.h"
#include "../storage.h"
#include "../ull.h"
#include "models/account.h"
#include "models/book.h"
#include "models/transaction.h"
#include "utils.h"

using std::string;

// 保存登录栈的元素
struct Atom {
    Account account;
    int book; // the offset of book
    Atom(const Account &, const int &);
};

// 登录栈
class AccountStack{
 private:
  std::stack<Atom> online;
  std::unordered_map<string, int> instack;
 public:
  AccountStack() = default;
  void push(const Account&);
  void pop();
  void check(int);
  bool empty();
  Privilege getPriority();
  bool loggedin(const string &);
  int selected();
  void select(const int &);
};

// Before run
void init();

void addAccount(const string&, const string&, Privilege, const string&);
void getAccount(const string&, Account &, int &);
void deleteAccount(const string&);
void login(const string &);
void login(const string &, const string &);
void logout();
void Register(const string&, const string&, const string&);
void changePassword(const string &, const string &);
void changePassword(const string &, const string &, const string&);
void userAdd(const string&, const string&, Privilege, const string&);

void addBook(const int &);
void modifyBook(const vector<string>&);
void buyBook(const string &, const int &);
void printBook(Book &);
void showBook();
void showBook(const string &);
void selectBook(const string &);

void showFinance(int t = -1);
void takeFinance(double);

void done();

#endif