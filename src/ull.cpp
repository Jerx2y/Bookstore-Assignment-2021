#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <fstream>

#include "ull.h"
#include "storage.h"

using std::string;
using std::vector;

Node::Node() {
    offset_ = 0;
}

bool Node::operator<(const Node &rhs) const {
    return first < rhs.first || (first == rhs.first && second < rhs.second);
}

bool Node::operator==(const Node &rhs) const {
    return first == rhs.first && second == rhs.second;
}
Block::Block() { size = 0; }

bool Block::empty() const { return !size; }

Node Block::maxvar() { return size ? array_[size - 1] : Node(); }

void Block::merge(Block &obj) {
    Node res[kSize];
    int ipos = 0, jpos = 0, rpos = 0;
    while (ipos < size && jpos < obj.size)
        if (array_[ipos] < array_[jpos])
            res[rpos++] = array_[ipos++];
        else res[rpos++] = obj.array_[jpos++];
    while (ipos < size)
        res[rpos++] = array_[ipos++];
    while (jpos < obj.size)
        res[rpos++] = obj.array_[jpos++];
    for (int i = 0; i < rpos; ++i)
        array_[i] = res[i];
    size = rpos;
}

Block Block::split() {
    Block res;
    res.size = size / 2;
    for (int i = size - 1, j = res.size - 1; j >= 0; --i, --j)
        res.array_[j] = array_[i], array_[i] = Node();
    size -= res.size;
    return res;
}

Block Block::add(const Node &var) {
    int targetpos = -1;
    for (int i = 0; i < size; ++i) {
        if (array_[i] < var) continue;
        targetpos = i;
        break;
    }
    if (targetpos == -1)
        targetpos = size;
    size++;
    for (int i = size - 1; i > targetpos; --i)
        array_[i] = array_[i - 1];
    array_[targetpos] = var;
    if (size > kLimit) return split();
    else return Block();
}

bool Block::dec(const Node &var) {
    int ipos = -1;
    for (int i = 0; i < size; ++i)
        if (array_[i] == var) {
            ipos = i;
            break;
        }
    if (ipos == -1) return false;
    size--;
    for (int i = ipos; i < size; ++i)
        array_[i] = array_[i + 1];
    array_[size] = Node();
    return true;
}

BlockIndex::BlockIndex() { size = 0; }

bool BlockIndex::inrange(const int &pos) {
    return 0 <= pos && pos < size;
}

int &BlockIndex::getoffset(const int &pos) {
    return offset[pos];
}

void BlockIndex::find(const Node &var, int &ipos) {
    for (int i = 0; i < size; ++i) {
        if (maxvar[i] < var) continue;
        ipos = i;
        return ;
    }
    if (size) ipos = size - 1, maxvar[ipos] = var;
    else size = 1, maxvar[0] = var;
}

void BlockIndex::extend(const Node &cvar, const Node &var, const int _offset, const int &ipos) {
    size++;
    for (int i = size - 1; i > ipos; --i) {
        maxvar[i] = maxvar[i - 1];
        offset[i] = offset[i - 1];
    }
    maxvar[ipos] = var;
    offset[ipos] = _offset;
    maxvar[ipos - 1] = cvar;
}

void BlockIndex::shrink(const Node &cvar, const int &ipos) {
    for (int i = ipos; i + 1 < size; ++i) {
        maxvar[i] = maxvar[i + 1];
        offset[i] = offset[i + 1];
    }
    --size;
    maxvar[size] = Node();
    offset[size] = 0;
    maxvar[ipos - 1] = cvar;
}

void BlockIndex::erase(const int &pos) {
    for (int i = pos; i + 1 < size; ++i) {
        maxvar[i] = maxvar[i + 1];
        offset[i] = offset[i + 1];
    }
    --size;
    maxvar[size] = Node();
    offset[size] = 0;
}

void Ull::initialize(const string& filename) {
    blockindex_.initialise(filename + ".idx.bin");
    block_.initialise(filename + ".bin");
}

void Ull::getall(vector<int> &res) {
    res.clear();
    BlockIndex index;
    blockindex_.read(index);
    for (int i = 0; i < index.size; ++i) {
        Block curblock;
        block_.read(curblock, index.getoffset(i));
        for (int j = 0; j < curblock.size; ++j)
            res.push_back(curblock.array_[j].offset_);
    }
}