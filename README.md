# 📚 书店管理系统 📚

> SJTU ACM 班 2021 级 程序设计课程大作业

> [原开发文档](https://hub.fastgit.org/Jerx2y/Bookstore-Assignment-2021/blob/master/Document.md)



Bookstore 2021 技术规格书 by Alan-Liang
=======================================

产品规格书见 <https://github.com/ACM-Programming-2021/Bookstore-2021/blob/main/README.md>。

[TOC]

## 代码规范

Google

## 存储结构

数据的实际内容存储在 `./rundata` 目录下对应的数据文件中，由 `Storage` 类管理。（鉴于 OJ 评测不支持将数据存在文件夹下，故在交到 OJ 的版本中直接将数据存在 `.` 目录中，这部分用宏定义来实现）

## 代码结构

### 工具库

#### 字符串存储 `varchar.{cpp,h}`

实现一个 `char [n]` 的包装。当输入的 `std::string`, `const char *` 或者 `Varchar<A>` 长度超过 `maxLength` 时抛出异常。

这个类存在的意义在于，如果用 `char []` 存数据，则无法方便地实现等号赋值和比较，需要特判；如果用 `std::string` 则无法方便地直接存储到文件中（因为 `std::string` 的实际内存在堆空间中），也需要特判。而使用 `Varchar<>` 存数据，则既可以 `user->name = "root";` 也可以直接 `write(static_cast<char *>(&data), sizeof(data));`。

```cpp
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
```

#### 数据存储 `storage.h`

实现一个类似于 Memory River 的存储结构，用于存储实际数据。

形式类似于 Memory River，文件开头有若干个 int 类型的数据，后接若干类的数据。

```cpp
template<class T>
class Storage {
private:
    fstream file_;
    string file_name_;
    int sizeofT_ = sizeof(T);
public:
    Storage() = default;
    bool initialise(string FN = "", int opt = 0);
    //读出第n个int的值赋给tmp，1_base
    void get_info(int &tmp, int n);
    //将tmp写入第n个int的位置，1_base
    void write_info(int tmp, int n);
    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t);
    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index);
    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index = 0);
};
```

#### 块状链表 `ull.{cpp,h}`

实现一个块状链表 `Ull`，支持以两个字符串分别为第一、第二关键字，查找其对应的某个整数数值:

```cpp
// 块状链表的底层节点
class Node {
public:
    int offset_; // 所存整数
    Varchar<60> first_, second_; // 两个关键字
    Node();
    template <int A, int B>
    Node(const Varchar<A>&, const Varchar<B>&, int);
    bool operator<(const Node &rhs) const;
    bool operator==(const Node &rhs) const;
};

// 块状链表的每一个块
class Block {
public:
    Node array_[kSize];
    int size_;
    Block();
    bool empty() const;
    Node maxvar();
    void merge(Block&);
    Block split();
    Block add(const Node&);
    bool dec(const Node&);
};

// 管理每个块的信息
class BlockIndex {
public:
    int size_;
    int offset_[kSize];
    Node maxvar_[kSize];
    BlockIndex();
    bool inrange(const int &);
    int &getoffset(const int &);
    void find(const Node &, int &);
    void extend(const Node &, const Node &, const int, const int &);
    void shrink(const Node &, const int &);
    template <int A>
    void query(const Varchar<A> &, int &, int &);
    void erase(const int &);
};

// 块状链表
class Ull {
private:
    Storage<BlockIndex> blockindex_;
    Storage<Block> block_;
public:
    void initialize(const string&);
    template <int A, int B>
    void insert(const Varchar<A> &, const Varchar<B> &, const int &);
    template <int A, int B>
    void erase(const Varchar<A> &, const Varchar<B> &, const int &);
    template <int A>
    void query(const Varchar<A>&, vector<int>&);
    void getall(vector<int> &);
};

```



### 业务逻辑相关

#### 对象模型 `models/`

##### 帐户 `account.{cpp,h}`

```cpp
enum Privilege {
  CUSTOMER = 1,
  WORKER = 3,
  ROOT = 7
};

class Account {
 public:
  Varchar<30> userId_;
  Varchar<30> password_;
  Varchar<30> name_;
  Privilege privilege_;
  Account() = default;
  Account(const string&, const string&, const string&, Privilege);
  void check();
};
```

##### 图书及关键词 `book.{cpp,h}`

```cpp
class Book {
 public:
  Varchar<20> isbn_;
  Varchar<60> name_;
  Varchar<60> author_;
  long long stock_;
  double price_;
  Book();
};
```

##### 交易 `transaction.{cpp,h}`

```cpp
struct Transaction {
    int opt_;
    double var_;
    Transaction();
    Transaction(double);
};
```

#### 日志的写入与读取 

对于每一条成功解析并执行的命令，需要调用这里的接口留下日志。日志以纯文本文件写入在 `rundata/log` 中。特别的，密码错误的登录请求和更改密码请求也需记录。

除 `modify` 外，日志格式为: `<userId>[priority] <time> <result> <command>`，其中 `<time>` 为 UNIX 时间戳，`<result>` 为 `success` 或 `failed`。匿名用户的 `<userId>` 记录为 `<anon>` (一个不合法的 user id)，`[priority]` 为用户的权限。例如:

```plain
<anon>[0] 1638601146 fail su root 123456
<anon>[0] 1638601146 success su root sjtu
root[7] 1638601146 success select 978-7-19260817
root[7] 1638601146 success modify -name="sjtu"
root[7] 1638601147 success logout
<anon>[0] 1638601147 success quit
```

记录日志过程实现在 `main` 函数中；显示日志过程实现在 `commands.cpp` 中。

#### 指令处理与执行 `commands.{cpp,h}`

```cpp
// 保存登录栈的元素
struct Atom {
    Account account;
    int book; // the offset of book
    Atom(const Account &, const int &);
};

// 登录栈
class AccountStack{
 private:
  std::stack<Atom> online;
  std::unordered_map<string, int> instack;
 public:
  AccountStack() = default;
  void push(const Account&);
  void pop();
  void check(int);
  bool empty();
  Privilege getPriority();
  bool loggedin(const string &);
  int selected();
  void select(const int &);
  void getTop(std::string &);
};

// 初始化
void init();

// 用户相关
void addAccount(const string&, const string&, Privilege, const string&);
void getAccount(const string&, Account &, int &);
void deleteAccount(const string&);
void login(const string &);
void login(const string &, const string &);
void logout();
void Register(const string&, const string&, const string&);
void changePassword(const string &, const string &);
void changePassword(const string &, const string &, const string&);
void userAdd(const string&, const string&, Privilege, const string&);

// 书本相关
void addBook(const long long &);
void modifyBook(const vector<string>&);
double buyBook(const string &, const long long &);
void printBook(Book &);
void showBook();
void showBook(const string &);
void selectBook(const string &);

// 显示财务
void showFinance(int t = -1);
void takeFinance(double);

// 显示日志
void reportMyself();
void reportFinance();
void reportEmployee();
void reportLog();

// 指令执行
bool run(std::vector<string>, std::string&, int &, double &);
```

#### 工具类和函数 `utils.{cpp,h}`

在这里定义其他代码中用到的工具类和函数。

```cpp
// modify 的类型
enum Option { ISBN, NAME, AUTHOR, KEYWORD, PRICE };
// 返回 modify 的类型以及值
void getCommand(const string &, Option &, string &);
// 检查字符串长度并抛出异常
void checkLen(const string &, const int &);
// 处理一行命令，将字符串分开并存在 vector 中
bool getCommand(string&, std::vector<string> &);
// 检查 modify 是否有重复参数，并抛出异常
void multiKeywordCheck(const string &);
// 检查 modify 后接参数是否合法，并抛出异常
void multiVarCheck(const vector<string> &, Ull&, const string&);
// 检查字符串是否为 int 类型，并抛出异常
void checkint(const string&, int);
// 检查字符串是否为 double 类型，并抛出异常
void checkdouble(const string&, int);
// 检查字符串是否只含字母，数字，下划线，并抛出异常
void checkstring1(const string&, int);
// 检查字符串是否只含可见字符，并抛出异常
void checkstring2(const string&, int, char = 0);
// int 转 string 类函数
string inttostring(int);
// string 转 int 类函数
long long toint(const string&, int);
```

#### 入口点 `main.cpp`

按行读入并解析、执行指令，并记录操作，当捕获异常时输出 `Invalid` 并解析下一条指令。

```cpp
int main ();
```

## Metadata

| Authors         | Version |
| :-------------- | :------ |
| Alan-Liang, WhY | 4.1     |

### 如何修改本文档

1. 请把代码规范调整为您（实现者）使用的代码规范。特别需要注意的地方：文件名（`.cpp` 还是 `.cc`？），命名格式（`PascalCase`, `camelCase` 还是 `snake_case`？），命名规范（用不用 Hungarian Naming Convention？）
2. 请删除您不打算实现的部分和相关说明
3. 更改本文档时请同步更新上面表格里的 Authors 和 Version
