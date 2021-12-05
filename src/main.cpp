#include <iostream>
#include <vector>
#include <sstream>

#include "blocklist.hpp"

using std::cin;
using std::cout;
using std::endl;

int main() {

    int m;
    cin >> m;

    BlockList index;

    index.initialize("var");

    while (m--) {
        string opt, idx;
        int val;
        cin >> opt >> idx;
        if (opt == "insert") {
            cin >> val;
            index.insert(idx, val, val);
        }
        if (opt == "delete") {
            cin >> val;
            index.erase(idx, val, val);
        }
        if (opt == "find") {
            std::vector<int> res;
            index.query(idx, res);
            if (res.empty())
                cout << "null";
            else for (auto x : res)
                cout << x << " ";
            cout << endl;
        }
    }

    return 0;
}
