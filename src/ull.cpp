#include "ull.h"

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "storage.h"


using std::string;
using std::vector;

Node::Node() { offset_ = 0; }

bool Node::operator<(const Node &rhs) const {
  return first_ < rhs.first_ || (first_ == rhs.first_ && second_ < rhs.second_);
}

bool Node::operator==(const Node &rhs) const {
  return first_ == rhs.first_ && second_ == rhs.second_;
}
Block::Block() { size_ = 0; }

bool Block::empty() const { return !size_; }

Node Block::maxvar() { return size_ ? array_[size_ - 1] : Node(); }

void Block::merge(Block &obj) {
  Node res[kSize];
  int ipos = 0, jpos = 0, rpos = 0;
  while (ipos < size_ && jpos < obj.size_)
    if (array_[ipos] < array_[jpos])
      res[rpos++] = array_[ipos++];
    else
      res[rpos++] = obj.array_[jpos++];
  while (ipos < size_) res[rpos++] = array_[ipos++];
  while (jpos < obj.size_) res[rpos++] = obj.array_[jpos++];
  for (int i = 0; i < rpos; ++i) array_[i] = res[i];
  size_ = rpos;
}

Block Block::split() {
  Block res;
  res.size_ = size_ / 2;
  for (int i = size_ - 1, j = res.size_ - 1; j >= 0; --i, --j)
    res.array_[j] = array_[i], array_[i] = Node();
  size_ -= res.size_;
  return res;
}

Block Block::add(const Node &var) {
  int targetpos = -1;
  for (int i = 0; i < size_; ++i) {
    if (array_[i] < var) continue;
    targetpos = i;
    break;
  }
  if (targetpos == -1) targetpos = size_;
  size_++;
  for (int i = size_ - 1; i > targetpos; --i) array_[i] = array_[i - 1];
  array_[targetpos] = var;
  if (size_ > kLimit)
    return split();
  else
    return Block();
}

bool Block::dec(const Node &var) {
  int ipos = -1;
  for (int i = 0; i < size_; ++i)
    if (array_[i] == var) {
      ipos = i;
      break;
    }
  if (ipos == -1) return false;
  size_--;
  for (int i = ipos; i < size_; ++i) array_[i] = array_[i + 1];
  array_[size_] = Node();
  return true;
}

BlockIndex::BlockIndex() { size_ = 0; }

bool BlockIndex::inrange(const int &pos) { return 0 <= pos && pos < size_; }

int &BlockIndex::getoffset(const int &pos) { return offset_[pos]; }

void BlockIndex::find(const Node &var, int &ipos) {
  for (int i = 0; i < size_; ++i) {
    if (maxvar_[i] < var) continue;
    ipos = i;
    return;
  }
  if (size_)
    ipos = size_ - 1, maxvar_[ipos] = var;
  else
    size_ = 1, maxvar_[0] = var;
}

void BlockIndex::extend(const Node &cvar, const Node &var, const int _offset,
                        const int &ipos) {
  size_++;
  for (int i = size_ - 1; i > ipos; --i) {
    maxvar_[i] = maxvar_[i - 1];
    offset_[i] = offset_[i - 1];
  }
  maxvar_[ipos] = var;
  offset_[ipos] = _offset;
  maxvar_[ipos - 1] = cvar;
}

void BlockIndex::shrink(const Node &cvar, const int &ipos) {
  for (int i = ipos; i + 1 < size_; ++i) {
    maxvar_[i] = maxvar_[i + 1];
    offset_[i] = offset_[i + 1];
  }
  --size_;
  maxvar_[size_] = Node();
  offset_[size_] = 0;
  maxvar_[ipos - 1] = cvar;
}

void BlockIndex::erase(const int &pos) {
  for (int i = pos; i + 1 < size_; ++i) {
    maxvar_[i] = maxvar_[i + 1];
    offset_[i] = offset_[i + 1];
  }
  --size_;
  maxvar_[size_] = Node();
  offset_[size_] = 0;
}

void Ull::initialize(const string &filename) {
  blockindex_.initialise(filename + ".idx.bin");
  block_.initialise(filename + ".bin");
}

void Ull::getall(vector<int> &res) {
  res.clear();
  BlockIndex index;
  blockindex_.read(index);
  for (int i = 0; i < index.size_; ++i) {
    Block curblock;
    block_.read(curblock, index.getoffset(i));
    for (int j = 0; j < curblock.size_; ++j)
      res.push_back(curblock.array_[j].offset_);
  }
}