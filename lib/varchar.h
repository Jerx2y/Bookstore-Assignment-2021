#ifndef BOOKSTORE_ASSIGNMENT_VARCHAR_H_
#define BOOKSTORE_ASSIGNMENT_VARCHAR_H_

#include <cstring>
#include <iostream>
#include <string>

#include "exception.h"

template <int maxLength>
class Varchar {
  template <int A>
  friend class Varchar;

 private:
  char content_[maxLength + 1];

 public:
  Varchar();
  Varchar(const std::string &s);
  Varchar(const char *cstr);
  operator std::string() const;
  std::string str() const;
  template <int A>
  Varchar &operator=(const Varchar<A> &that);
  Varchar &operator=(const std::string &str);
  template <int A>
  bool operator<(const Varchar<A> &that) const;
  template <int A>
  bool operator>(const Varchar<A> &that) const;
  template <int A>
  bool operator<=(const Varchar<A> &that) const;
  template <int A>
  bool operator>=(const Varchar<A> &that) const;
  template <int A>
  bool operator==(const Varchar<A> &that) const;
  template <int A>
  bool operator!=(const Varchar<A> &that) const;
  bool empty();
};

template <int maxLength>
Varchar<maxLength>::Varchar() {
  memset(content_, 0, sizeof content_);
}

template <int maxLength>
Varchar<maxLength>::Varchar(const std::string &str) {
  auto size = str.size();
  if (size > maxLength) throw(Exception("The String Is Too Long"));
  for (int i = 0; i < size; ++i) content_[i] = str[i];
  content_[size] = 0;
}

template <int maxLength>
Varchar<maxLength>::Varchar(const char *cstr) {
  int size = 0;
  while (cstr[size]) ++size;
  if (size > maxLength) throw(Exception("The char* Is Too Long"));
  for (int i = 0; i < size; ++i) content_[i] = cstr[i];
  content_[size] = 0;
}

template <int maxLength>
std::string Varchar<maxLength>::str() const {
  std::string res;
  const char *p = content_;
  while (*p) res += *(p++);
  return res;
}

template <int maxLength>
Varchar<maxLength>::operator std::string() const {
  return str();
}

template <int maxLength>
template <int A>
Varchar<maxLength> &Varchar<maxLength>::operator=(const Varchar<A> &that) {
  int sz = 0;
  while (that.content_[sz]) ++sz;
  if (sz > maxLength) throw(Exception("That Is Too Long"));
  for (int i = 0; i < sz; ++i) content_[i] = that.content_[i];
  content_[sz] = 0;
  return *this;
}

template <int maxLength>
Varchar<maxLength> &Varchar<maxLength>::operator=(const std::string &str) {
  auto size = str.size();
  if (size > maxLength) throw(Exception("The String Is Too Long"));
  for (int i = 0; i < size; ++i) content_[i] = str[i];
  content_[size] = 0;
  return *this;
}

template <int maxLength>
template <int A>
bool Varchar<maxLength>::operator<(const Varchar<A> &that) const {
  const char *pthis = content_, *pthat = that.content_;
  while (*pthis && *pthat && *pthis == *pthat) ++pthis, ++pthat;
  return *pthis < *pthat;
}

template <int maxLength>
template <int A>
bool Varchar<maxLength>::operator>(const Varchar<A> &that) const {
  return that < *this;
}

template <int maxLength>
template <int A>
bool Varchar<maxLength>::operator<=(const Varchar<A> &that) const {
  return !(that < *this);
}

template <int maxLength>
template <int A>
bool Varchar<maxLength>::operator>=(const Varchar<A> &that) const {
  return !(*this < that);
}

template <int maxLength>
template <int A>
bool Varchar<maxLength>::operator==(const Varchar<A> &that) const {
  return that <= *this && *this <= that;
}

template <int maxLength>
template <int A>
bool Varchar<maxLength>::operator!=(const Varchar<A> &that) const {
  return that < *this || *this < that;
}

template <int maxLength>
bool Varchar<maxLength>::empty() {
  return content_[0] == 0;
}

#endif