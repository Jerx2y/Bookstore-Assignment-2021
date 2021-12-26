#ifndef BOOKSTORE_ASSIGNMENT_STORAGE_H_
#define BOOKSTORE_ASSIGNMENT_STORAGE_H_

#include <fstream>
#include <iostream>

using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

template <class T>
class Storage {
 private:
  fstream file;
  string file_name;
  int sizeofT = sizeof(T);

 public:
  Storage() = default;

  bool initialise(string FN = "", int opt = 0) {
    file_name = FN;
    file.open(file_name);
    if (!file) {
      file.open(file_name, std::ios::out);
      for (int i = 1, x = 0; i <= opt; ++i)
        file.write(reinterpret_cast<char *>(&x), sizeof(int));
      file.close();
      return 0;
    }
    file.close();
    return 1;
  }

  //读出第n个int的值赋给tmp，1_base
  void get_info(int &tmp, int n) {
    file.open(file_name);
    file.seekg((n - 1) * sizeof(int));
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  //将tmp写入第n个int的位置，1_base
  void write_info(int tmp, int n) {
    file.open(file_name);
    file.seekp((n - 1) * sizeof(int));
    file.write(reinterpret_cast<char *>(&tmp), sizeof(int));
    file.close();
  }

  //在文件合适位置写入类对象t，并返回写入的位置索引index
  //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
  //位置索引index可以取为对象写入的起始位置
  int write(T &t) {
    file.open(file_name);
    file.seekp(0, std::ios::end);
    int res = file.tellp();
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
    return res;
  }

  //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
  void update(T &t, const int index) {
    file.open(file_name);
    file.seekp(index);
    file.write(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
  }

  //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
  void read(T &t, const int index = 0) {
    file.open(file_name);
    file.seekg(index);
    file.read(reinterpret_cast<char *>(&t), sizeofT);
    file.close();
  }

  void Delete(int index) { ; }
};

#endif
