#include <iostream>
#include <iomanip>
#include "commands.h"

Atom::Atom(const Account &account_, const int & book_) : account(account_), book(book_) { }

void AccountStack::push(const Account &u) {
    instack[u.userId.str()]++;
    online.push(Atom(u, -1));
}

void AccountStack::pop() {
    if (online.empty()) throw Exception("No User Online");
    instack[online.top().account.userId.str()]--;
    online.pop();
}

void AccountStack::check(int p) {
    if (online.empty() || online.top().account.privilege <= p)
        throw Exception("Has Not Priority");
}

bool AccountStack::empty() {
    return online.empty();
}

Privilege AccountStack::getPriority() {
    return online.top().account.privilege;
}

bool AccountStack::loggedin(const string &u) {
    return instack[u];
}

int AccountStack::selected() {
    if (online.empty())
        throw Exception("No user!");
    if (online.top().book == -1)
        throw Exception("No Books are selected");
    return online.top().book;
}

void AccountStack::select(const int &offset) {
    online.top().book = offset;
}

AccountStack stack;

Storage<Account> user;
Ull userid;

Storage<Book> book;
Storage<Varchar<60> > keyword;
Ull keywordisbn, bookkeyword, bookisbn, bookauthor, bookname;

Storage<Transaction> transaction;

void init() {
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
}

// Account

void addAccount(const string &id, const string &password, Privilege priority, const string &name) {
    std::vector<int> res;
    Varchar<30> user_id(id);
    userid.query(user_id, res);
    if (res.size())
        throw Exception("Userid already exist");
    Account now(id, password, name, priority);
    int offset = user.write(now);
    userid.insert(now.userId, now.userId, offset);
}

void getAccount(const string &id, Account &now, int &offset) {
    Varchar<30> user_id(id);
    std::vector<int> res;
    userid.query(user_id, res);
    if (res.empty()) throw Exception("Find Nothing");
    assert(res.size() == 1);
//    if (res.size() != 1) {
//        std::cout << res.size() << std::endl;
//        for (auto it : res) {
//            user.read(now, it);
//            std::cout << now.userId.str() << " " << now.name.str() << " " << now.password.str() << " # " << std::endl;
//        }
//        assert(0);
//    }
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
    userid.erase(now.userId, now.userId, offset);
}

void login(const string &id) {
    Account now;
    int offset;
    getAccount(id, now, offset);
    stack.check(now.privilege);
    stack.push(now);
}

void login(const string &id, const string &password) {
    Account now;
    int offset;
    getAccount(id, now, offset);
    if (now.password.str() != password)
        throw Exception("su: Password Wrong to login: " + now.password.str() + " instead of " + password);
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
    now.password = newpassword;
    user.update(now, offset);
}

void changePassword(const string &id, const string &oldpassword, const string &newpassword) {
    stack.check(0);
    Account now;
    int offset;
    getAccount(id, now, offset);
    if (now.password.str() != oldpassword)
        throw Exception("passwd: wrong password to change password");
    now.password = newpassword;
    user.update(now, offset);
}

void userAdd(const string &id, const string &password, Privilege priority, const string &name) {
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
    now.stock += quantity;
    book.update(now, offset);
}

void modifyBook(const vector<string> &var) {
    stack.check(2);
    int offset = stack.selected();
    Book now;
    book.read(now, offset);
    multiVarCheck(var, bookisbn, now.isbn.str());
    for (const auto &com : var)
        multiKeywordCheck(com);
    for (const auto &com : var) {
        Option opt;
        string res;
        getCommand(com, opt, res);
        if (opt == ISBN) {
            checkLen(res, 20);
            bookisbn.erase(now.isbn, now.isbn, offset);
            vector<int> kres;
            keywordisbn.query(now.isbn, kres);
            int cnt = 0;
            for (int tnow : kres) {
                Varchar<2> order(inttostring(++cnt));
                keywordisbn.erase(now.isbn, order, tnow);
            }
            now.isbn = res;
            cnt = 0;
            for (int tnow : kres) {
                Varchar<2> order(inttostring(++cnt));
                keywordisbn.insert(now.isbn, order, tnow);
            }
            bookisbn.insert(now.isbn, now.isbn, offset);
            book.update(now, offset);
        } else if (opt == AUTHOR) {
            checkLen(res, 60);
            if (!now.author.empty())
                bookauthor.erase(now.author, now.isbn, offset);
            now.author = res;
            bookauthor.insert(now.author, now.isbn, offset);
            book.update(now, offset);
        } else if (opt == NAME) {
            checkLen(res, 60);
            if (!now.name.empty())
                bookname.erase(now.name, now.isbn, offset);
            now.name = res;
            bookname.insert(now.name, now.isbn, offset);
            book.update(now, offset);
        } else if (opt == KEYWORD) {
            checkLen(res, 60);
            int cnt = 0;
            vector<int> vec;
            keywordisbn.query(now.isbn, vec);
            for (auto k : vec) {
                Varchar<2> order(inttostring(++cnt));
                Varchar<60> kwd;
                keyword.read(kwd, k);
                bookkeyword.erase(kwd, now.isbn, offset);
                keywordisbn.erase(now.isbn, order, k);
            }

            res += '|';
            cnt = 0;
            string tmp;
            for (int i = 0, sz = res.size(); i < sz; ++i) {
                if (res[i] == '|') {
                    Varchar<60> keyw(tmp);
                    Varchar<2> order(inttostring(++cnt));
                    int offsetk = keyword.write(keyw);
                    bookkeyword.insert(keyw, now.isbn, offset);
                    keywordisbn.insert(now.isbn, order, offsetk);
                    tmp.clear();
                } else tmp += res[i];
            }
        } else if (opt == PRICE) {
            now.price = std::stod(res);
            book.update(now, offset);
        }
    }
}

void buyBook(const string &isbn, const long long &quantity) {
    stack.check(0);
    Varchar<20> nowisbn(isbn);
    vector<int> offset;
    bookisbn.query(nowisbn, offset);
    assert(offset.size() <= 1);
    if (!offset.size())
        throw Exception("buy: book doesn't exist");
    Book now;
    book.read(now, offset[0]);
    if (now.stock < quantity)
        throw Exception("buy: book is not enough");
    takeFinance(quantity * now.price);
    std::cout << std::setiosflags(std::ios::fixed) <<\
        std::setprecision(2) << quantity * now.price << std::endl;
    now.stock -= quantity;
    book.update(now, offset[0]);
}

void printBook(Book &now) {
    vector<int> kres;
    keywordisbn.query(now.isbn, kres);
    string keyw;
    for (int offsetk : kres) {
        Varchar<60> tmp;
        keyword.read(tmp, offsetk);
        keyw += tmp.str();
        keyw += '|';
    }
    if (!keyw.empty()) keyw.pop_back();
    std::cout << now.isbn.str() << '\t' << now.name.str() << '\t' \
        << now.author.str() << '\t' << keyw << '\t' << std::setiosflags(std::ios::fixed) \
        << std::setprecision(2) << now.price << '\t' << now.stock << std::endl;
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
    if (res.empty())
        std::cout << std::endl;
}

void showBook(const string &cmd) {
    stack.check(0);
    Option opt; string val;
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
            if (at == '|')
                throw Exception("show -keywrod: So many keywords");
        Varchar<60> keywd(val);
        bookkeyword.query(keywd, offset);
    } else throw Exception("error");
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
        now.isbn = nowisbn;
        int res = book.write(now);
        bookisbn.insert(nowisbn, nowisbn, res);
        stack.select(res);
    } else stack.select(offset[0]);
}

// Finance

void showFinance(int t) {
    stack.check(4);
    if (!t)
        return std::cout << std::endl, void();

    int total;
    transaction.get_info(total, 1);
    if (t > total)
        throw Exception("show finance: too many times");

    if (t == -1) t = total;

    double income = 0, expense = 0;
    Transaction tmp;
    int delta = sizeof(Transaction);
    int pos = (total - t) * delta + sizeof(int);
    while (t--) {
        transaction.read(tmp, pos);
        if (tmp.opt == 1) income += tmp.var;
        else expense += tmp.var;
        pos += delta;
    }
    std::cout << std::setiosflags(std::ios::fixed) << std::setprecision(2)\
        << "+ " << income << " - " << expense << std::endl;
}

void takeFinance(double var) {
    int total;
    transaction.get_info(total, 1);
    ++total;
    transaction.write_info(total, 1);
    Transaction now(var);
    transaction.write(now);
}