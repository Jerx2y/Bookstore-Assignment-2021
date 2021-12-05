#ifndef BOOKSTORE_MEMORYRIVER_HPP_
#define BOOKSTORE_MEMORYRIVER_HPP_

#include <fstream>
#include <iostream>
#include <queue>

using std::string;
using std::fstream;
using std::ifstream;
using std::ofstream;

template<class T>
class MemoryRiver {
private:
    fstream file;
    string file_name;
    int sizeofT = sizeof(T);
    std::queue<long> q;
public:
    MemoryRiver() = default;

    void initialise(string FN = "", int opt = 0) {
        file_name = FN;
        while (!q.empty()) q.pop();
        file.open(file_name);
        if (!file) {
            file.open(file_name, std::ios::out);
            T t;
            if (opt) file.write(reinterpret_cast<char*> (&t), sizeofT);
        }
        file.close();
    }

    //在文件合适位置写入类对象t，并返回写入的位置索引index
    //位置索引意味着当输入正确的位置索引index，在以下三个函数中都能顺利的找到目标对象进行操作
    //位置索引index可以取为对象写入的起始位置
    int write(T &t) {
        file.open(file_name);
        if (!q.empty()) {
            file.seekp(q.front(), std::ios::beg);
            q.pop();
        } else file.seekp(0, std::ios::end);
        int res = file.tellp();
        file.write(reinterpret_cast<char*> (&t), sizeofT);
        file.close();
        return res;
    }

    //用t的值更新位置索引index对应的对象，保证调用的index都是由write函数产生
    void update(T &t, const int index) {
        file.open(file_name);
        file.seekp(index);
        file.write(reinterpret_cast<char*> (&t), sizeofT);
        file.close();
    }

    //读出位置索引index对应的T对象的值并赋值给t，保证调用的index都是由write函数产生
    void read(T &t, const int index = 0) {
        file.open(file_name);
        file.seekg(index);
        file.read(reinterpret_cast<char*> (&t), sizeofT);
        file.close();
    }

    //删除位置索引index对应的对象(不涉及空间回收时，可忽略此函数)，保证调用的index都是由write函数产生
    void Delete(int index) {
        q.push(index);
    }
};


#endif //BPT_MEMORYRIVER_HPP
