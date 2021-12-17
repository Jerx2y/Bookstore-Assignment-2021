#ifndef BOOKSTORE_ASSIGNMENT_EXCEPTION_H_
#define BOOKSTORE_ASSIGNMENT_EXCEPTION_H_

#include <string>

class Exception {
 private:
  std::string message;

 public:
  Exception(const std::string& arg) { message = arg; }

  std::string what() { return message; }
};

#endif