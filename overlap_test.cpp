#include <iostream>
#include <cstring>

struct Node {
    char keys[3][5];  // 3 keys of 5 chars each = 15 bytes
    int values[3];    // 3 ints = 12 bytes
};

int main() {
    std::cout << "sizeof(Node): " << sizeof(Node) << std::endl;
    std::cout << "Address of keys: " << (void*)(&((Node*)nullptr)->keys) << std::endl;
    std::cout << "Address of keys[2]: " << (void*)(&((Node*)nullptr)->keys[2]) << std::endl;
    std::cout << "Address of keys[3]: " << (void*)(&((Node*)nullptr)->keys[3]) << std::endl;
    std::cout << "Address of values[0]: " << (void*)(&((Node*)nullptr)->values[0]) << std::endl;
    std::cout << "Address of values[1]: " << (void*)(&((Node*)nullptr)->values[1]) << std::endl;
    
    Node node;
    memset(&node, 0, sizeof(node));
    
    strcpy(node.keys[0], "one");
    node.values[0] = 1;
    strcpy(node.keys[1], "two");
    node.values[1] = 2;
    strcpy(node.keys[2], "thr");
    node.values[2] = 3;
    
    std::cout << "Before: values[0]=" << node.values[0] << ", values[1]=" << node.values[1] << std::endl;
    
    // This writes to keys[3], which overlaps with values
    strcpy(node.keys[3], "fou");
    
    std::cout << "After writing to keys[3]: values[0]=" << node.values[0] << ", values[1]=" << node.values[1] << std::endl;
    
    return 0;
}
