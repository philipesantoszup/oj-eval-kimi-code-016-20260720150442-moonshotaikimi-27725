#include <iostream>
#include <fstream>
#include <cstring>

const int MAX_KEYS = 127;
const int MAX_CHILDREN = 128;
const int HEADER_SIZE = 1024;

struct BPTNode {
    bool is_leaf;
    int key_count;
    int parent_id;
    int next_id;
    int prev_id;
    char keys[MAX_KEYS][65];
    int values[MAX_KEYS];
    int children[MAX_CHILDREN];
    
    void init() {
        is_leaf = true;
        key_count = 0;
        parent_id = -1;
        next_id = -1;
        prev_id = -1;
        memset(keys, 0, sizeof(keys));
        memset(values, 0, sizeof(values));
        memset(children, -1, sizeof(children));
    }
};

int main() {
    {
        std::fstream file("test.idx", std::ios::out | std::ios::binary);
        file.seekp(HEADER_SIZE);
        BPTNode node;
        node.init();
        node.key_count = 2;
        strcpy(node.keys[0], "aa");
        node.values[0] = 1;
        strcpy(node.keys[1], "bb");
        node.values[1] = 2;
        file.write(reinterpret_cast<const char*>(&node), sizeof(BPTNode));
        file.flush();
    }
    
    {
        std::fstream file("test.idx", std::ios::in | std::ios::out | std::ios::binary);
        BPTNode node;
        file.seekg(HEADER_SIZE);
        file.read(reinterpret_cast<char*>(&node), sizeof(BPTNode));
        std::cout << "is_leaf: " << node.is_leaf << std::endl;
        std::cout << "key_count: " << node.key_count << std::endl;
        std::cout << "keys[0]: " << node.keys[0] << ", values[0]: " << node.values[0] << std::endl;
        std::cout << "keys[1]: " << node.keys[1] << ", values[1]: " << node.values[1] << std::endl;
    }
    
    return 0;
}
