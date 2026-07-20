#include <iostream>
#include <cstring>

struct BPTNode {
    int key_count;
    char keys[3][65];
    int values[3];
};

int main() {
    BPTNode leaf;
    leaf.key_count = 3;
    strcpy(leaf.keys[0], "key01");
    leaf.values[0] = 1;
    strcpy(leaf.keys[1], "key02");
    leaf.values[1] = 2;
    strcpy(leaf.keys[2], "key03");
    leaf.values[2] = 3;
    
    std::cout << "Before shift:" << std::endl;
    for (int i = 0; i < leaf.key_count; i++) {
        std::cout << "  [" << i << "] key=" << leaf.keys[i] << ", value=" << leaf.values[i] << std::endl;
    }
    
    const char* key = "key04";
    int value = 4;
    int pos = 3; // position to insert
    
    for (int i = leaf.key_count; i > pos; i--) {
        std::cout << "Shifting i=" << i << std::endl;
        strcpy(leaf.keys[i], leaf.keys[i-1]);
        leaf.values[i] = leaf.values[i-1];
    }
    strcpy(leaf.keys[pos], key);
    leaf.values[pos] = value;
    leaf.key_count++;
    
    std::cout << "After shift:" << std::endl;
    for (int i = 0; i < leaf.key_count; i++) {
        std::cout << "  [" << i << "] key=" << leaf.keys[i] << ", value=" << leaf.values[i] << std::endl;
    }
    
    return 0;
}
