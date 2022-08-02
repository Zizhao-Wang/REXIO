#include "../Header.h"

class HashTable:public Hash
{
    std::vector<std::list<std::string>> _vec;
public:

    HashTable() :_vec(26){}

    void add(std::string& val)
    {
        _vec[val[0] - 97].push_back(val);
    }

    bool check(std::string& val)
    {
        if (find(_vec[val[0] - 97].begin(), _vec[val[0] - 97].end(), val) != _vec[val[0] - 97].end())
            return true;
        else
            return false;
    }

    int getMaxCollisions() const
    {
        int tmp = _vec[0].size();

        for (int i = 1; i < _vec.size(); ++i)
        {
            if (tmp < _vec[i].size())
                tmp = _vec[i].size();
        }
        return tmp;
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

