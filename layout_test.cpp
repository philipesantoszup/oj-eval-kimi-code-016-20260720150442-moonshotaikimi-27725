#include <iostream>
#include <cstring>

struct Test {
    int a[5];
};

int main() {
    Test t;
    t.a[0] = 1;
    t.a[1] = 2;
    t.a[2] = 3;
    
    std::cout << "Before: " << t.a[0] << ", " << t.a[1] << ", " << t.a[2] << std::endl;
    
    for (int i = 2; i > 0; i--) {
        t.a[i] = t.a[i-1];
    }
    t.a[0] = 0;
    
    std::cout << "After: " << t.a[0] << ", " << t.a[1] << ", " << t.a[2] << std::endl;
    
    // Now test with actual shift insertion
    int values[5] = {1, 2, 3, 0, 0};
    int pos = 3;
    int value = 4;
    
    std::cout << "Before shift: " << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << std::endl;
    
    for (int i = 3; i > 3; i--) {
        values[i] = values[i-1];
    }
    values[3] = 4;
    
    std::cout << "After shift: " << values[0] << ", " << values[1] << ", " << values[2] << ", " << values[3] << std::endl;
    
    return 0;
}
