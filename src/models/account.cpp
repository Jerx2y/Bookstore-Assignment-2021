#include "account.h"

Account::Account(const string &_id, const string &_password,
                 const string &_name, Privilege _priority)
    : userId_(_id), password_(_password), name_(_name), privilege_(_priority) {}
