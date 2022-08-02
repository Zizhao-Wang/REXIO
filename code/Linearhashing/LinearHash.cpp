#include "../Header.h"

class HashTable:public Hash {
    int max_col;
    std::vector<std::string> _vec;
public:

    HashTable() : _vec(100), max_col(0) {}

    void add(std::string &val) {
        int tmp = 0;
        int res = val[0] - 97;
        if (_vec[res].empty()) {
            _vec[res] = val;
        } else {
            while (!_vec[res].empty()) {
                ++tmp;
                ++res;
            }
            _vec[res] = val;
        }
        if (tmp > max_col)
            max_col = tmp;
    }

    bool check(std::string &val) {
        int res = val[0] - 97;
        if (find(_vec.begin(), _vec.end(), val) != _vec.end())
            return true;
        else {
            while (!_vec[res].empty()) {
                ++res;
                if (find(_vec.begin(), _vec.end(), val) != _vec.end())
                    return true;
            }
            return false;
        }
    }

    int getMaxCollisions() const {
        return max_col;
    }
};

int main() {
    std::string dictionary[] = {
            "a", "or", "and", "asterisk", "zorb", "zorg", "ant", "cat", "rat", "rack"
    };

    Hash *hashTable = new HashTable();

    for (auto w : dictionary) {
        hashTable->add(w);
    }

    for (auto w : dictionary) {
        if (!hashTable->check(w)) {
            std::cout << "Exists check failed!" << std::endl;
        }
    }

    std::string badWords[] = {
            "aa", "oro", "b", "asteriska", "zorba", "zork", "racks"
    };

    for (auto w : badWords) {
        if (hashTable->check(w)) {
            std::cout << "NOT Exists check failed!" << std::endl;
        }
    }

    std::cout << "Maximal collisions count " << hashTable->getMaxCollisions() << std::endl;
}