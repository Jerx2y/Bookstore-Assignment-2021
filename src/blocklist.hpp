#ifndef BOOKSTORE_BLOCKLIST_HPP_
#define BOOKSOTRE_BLOCKLIST_HPP_

#include <string>
#include <vector>
#include <cstring>
#include "memoryriver.hpp"

using std::string;
using std::vector;
const int kSize = 520;
const int kLimit = 500;
const int ksLimit = 400;

class Node {
public:
    int offset_, second;
    char first[70];
    Node();
    Node(string, int, int);
    bool operator<(const Node &rhs) const;
    bool operator==(const Node &rhs) const;
};

class Block {
public:
    Node array_[kSize];
    int size;
    Block();
    bool empty() const;
    Node maxvar();
    void merge(Block&);
    Block split();
    Block add(const Node&);
    bool dec(const Node&);
};

class BlockIndex {
public:
    int size;
    int offset[kSize];
    Node maxvar[kSize];
    BlockIndex();
    bool inrange(const int &);
    int &getoffset(const int &);
    void find(const Node &, int &);
    void extend(const Node &, const Node &, const int, const int &);
    void shrink(const Node &, const int &);
    void query(const string &, int &, int &);
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