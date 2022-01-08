#ifndef BOOKSTORE_ASSIGNMENT_ULL_H_
#define BOOKSTORE_ASSIGNMENT_ULL_H_

#include <cstring>
#include <string>
#include <vector>

#include "../lib/varchar.h"
#include "storage.h"


using std::string;
using std::vector;

const int kSize = 4200;
const int kLimit = 4000;
const int ksLimit = 2000;

class Node {
 public:
  int offset_;
  Varchar<60> first_, second_;
  Node();
  template <int A, int B>
  Node(const Varchar<A> &, const Varchar<B> &, int);
  bool operator<(const Node &rhs) const;
  bool operator==(const Node &rhs) const;
};

class Block {
 public:
  Node array_[kSize];
  int size_;
  Block();
  bool empty() const;
  Node maxvar();
  void merge(Block &);
  Block split();
  Block add(const Node &);
  bool dec(const Node &);
};

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

class Ull {
 private:
  Storage<BlockIndex> blockindex_;
  Storage<Block> block_;

 public:
  void initialize(const string &);
  template <int A, int B>
  void insert(const Varchar<A> &, const Varchar<B> &, const int &);
  template <int A, int B>
  void erase(const Varchar<A> &, const Varchar<B> &, const int &);
  template <int A>
  void query(const Varchar<A> &, vector<int> &);
  void getall(vector<int> &);
};

template <int A, int B>
Node::Node(const Varchar<A> &fir, const Varchar<B> &sec, int val) {
  first_ = fir, second_ = sec, offset_ = val;
}

template <int A>
void BlockIndex::query(const Varchar<A> &var, int &lpos, int &rpos) {
  lpos = 0, rpos = size_ - 1;
  for (int i = size_; i; --i) {
    if (maxvar_[i - 1].first_ < var) {
      lpos = i;
      break;
    }
  }
  for (int i = 0; i < size_; ++i) {
    if (var < maxvar_[i - 1].first_) {
      rpos = i;
      break;
    }
  }
}

template <int A, int B>
void Ull::insert(const Varchar<A> &fir, const Varchar<B> &scd, const int &val) {
  Node var(fir, scd, val);
  BlockIndex index;
  blockindex_.read(index);
  int ipos = -1;
  index.find(var, ipos);
  Block curblock;
  if (ipos != -1)
    block_.read(curblock, index.getoffset(ipos));
  else
    ipos = 0, index.getoffset(ipos) = block_.write(curblock);
  Block extend = curblock.add(var);
  if (!extend.empty()) {
    int offset_ = block_.write(extend);
    index.extend(curblock.maxvar(), extend.maxvar(), offset_, ipos + 1);
  }
  block_.update(curblock, index.getoffset(ipos));
  blockindex_.update(index, 0);
}

template <int A, int B>
void Ull::erase(const Varchar<A> &fir, const Varchar<B> &scd, const int &val) {
  Node var(fir, scd, val);
  BlockIndex index;
  blockindex_.read(index);
  int ipos;
  index.find(var, ipos);
  Block curblock;
  block_.read(curblock, index.getoffset(ipos));
  curblock.dec(var);
  if (index.inrange(ipos + 1)) {
    Block nxtblock;
    block_.read(nxtblock, index.getoffset(ipos + 1));
    if (curblock.size_ + nxtblock.size_ < ksLimit) {
      block_.Delete(index.getoffset(ipos + 1));
      curblock.merge(nxtblock);
      index.shrink(curblock.maxvar(), ipos + 1);
    }
  }
  if (curblock.empty()) index.erase(ipos);
  blockindex_.update(index, 0);
  block_.update(curblock, index.getoffset(ipos));
}

template <int A>
void Ull::query(const Varchar<A> &var, vector<int> &res) {
  res.clear();
  BlockIndex index;
  blockindex_.read(index);
  int lpos, rpos;
  index.query(var, lpos, rpos);
  for (int i = lpos; i <= rpos; ++i) {
    Block curblock;
    block_.read(curblock, index.getoffset(i));
    for (int j = 0; j < curblock.size_; ++j) {
      if (var == curblock.array_[j].first_)
        res.push_back(curblock.array_[j].offset_);
    }
  }
}

#endif