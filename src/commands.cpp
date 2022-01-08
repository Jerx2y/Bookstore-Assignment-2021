#include "commands.h"

#include <iomanip>
#include <iostream>

Atom::Atom(const Account &account_, const int &book_)
    : account(account_), book(book_) {}

void AccountStack::push(const Account &u) {
  instack[u.userId_.str()]++;
  online.push(Atom(u, -1));
}

void AccountStack::pop() {
  if (online.empty()) throw Exception("No user online");
  instack[online.top().account.userId_.str()]--;
  online.pop();
}

void AccountStack::check(int p) {
  if (online.empty() || online.top().account.privilege_ <= p)
    throw Exception("You have not enough priority");
}

bool AccountStack::empty() { return online.empty(); }

Privilege AccountStack::getPriority() { return online.top().account.privilege_; }

bool AccountStack::loggedin(const string &u) { return instack[u]; }

int AccountStack::selected() {
  if (online.empty()) throw Exception("No user!");
  if (online.top().book == -1) throw Exception("No Books are selected");
  return online.top().book;
}

void AccountStack::select(const int &offset) { online.top().book = offset; }

void AccountStack::getTop(std::string &id) {
  if (online.empty())
    id = "<anon>";
  else
    id = online.top().account.userId_;
}

AccountStack stack;

Storage<Account> user;
Ull userid;

Storage<Book> book;
Storage<Varchar<60> > keyword;
Ull keywordisbn, bookkeyword, bookisbn, bookauthor, bookname;

Storage<Transaction> transaction;

void init() {
#ifdef LOCAL
  bool res = user.initialise("./rundata/accounts");
  userid.initialize("./rundata/accounts.id");
  if (!res) addAccount("root", "sjtu", ROOT, "");

  book.initialise("./rundata/books");
  bookkeyword.initialize("./rundata/books.keyword");
  bookisbn.initialize("./rundata/books.isbn");
  bookauthor.initialize("./rundata/books.author");
  bookname.initialize("./rundata/books.name");

  keyword.initialise("./rundata/keywords");
  keywordisbn.initialize("./rundata/keyword.isbn");

  transaction.initialise("./rundata/transaction", 1);
#else

  bool res = user.initialise("accounts");
  userid.initialize("accounts.id");
  if (!res) addAccount("root", "sjtu", ROOT, "");

  book.initialise("books");
  bookkeyword.initialize("books.keyword");
  bookisbn.initialize("books.isbn");
  bookauthor.initialize("books.author");
  bookname.initialize("books.name");

  keyword.initialise("keywords");
  keywordisbn.initialize("keyword.isbn");

  transaction.initialise("transaction", 1);

#endif
}

// Account

void addAccount(const string &id, const string &password, Privilege priority,
                const string &name) {
  std::vector<int> res;
  Varchar<30> user_id(id);
  userid.query(user_id, res);
  if (res.size()) throw Exception("Userid already exist");
  Account now(id, password, name, priority);
  int offset = user.write(now);
  userid.insert(now.userId_, now.userId_, offset);
}

void getAccount(const string &id, Account &now, int &offset) {
  Varchar<30> user_id(id);
  std::vector<int> res;
  userid.query(user_id, res);
  if (res.empty()) throw Exception("Find Nothing");
  assert(res.size() == 1);
  user.read(now, res[0]);
  offset = res[0];
}

void deleteAccount(const string &id) {
  stack.check(4);
  Account now;
  int offset;
  getAccount(id, now, offset);
  if (stack.loggedin(id))
    throw Exception("Can not delete account that has logged in");
  userid.erase(now.userId_, now.userId_, offset);
}

void login(const string &id) {
  Account now;
  int offset;
  getAccount(id, now, offset);
  stack.check(now.privilege_);
  stack.push(now);
}

void login(const string &id, const string &password) {
  Account now;
  int offset;
  getAccount(id, now, offset);
  if (now.password_.str() != password)
    throw Exception("su: Wrong password to login");
  stack.push(now);
}

void logout() {
  stack.check(0);
  stack.pop();
}

void Register(const string &id, const string &password, const string &name) {
  addAccount(id, password, CUSTOMER, name);
}

void changePassword(const string &id, const string &newpassword) {
  stack.check(4);
  Account now;
  int offset;
  getAccount(id, now, offset);
  now.password_ = newpassword;
  user.update(now, offset);
}

void changePassword(const string &id, const string &oldpassword,
                    const string &newpassword) {
  stack.check(0);
  Account now;
  int offset;
  getAccount(id, now, offset);
  if (now.password_.str() != oldpassword)
    throw Exception("passwd: wrong password to change password");
  now.password_ = newpassword;
  user.update(now, offset);
}

void userAdd(const string &id, const string &password, Privilege priority,
             const string &name) {
  stack.check(2);
  if (stack.getPriority() <= priority)
    throw Exception("Can not add user not less than you");
  addAccount(id, password, priority, name);
}

// Book

void addBook(const long long &quantity) {
  stack.check(2);
  int offset = stack.selected();
  Book now;
  book.read(now, offset);
  now.stock_ += quantity;
  book.update(now, offset);
}

void modifyBook(const vector<string> &var) {
  stack.check(2);
  int offset = stack.selected();
  Book now;
  book.read(now, offset);
  multiVarCheck(var, bookisbn, now.isbn_.str());
  for (const auto &com : var) multiKeywordCheck(com);
  for (const auto &com : var) {
    Option opt;
    string res;
    getCommand(com, opt, res);
    if (opt == ISBN) {
      checkLen(res, 20);
      bookisbn.erase(now.isbn_, now.isbn_, offset);
      vector<int> kres;
      keywordisbn.query(now.isbn_, kres);
      int cnt = 0;
      for (int tnow : kres) {
        Varchar<2> order(inttostring(++cnt));
        keywordisbn.erase(now.isbn_, order, tnow);
      }
      now.isbn_ = res;
      cnt = 0;
      for (int tnow : kres) {
        Varchar<2> order(inttostring(++cnt));
        keywordisbn.insert(now.isbn_, order, tnow);
      }
      bookisbn.insert(now.isbn_, now.isbn_, offset);
      book.update(now, offset);
    } else if (opt == AUTHOR) {
      checkLen(res, 60);
      if (!now.author_.empty()) bookauthor.erase(now.author_, now.isbn_, offset);
      now.author_ = res;
      bookauthor.insert(now.author_, now.isbn_, offset);
      book.update(now, offset);
    } else if (opt == NAME) {
      checkLen(res, 60);
      if (!now.name_.empty()) bookname.erase(now.name_, now.isbn_, offset);
      now.name_ = res;
      bookname.insert(now.name_, now.isbn_, offset);
      book.update(now, offset);
    } else if (opt == KEYWORD) {
      checkLen(res, 60);
      int cnt = 0;
      vector<int> vec;
      keywordisbn.query(now.isbn_, vec);
      for (auto k : vec) {
        Varchar<2> order(inttostring(++cnt));
        Varchar<60> kwd;
        keyword.read(kwd, k);
        bookkeyword.erase(kwd, now.isbn_, offset);
        keywordisbn.erase(now.isbn_, order, k);
      }

      res += '|';
      cnt = 0;
      string tmp;
      for (int i = 0, sz = res.size(); i < sz; ++i) {
        if (res[i] == '|') {
          Varchar<60> keyw(tmp);
          Varchar<2> order(inttostring(++cnt));
          int offsetk = keyword.write(keyw);
          bookkeyword.insert(keyw, now.isbn_, offset);
          keywordisbn.insert(now.isbn_, order, offsetk);
          tmp.clear();
        } else
          tmp += res[i];
      }
    } else if (opt == PRICE) {
      now.price_ = std::stod(res);
      book.update(now, offset);
    }
  }
}

double buyBook(const string &isbn, const long long &quantity) {
  stack.check(0);
  Varchar<20> nowisbn(isbn);
  vector<int> offset;
  bookisbn.query(nowisbn, offset);
  assert(offset.size() <= 1);
  if (!offset.size()) throw Exception("buy: book doesn't exist");
  Book now;
  book.read(now, offset[0]);
  if (now.stock_ < quantity) throw Exception("buy: book is not enough");
  takeFinance(quantity * now.price_);
  std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2)
            << quantity * now.price_ << std::endl;
  now.stock_ -= quantity;
  book.update(now, offset[0]);
  return quantity * now.price_;
}

void printBook(Book &now) {
  vector<int> kres;
  keywordisbn.query(now.isbn_, kres);
  string keyw;
  for (int offsetk : kres) {
    Varchar<60> tmp;
    keyword.read(tmp, offsetk);
    keyw += tmp.str();
    keyw += '|';
  }
  if (!keyw.empty()) keyw.pop_back();
  std::cout << now.isbn_.str() << '\t' << now.name_.str() << '\t'
            << now.author_.str() << '\t' << keyw << '\t'
            << std::setiosflags(std::ios::fixed) << std::setprecision(2)
            << now.price_ << '\t' << now.stock_ << std::endl;
}

void showBook() {
  stack.check(0);
  vector<int> res;
  bookisbn.getall(res);
  for (int offset : res) {
    Book now;
    book.read(now, offset);
    printBook(now);
  }
  if (res.empty()) std::cout << std::endl;
}

void showBook(const string &cmd) {
  stack.check(0);
  Option opt;
  string val;
  getCommand(cmd, opt, val);
  vector<int> offset;
  if (opt == ISBN) {
    Varchar<20> isbn(val);
    bookisbn.query(isbn, offset);
  } else if (opt == NAME) {
    Varchar<60> name(val);
    bookname.query(name, offset);
  } else if (opt == AUTHOR) {
    Varchar<60> author(val);
    bookauthor.query(author, offset);
  } else if (opt == KEYWORD) {
    for (auto at : val)
      if (at == '|') throw Exception("show -keywrod: So many keywords");
    Varchar<60> keywd(val);
    bookkeyword.query(keywd, offset);
  } else
    throw Exception("error");
  for (int k : offset) {
    Book now;
    book.read(now, k);
    printBook(now);
  }
  if (offset.empty()) {
    std::cout << std::endl;
  }
}

void selectBook(const string &isbn) {
  stack.check(2);
  Varchar<20> nowisbn(isbn);
  vector<int> offset;
  bookisbn.query(nowisbn, offset);
  assert(offset.size() <= 1);
  if (offset.size() == 0) {
    Book now;
    now.isbn_ = nowisbn;
    int res = book.write(now);
    bookisbn.insert(nowisbn, nowisbn, res);
    stack.select(res);
  } else
    stack.select(offset[0]);
}

// Finance

void showFinance(int t) {
  stack.check(4);
  if (!t) return std::cout << std::endl, void();

  int total;
  transaction.get_info(total, 1);
  if (t > total) throw Exception("show finance: too many times");

  if (t == -1) t = total;

  double income = 0, expense = 0;
  Transaction tmp;
  int delta = sizeof(Transaction);
  int pos = (total - t) * delta + sizeof(int);
  while (t--) {
    transaction.read(tmp, pos);
    if (tmp.opt_ == 1)
      income += tmp.var_;
    else
      expense += tmp.var_;
    pos += delta;
  }
  std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2) << "+ "
            << income << " - " << expense << std::endl;
}

void takeFinance(double var) {
  int total;
  transaction.get_info(total, 1);
  ++total;
  transaction.write_info(total, 1);
  Transaction now(var);
  transaction.write(now);
}

void showLog(const string &id, bool showall = 0) {
  fstream logfile("./rundata/log", std::ios::in);
  string tmp;
  while (getline(logfile, tmp)) {
    if (showall) {
      std::cout << tmp << std::endl;
      continue;
    }
    string nowuser;
    for (int i = 0, sz = tmp.size(); i < sz && tmp[i] != ' '; ++i)
      nowuser += tmp[i];
    if (nowuser[nowuser.size() - 2] != '3' &&
        nowuser[nowuser.size() - 2] != '7')
      continue;
    if (id.empty() || nowuser.substr(0, nowuser.size() - 3) == id)
      std::cout << tmp << std::endl;
  }
}

void reportMyself() {
  stack.check(2);
  std::string id;
  stack.getTop(id);
  std::cout << "------------YOURSELF-------------\n";
  showLog(id);
  std::cout << "---------------END---------------\n";
}

void reportEmployee() {
  stack.check(4);
  std::cout << "------------EMPLOYEE-------------\n";
  showLog("");
  std::cout << "---------------END---------------\n";
}

void reportFinance() {
  stack.check(4);
  fstream logfile("./rundata/log", std::ios::in);
  string tmp;
  std::cout << "---------FINANCE---------\n";
  while (getline(logfile, tmp)) {
    std::vector<string> cmd;
    getCommand(tmp, cmd);
    if (cmd[3] == "buy")
      std::cout << cmd[0].substr(0, cmd[0].size() - 3) << "\tbuy\t" << cmd[6]
                << std::endl;
    if (cmd[3] == "import")
      std::cout << cmd[0].substr(0, cmd[0].size() - 3) << "\timport\t" << cmd[5]
                << std::endl;
  }
  std::cout << "-----------END-----------\n";
}

void reportLog() {
  stack.check(4);
  std::cout << "---------------LOG---------------\n";
  showLog("", 1);
  std::cout << "---------------END---------------\n";
}

bool run(std::vector<string> command, std::string &userid, int &pri,
         double &buy) {
  stack.getTop(userid);
  if (stack.empty())
    pri = 0;
  else
    pri = stack.getPriority();
  if (command[0] == "exit" || command[0] == "quit") {
    if (command.size() > 1) throw Exception("quit: invalid commands numbers");
    return false;
  } else if (command[0] == "su") {
    if (command.size() == 1 || command.size() > 3)
      throw Exception("su: invalid commands numbers");
    checkstring1(command[1], 30);
    if (command.size() == 2)
      login(command[1]);
    else {
      checkstring2(command[2], 30);
      login(command[1], command[2]);
    }
  } else if (command[0] == "logout") {
    if (command.size() > 1) throw Exception("logout: invalid commands numbers");
    logout();
  } else if (command[0] == "register") {
    if (command.size() != 4)
      throw Exception("register: invalid commands numbers");
    checkstring1(command[1], 30);
    checkstring1(command[2], 30);
    checkstring2(command[3], 30);
    Register(command[1], command[2], command[3]);
  } else if (command[0] == "passwd") {
    if (command.size() != 4 && command.size() != 3)
      throw Exception("passwd: invalid commands numbers");
    checkstring1(command[1], 30);
    checkstring1(command[2], 30);
    if (command.size() == 3)
      changePassword(command[1], command[2]);
    else {
      checkstring1(command[3], 30);
      changePassword(command[1], command[2], command[3]);
    }
  } else if (command[0] == "useradd") {
    if (command.size() != 5)
      throw Exception("useradd: invalid commands numbers");
    Privilege priority;
    if (command[3] == "1")
      priority = CUSTOMER;
    else if (command[3] == "3")
      priority = WORKER;
    else if (command[3] == "7")
      priority = ROOT;
    else
      throw Exception("useradd: user's priority invalid");
    checkstring1(command[1], 30);
    checkstring1(command[2], 30);
    checkstring2(command[4], 30);
    userAdd(command[1], command[2], priority, command[4]);
  } else if (command[0] == "delete") {
    if (command.size() != 2)
      throw Exception("delete: invalid commands numbers");
    checkstring1(command[1], 30);
    deleteAccount(command[1]);
  } else if (command[0] == "show") {  // show book or show finance
    if (command.size() > 3) throw Exception("show: invalid commands numbers");
    if (command.size() == 1)
      showBook();
    else if (command[1] != "finance") {
      if (command.size() != 2)
        throw Exception("show -: invalid commands numbers");
      showBook(command[1]);
    } else if (command.size() == 2)
      showFinance();
    else if (command.size() == 3) {
      checkint(command[2], 10);
      showFinance(toint(command[2], 2147483647));
    }
  } else if (command[0] == "buy") {
    if (command.size() != 3) throw Exception("buy: invalid commands numbers");
    checkstring2(command[1], 20);
    checkint(command[2], 10);
    buy = buyBook(command[1], toint(command[2], 2147483647));
  } else if (command[0] == "select") {
    if (command.size() != 2)
      throw Exception("select: invalid commands numbers");
    checkstring2(command[1], 20);
    selectBook(command[1]);
  } else if (command[0] == "modify") {
    std::vector<string> modi;
    for (int i = 1; i < command.size(); ++i) modi.push_back(command[i]);
    if (modi.empty()) throw Exception("modify: Modify Empty");
    modifyBook(modi);
  } else if (command[0] == "import") {
    if (command.size() != 3)
      throw Exception("import: invalid commands numbers");
    checkint(command[1], 10);
    checkdouble(command[2], 13);
    addBook(toint(command[1], 2147483647));
    takeFinance(-std::stod(command[2]));
  } else if (command[0] == "log") {
    if (command.size() != 1) throw Exception("log: invalid commands numbers");
    reportLog();
  } else if (command[0] == "report") {
    if (command.size() != 2)
      throw Exception("report: invalid commands numbers");
    if (command[1] == "myself")
      reportMyself();
    else if (command[1] == "finance")
      reportFinance();
    else if (command[1] == "employee")
      reportEmployee();
    else
      throw Exception("");
  } else
    throw Exception("Invalid command");
  return true;
}