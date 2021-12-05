#ifndef BOOKSTORE_BLOCKLIST_HPP_
#define BOOKSOTRE_BLOCKLIST_HPP_

#include <string>
#include <vector>
#include <cstring>
#include "memoryriver.hpp"

using std::string;
using std::vector;
const int kSize = 350;
const int kLimit = 333;
const int ksLimit = 222;

class Node {
public:
    int offset_, second;
    char first[70];
    Node() {
        offset_ = 0;
        memset(first, 0, sizeof first);
    }
    Node(string fir, int sec, int val) {
        memset(first, 0, sizeof first);
        for (int i = 0, sz = fir.size(); i < sz; ++i)
            first[i] = fir[i];
        second = sec, offset_ = val;
    }
    bool operator<(const Node &rhs) const;
    bool operator==(const Node &rhs) const;
};

class Block {
public:
    Node array_[kSize];
    int size;
    Block() { size = 0; }
    bool empty() const { return !size; }
    Node maxvar() { return size ? array_[size - 1] : Node(); }
    void merge(Block&);
    Block split();
    Block add(const Node&);
    bool dec(const Node&);
    void print() {
        std::cout << size << " " << std::endl;
        for (int i = 0; i < size; ++i)
            std::cout << array_[i].first << std::endl;
    }
};

class BlockIndex {
public:
    int size;
    int offset[kSize];
    Node maxvar[kSize];
    BlockIndex() { size = 0; }
    bool inrange(const int &pos) { return 0 <= pos && pos < size; };
    int &getoffset(const int &pos) { return offset[pos]; }
    void find(const Node &var, int &ipos) {
        for (int i = 0; i < size; ++i) {
            if (maxvar[i] < var) continue;
            ipos = i;
            return ;
        }
        if (size) ipos = size - 1, maxvar[ipos] = var;
        else size = 1, maxvar[0] = var;
    }
    void extend(const Node &cvar, const Node &var, const int _offset, const int &ipos) {
        size++;
        for (int i = size - 1; i > ipos; --i) {
            maxvar[i] = maxvar[i - 1];
            offset[i] = offset[i - 1];
        }
        maxvar[ipos] = var;
        offset[ipos] = _offset;
        maxvar[ipos - 1] = cvar;
    }
    void shrink(const Node &cvar, const int &ipos) {
        for (int i = ipos; i + 1 < size; ++i) {
            maxvar[i] = maxvar[i + 1];
            offset[i] = offset[i + 1];
        }
        --size;
        maxvar[size] = Node();
        offset[size] = 0;
        maxvar[ipos - 1] = cvar;
    }
    void query(const string &var, int &lpos, int &rpos) {
        lpos = 0, rpos = size - 1;
        // std::cout << var << " : " << std::endl;
        for (int i = size; i; --i) {
            string tmp = maxvar[i - 1].first;
            // std::cout << tmp << " " << var << std::endl;
            if (tmp < var) {
                lpos = i;
                break;
            }
        }
        for (int i = 0; i < size; ++i) {
            string tmp = maxvar[i - 1].first;
            if (var < tmp) {
                rpos = i;
                break;
            }
        }
    }
};

class BlockList {
private:
    MemoryRiver<BlockIndex> blockindex_;
    MemoryRiver<Block> block_;
public:
    void initialize(const string&);
    void insert(const string &, const int &, const int &);
    void erase(const string &, const int &, const int &);
    void query(const string&, vector<int>&);
};

#endif