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
        checkstring1(command[1], 30);
        if (command.size() == 2) login(command[1]);
        else {
            checkstring2(command[2], 30);
            login(command[1], command[2]);
        }
    } else if (command[0] == "logout") {
        if (command.size() > 1)
            throw Exception("logout: invalid commands numbers");
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
        if (command[3] == "1") priority = CUSTOMER;
        else if (command[3] == "3") priority = WORKER;
        else if (command[3] == "7") priority = ROOT;
        else throw Exception("useradd: user's priority invalid");
        checkstring1(command[1], 30);
        checkstring1(command[2], 30);
        checkstring2(command[4], 30);
        userAdd(command[1], command[2], priority, command[4]);
    } else if (command[0] == "delete") {
        if (command.size() != 2)
            throw Exception("delete: invalid commands numbers");
        checkstring1(command[1], 30);
        deleteAccount(command[1]);
    } else if (command[0] == "show") { // show book or show finance
        if (command.size() > 3)
            throw Exception("show: invalid commands numbers");
        if (command.size() == 1) showBook();
        else if (command[1] != "finance") {
            // TODO: maby need to check
            if (command.size() != 2)
                throw Exception("show -: invalid commands numbers");
            showBook(command[1]);
        } else if (command.size() == 2) showFinance();
        else if (command.size() == 3) {
            checkint(command[2], 9);
            showFinance(std::stoi(command[2]));
        }
    } else if (command[0] == "buy") {
        if (command.size() != 3)
            throw Exception("buy: invalid commands numbers");
        checkstring2(command[1], 20);
        checkint(command[2], 10);
        buyBook(command[1], std::stoi(command[2]));
    } else if (command[0] == "select") {
        if (command.size() != 2)
            throw Exception("select: invalid commands numbers");
        checkstring2(command[1], 20);
        selectBook(command[1]);
    } else if (command[0] == "modify") {
        // TODO: modify need to check
        std::vector<string> modi;
        for (int i = 1; i < command.size(); ++i)
            modi.push_back(command[i]);
        if (modi.empty())
            throw Exception("modify: Modify Empty");
        modifyBook(modi);
    } else if (command[0] == "import") {
        if (command.size() != 3)
            throw Exception("import: invalid commands numbers");
        checkint(command[1], 10);
        checkdouble(command[2], 13);
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
    std::string command;
    vector<std::string> commands;
    while (getline(cin, command)) {
        if (command.empty()) continue;
        try {
            getCommand(command, commands);
            if (commands.empty()) continue;
            if (!run(commands))
                break;
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