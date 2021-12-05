#include <string>
#include <vector>
#include <cstring>
#include <sstream>
#include <fstream>

#include "blocklist.hpp"
#include "memoryriver.hpp"

using std::string;
using std::vector;

Node::Node() {
    offset_ = 0;
    memset(first, 0, sizeof first);
}

Node::Node(string fir, int sec, int val) {
    memset(first, 0, sizeof first);
    for (int i = 0, sz = fir.size(); i < sz; ++i)
        first[i] = fir[i];
    second = sec, offset_ = val;
}

bool Node::operator<(const Node &rhs) const {
    return strcmp(first, rhs.first) < 0 || (!strcmp(first, rhs.first) && second < rhs.second);
    // TODO
}

bool Node::operator==(const Node &rhs) const {
    return !strcmp(first, rhs.first) && second == rhs.second;
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

void BlockIndex::query(const string &var, int &lpos, int &rpos) {
    lpos = 0, rpos = size - 1;
    for (int i = size; i; --i) {
        if (maxvar[i - 1].first < var) {
            lpos = i;
            break;
        }
    }
    for (int i = 0; i < size; ++i) {
        if (var < maxvar[i - 1].first) {
            rpos = i;
            break;
        }
    }
}

void BlockList::initialize(const string& filename) {
    blockindex_.initialise(filename + ".idx.bin");
    block_.initialise(filename + ".bin");
}

void BlockList::insert(const string &fir, const int &scd, const int &val) {
    Node var(fir, scd, val);
    BlockIndex index;
    blockindex_.read(index);
    int ipos = -1;
    index.find(var, ipos);
    Block curblock;
    if (ipos != -1) block_.read(curblock, index.getoffset(ipos));
    else ipos = 0, index.getoffset(ipos) = block_.write(curblock);
    Block extend = curblock.add(var);
    if (!extend.empty()) {
        int offset = block_.write(extend);
        index.extend(curblock.maxvar(), extend.maxvar(), offset, ipos + 1);
    }
    block_.update(curblock, index.getoffset(ipos));
    blockindex_.update(index, 0);
}

void BlockList::erase(const string &fir, const int &scd, const int &val) {
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
        if (curblock.size + nxtblock.size < ksLimit) {
            block_.Delete(index.getoffset(ipos + 1));
            curblock.merge(nxtblock);
            index.shrink(curblock.maxvar(), ipos + 1);
        }
    }
    blockindex_.update(index, 0);
    block_.update(curblock, index.getoffset(ipos));
}

void BlockList::query(const string &var, vector<int> &res) {
    res.clear();
    BlockIndex index;
    blockindex_.read(index);
    int lpos, rpos;
    index.query(var, lpos, rpos);
    for (int i = lpos; i <= rpos; ++i) {
        Block curblock;
        block_.read(curblock, index.getoffset(i));
        for (int j = 0; j < curblock.size; ++j) {
            if (var == curblock.array_[j].first)
                res.push_back(curblock.array_[j].offset_);
        }
    }
}