#include "account.h"

Account::Account(const string &_id, const string &_password, const string &_name, Privilege _priority) : userId(_id), password(_password), name(_name), privilege(_priority) { }
