#include "utils.h"

void getCommand(const string &command, Option &opt, string &var) {
    var.clear();
    if (command[1] == 'I') {
        if (command.size() <= 6)
            throw Exception("modify: -ISBN = NULL");
        if (command.substr(0, 6) != "-ISBN=") 
            throw Exception("error");
        opt = ISBN;
        for (int i = 6, sz = command.size(); i < sz; ++i)
            var += command[i]; 
        checkstring2(var, 20);
    } else if (command[1] == 'n') {
        if (command.size() <= 8)
            throw Exception("modify: -ISBN = NULL");
        if (command.substr(0, 6) != "-name=") 
            throw Exception("error");
        opt = NAME;
        if (command[6] != '"'|| command[command.size() - 1] != '"')
            throw Exception("error");
        for (int i = 7, sz = command.size(); i < sz - 1; ++i)
            var += command[i];
        checkstring2(var, 60, '"');
    } else if (command[1] == 'a') {
        if (command.size() <= 10)
            throw Exception("modify: -ISBN = NULL");
        if (command.substr(0, 8) != "-author=") 
            throw Exception("error");
        opt = AUTHOR;
        if (command[8] != '"'|| command[command.size() - 1] != '"')
            throw Exception("error");
        for (int i = 9, sz = command.size(); i < sz - 1; ++i)
            var += command[i];
        checkstring2(var, 60, '"');
    } else if (command[1] == 'k') {
        if (command.size() <= 11)
            throw Exception("modify: -ISBN = NULL");
        if (command.substr(0, 9) != "-keyword=") 
            throw Exception("error");
        opt = KEYWORD;
        if (command[9] != '"'|| command[command.size() - 1] != '"')
            throw Exception("error");
        for (int i = 10, sz = command.size(); i < sz - 1; ++i)
            var += command[i];
        checkstring2(var, 60, '"');
    } else if (command[1] == 'p') {
        opt = PRICE;
        for (int i = 7; i < command.size(); ++i)
            var += command[i];
        checkdouble(var, 13);
    } else throw Exception("error");
}

void checkLen(const string &str, const int &maxLength) {
    if (str.size() > maxLength)
        throw Exception("len > maxlen");
}

bool getCommand(std::string &line, std::vector<string> &command) {
    if (line.size() > 1024)
        throw Exception("command: too long");
    for (int i = 0, sz = line.size(); i < sz; ++i)
        if (!isascii(line[i]))
            throw Exception("command: not ASCII");
    string tmp;
    tmp.clear();
    command.clear();
    bool isquote = 0;
    for (auto ch : line) {
        if ((isquote == 0 && ch == ' ') || ch == '\n') {
            if (!tmp.empty())
                command.push_back(tmp);
            tmp.clear();
        } else tmp += ch;
    }

    if (!tmp.empty())
        command.push_back(tmp);
    return 1;
}

void multiVarCheck(const std::vector<string>& var, Ull &book, const string &nowisbn) {
    bool visisbn(0), visname(0), visauthor(0), viskeyword(0), visprice(0);
    for (auto command : var) {
        if (command[1] == 'I') {
            if (visisbn)
                throw Exception("modify: multi-ISBN");
            visisbn = 1;
            if (command.size() <= 6)
                throw Exception("modify: -ISBN = NULL");
            if (command.substr(0, 6) != "-ISBN=") 
                throw Exception("error");
            string var;
            for (int i = 6, sz = command.size(); i < sz; ++i)
                var += command[i]; 
            checkstring2(var, 20);
            if (var == nowisbn)
                continue;
            std::vector<int> res;
            Varchar<20> isbn(var);
            book.query(isbn, res);
            if (res.size())
                throw Exception("modify: ISBN exists");
        } else if (command[1] == 'n') {
            if (visname)
                throw Exception("modify: multi-bookname");
            visname = 1;
            if (command.size() <= 8)
                throw Exception("modify: -name = NULL");
            if (command.substr(0, 6) != "-name=") 
                throw Exception("error");
            if (command[6] != '"'|| command[command.size() - 1] != '"')
                throw Exception("error");
            int cnt = 0;
            for (int i = 7, sz = command.size(); i < sz - 1; ++i, ++cnt)
                if (command[i] == '\"' || iscntrl(command[i]))
                    throw Exception("modify: -name \" is invalid"); 
            if (cnt > 60)
                throw Exception("modify: -name: string is too long");
        } else if (command[1] == 'a') {
            if (visauthor)
                throw Exception("modify: multi-author");
            visauthor = 1;
            if (command.size() <= 10)
                throw Exception("-author = NULL");
            if (command.substr(0, 8) != "-author=") 
                throw Exception("error");
            if (command[8] != '"'|| command[command.size() - 1] != '"')
                throw Exception("error");
            int cnt = 0;
            for (int i = 9, sz = command.size(); i < sz - 1; ++i, ++cnt)
                if (command[i] == '\"' || iscntrl(command[i]))
                    throw Exception("modify: -author \" is invalid"); 
            if (cnt > 60)
                throw Exception("modify: -author: string is too long");
        } else if (command[1] == 'k') {
            if (viskeyword)
                throw Exception("modify: multi-keyword");
            viskeyword = 1;
            if (command.size() <= 11)
                throw Exception("-keyword = NULL");
            if (command.substr(0, 9) != "-keyword=") 
                throw Exception("error");
            if (command[9] != '"'|| command[command.size() - 1] != '"')
                throw Exception("error");
            multiKeywordCheck(command);
            int cnt = 0;
            for (int i = 10, sz = command.size(); i < sz - 1; ++i, ++cnt)
                if (command[i] == '\"' || iscntrl(command[i]))
                    throw Exception("modify: -keyword \" is invalid"); 
            if (cnt > 60)
                throw Exception("modify: -keyword: string is too long");
        } else if (command[1] == 'p') {
            if (visprice)
                throw Exception("modify: multi-price");
            visprice=1;
            if (command.size() <= 7) 
                throw Exception("-price = NULL");
            if (command.substr(0, 7) != "-price=") 
                throw Exception("error");
            int cnt = 0;
            for (int i = 7; i < command.size(); ++i, ++cnt)
                if (command[i] != '.' && !isdigit(command[i]))
                    throw Exception("modify: -price: something strange appeared");
            if (cnt > 13)
                throw Exception("price string is too long");
        } else throw Exception("modify: - you mean what?");
    }
}

void multiKeywordCheck(const string &kw) {
    std::unordered_map<string, bool> vis;
    string tmp; tmp.clear();
    for (int i = 10, sz = kw.size(); i < sz - 1; ++i) {
        if (kw[i] == '|') {
            if (vis[tmp])
                throw Exception("modify -keyword: Keyword Reappeared");
            vis[tmp] = 1;
            tmp.clear();
        } else tmp += kw[i];
    }
    if (vis[tmp])
        throw Exception("modify -keyword: keyword Reappeared");
}

void checkint(const std::string &var, int len) {
    if (var.size() > len)
        throw Exception("int too long");
    for (const auto &it : var)
        if (!isdigit(it))
            throw Exception("invalid char in int");
}

void checkdouble(const std::string & var, int len) {
    if (var.size() > len)
        throw Exception("double too long");
    double visdot = 0;
    for (const auto &it : var)
        if (!isdigit(it))
            if (it == '.') {
                if (visdot)
                    throw Exception("2 more . in double");
                visdot = 1;
            } else throw Exception("invalid char in double");
}

void checkstring1(const std::string &var, int len) {
    if (var.size() > len)
        throw Exception("string1 too long");
    for (const auto &it : var)
        if (!isdigit(it) && !isalpha(it) && it != '_')
            throw Exception("invalid char in string1");
}

void checkstring2(const std::string &var, int len, char ch) {
    if (var.size() > len)
        throw Exception("string2 too long");
    for (const auto &it : var)
        if (iscntrl(it) || it == ch)
            throw Exception("invalid char in string2");
}

std::string inttostring(int cnt) {
    std::string t;
    t.resize(2);
    t[0] = cnt / 10 + '0';
    t[1] = cnt % 10 + '0';
    return t;
}

long long toint(const string &s, int MAXRANGE) {
    long long tmp = std::stoll(s);
    if (tmp > MAXRANGE)
        throw Exception("int exceed");
    return tmp;
}