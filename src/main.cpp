#include <iostream>
#include <vector>
#include <sstream>

#include "commands.h"

using std::cin;
using std::cout;
using std::endl;

bool run(std::vector<string> command) {
    // TODO: handle Invalid commands (include assert)
    if (command[0] == "exit" || command[0] == "quit") {
        if (command.size() > 1)
            throw Exception("quit: invalid commands numbers");
        return false;
    } else if (command[0] == "su") {
        if (command.size() == 1 || command.size() > 3)
            throw Exception("su: invalid commands numbers");
        if (command.size() == 2)
            login(command[1]);
        else login(command[1], command[2]);
    } else if (command[0] == "logout") {
        if (command.size() > 1)
            throw Exception("logout: invalid commands numbers");
        logout();
    } else if (command[0] == "register") {
        if (command.size() != 4)
            throw Exception("register: invalid commands numbers");
        Register(command[1], command[2], command[3]);
    } else if (command[0] == "passwd") {
        if (command.size() != 4 && command.size() != 3)
            throw Exception("passwd: invalid commands numbers");
        if (command.size() == 3)
            changePassword(command[1], command[2]);
        else changePassword(command[1], command[2], command[3]);
    } else if (command[0] == "useradd") {
        if (command.size() != 5)
            throw Exception("useradd: invalid commands numbers");
        Privilege priority;
        if (command[3] == "1") priority = CUSTOMER;
        if (command[3] == "3") priority = WORKER;
        if (command[3] == "7") priority = ROOT;
        userAdd(command[1], command[2], priority, command[4]);
    } else if (command[0] == "delete") {
        if (command.size() != 2)
            throw Exception("delete: invalid commands numbers");
        deleteAccount(command[1]);
    } else if (command[0] == "show") { // show book or show finance
        if (command.size() > 3)
            throw Exception("show: invalid commands numbers");
        if (command.size() == 1) showBook();
        else if (command[1] != "finance") showBook(command[1]);
        else if (command.size() == 2) showFinance();
        else if (command.size() == 3) showFinance(std::stoi(command[2]));
    } else if (command[0] == "buy") {
        if (command.size() != 3)
            throw Exception("buy: invalid commands numbers");
        buyBook(command[1], std::stoi(command[2]));
    } else if (command[0] == "select") {
        if (command.size() != 2)
            throw Exception("select: invalid commands numbers");
        selectBook(command[1]);
    } else if (command[0] == "modify") {
        std::vector<string> modi;
        for (int i = 1; i < command.size(); ++i)
            modi.push_back(command[i]);
        if (modi.empty())
            throw Exception("modify: Modify Empty");
        modifyBook(modi);
    } else if (command[0] == "import") {
        if (command.size() != 3)
            throw Exception("import: invalid commands numbers");
        addBook(std::stoi(command[1]));
        takeFinance(-std::stod(command[2]));
    } else if (command[0] == "log") {
        if (command.size() != 1)
            throw Exception("log: invalid commands numbers");
        // TODO
    } else if (command[0] == "report") {
            if (command.size() != 2)
                throw Exception("report: invalid commands numbers");
            if (command[1] == "myself")
                ;// TODO
            if (command[1] == "finance")
                ;// TODO
            if (command[1] == "emplyee")
                ;// TODO
    } else throw Exception("Invalid command");
    return true;
}

int main() {

    ;

    init();
    std::vector<string> command;
    while (getCommand(command)) {
        if (command.empty()) continue;
        try {
            if (!run(command)) {
                break;
            }
        } catch (Exception ex) {
#ifdef LOCAL
            cout << "Invalid: ";
            cout << ex.what() << endl;
#else
            cout << "Invalid" << endl;
#endif
        }
    }

    return 0;
}