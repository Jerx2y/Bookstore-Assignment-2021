#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

#include "commands.h"

using std::cin;
using std::cout;
using std::endl;

int main() {
  fstream logfile("./rundata/log", std::ios::app);
  if (!logfile.good()) {
    logfile.close();
    logfile.open("./rundata/log", std::ios::out);
    logfile.close();
    logfile.open("./rundata/log", std::ios::app);
  }

  init();
  std::string command;
  vector<std::string> commands;
  while (getline(cin, command)) {
    if (command.empty()) continue;
    std::string id;
    int pri;
    try {
      getCommand(command, commands);
      if (commands.empty()) continue;
      double buy = -1;
      bool tmp = run(commands, id, pri, buy);
      if (buy == -1)
        logfile << id << '[' << pri << ']' << ' ' << time(0) << " success "
                << command << endl;
      else
        logfile << id << '[' << pri << ']' << ' ' << time(0) << " success "
                << command << " " << std::setiosflags(std::ios::fixed)
                << std::setprecision(2) << buy << std::endl;
      if (!tmp) break;
    } catch (Exception ex) {
      if (ex.what() == "passwd: wrong password to change password" ||
          ex.what() == "su: Wrong password to login")
        logfile << id << '[' << pri << ']' << ' ' << time(0) << " failed "
                << command << endl;

#ifdef LOCAL
      cout << "Invalid: ";
      cout << ex.what() << endl;
#else
      cout << "Invalid" << endl;
#endif
    }
  }
  logfile.close();

  return 0;
}