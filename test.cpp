#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>

const char* NODE_FILE = "nodes.idx";
const int MAX_DEGREE = 128;
const int MAX_KEYS = MAX_DEGREE - 1;
const int MAX_CHILDREN = MAX_DEGREE;
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
    std::cout << "sizeof(bool): " << sizeof(bool) << std::endl;
    std::cout << "sizeof(int): " << sizeof(int) << std::endl;
    std::cout << "sizeof(BPTNode): " << sizeof(BPTNode) << std::endl;
    
    BPTNode node;
    node.init();
    strcpy(node.keys[0], "test");
    node.values[0] = 123;
    node.key_count = 1;
    
    std::cout << "keys[0]: " << node.keys[0] << std::endl;
    std::cout << "values[0]: " << node.values[0] << std::endl;
    
    return 0;
}
