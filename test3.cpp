#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdio>

const char* NODE_FILE = "nodes.idx";
const int MAX_DEGREE = 4;
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
    
    void dump() {
        std::cout << "Node: is_leaf=" << is_leaf << ", key_count=" << key_count;
        std::cout << ", parent=" << parent_id << ", next=" << next_id << ", prev=" << prev_id << std::endl;
        for (int i = 0; i < key_count; i++) {
            std::cout << "  [" << i << "] key=" << keys[i] << ", value=" << values[i] << std::endl;
        }
    }
};

class BPTree {
private:
    std::fstream file;
    int root_id;
    int node_count;
    
    int node_offset(int id) {
        return HEADER_SIZE + id * sizeof(BPTNode);
    }
    
    void write_node(int id, const BPTNode& node) {
        std::cout << "write_node(" << id << "): ";
        for (int i = 0; i < node.key_count; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << node.keys[i] << "=" << node.values[i];
        }
        std::cout << std::endl;
        
        file.seekp(node_offset(id));
        file.write(reinterpret_cast<const char*>(&node), sizeof(BPTNode));
    }
    
    void read_node(int id, BPTNode& node) {
        file.seekg(node_offset(id));
        file.read(reinterpret_cast<char*>(&node), sizeof(BPTNode));
        
        std::cout << "read_node(" << id << "): ";
        for (int i = 0; i < node.key_count; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << node.keys[i] << "=" << node.values[i];
        }
        std::cout << std::endl;
    }
    
    int new_node() {
        std::cout << "new_node: id=" << node_count << std::endl;
        BPTNode node;
        node.init();
        int id = node_count++;
        write_node(id, node);
        return id;
    }
    
    void save_header() {
        file.seekp(0);
        file.write(reinterpret_cast<const char*>(&root_id), sizeof(int));
        file.write(reinterpret_cast<const char*>(&node_count), sizeof(int));
    }
    
    int find_leaf(const char* key) {
        int node_id = root_id;
        BPTNode node;
        
        while (true) {
            read_node(node_id, node);
            if (node.is_leaf) break;
            
            int i = 0;
            while (i < node.key_count && strcmp(key, node.keys[i]) >= 0) {
                i++;
            }
            node_id = node.children[i];
        }
        std::cout << "find_leaf: found leaf " << node_id << std::endl;
        return node_id;
    }
    
    void split_leaf(int leaf_id, BPTNode& leaf) {
        std::cout << "split_leaf: leaf_id=" << leaf_id << std::endl;
        leaf.dump();
        
        int new_leaf_id = new_node();
        BPTNode new_leaf;
        new_leaf.init();
        new_leaf.is_leaf = true;
        new_leaf.parent_id = leaf.parent_id;
        new_leaf.prev_id = leaf_id;
        new_leaf.next_id = leaf.next_id;
        
        if (leaf.next_id != -1) {
            BPTNode next_leaf;
            read_node(leaf.next_id, next_leaf);
            next_leaf.prev_id = new_leaf_id;
            write_node(leaf.next_id, next_leaf);
        }
        
        leaf.next_id = new_leaf_id;
        
        int split_pos = (leaf.key_count + 1) / 2;
        new_leaf.key_count = leaf.key_count - split_pos;
        
        std::cout << "  split_pos=" << split_pos << ", new_leaf.key_count=" << new_leaf.key_count << std::endl;
        
        for (int i = 0; i < new_leaf.key_count; i++) {
            strcpy(new_leaf.keys[i], leaf.keys[split_pos + i]);
            new_leaf.values[i] = leaf.values[split_pos + i];
            std::cout << "  new_leaf[" << i << "]: copied " << new_leaf.keys[i] << "=" << new_leaf.values[i] << std::endl;
        }
        
        leaf.key_count = split_pos;
        
        write_node(new_leaf_id, new_leaf);
        write_node(leaf_id, leaf);
        
        std::cout << "  leaf[" << leaf_id << "] now has " << leaf.key_count << " keys" << std::endl;
        std::cout << "  calling insert_into_parent with key=" << new_leaf.keys[0] << std::endl;
        
        insert_into_parent(leaf_id, new_leaf.keys[0], new_leaf_id, leaf.parent_id);
    }
    
    void split_internal(int node_id, BPTNode& node) {
        std::cout << "split_internal: node_id=" << node_id << std::endl;
        node.dump();
        
        int new_node_id = new_node();
        BPTNode new_node;
        new_node.init();
        new_node.is_leaf = false;
        new_node.parent_id = node.parent_id;
        
        int split_pos = node.key_count / 2;
        char promoted_key[65];
        strcpy(promoted_key, node.keys[split_pos]);
        
        new_node.key_count = node.key_count - split_pos - 1;
        for (int i = 0; i < new_node.key_count; i++) {
            strcpy(new_node.keys[i], node.keys[split_pos + 1 + i]);
        }
        for (int i = 0; i <= new_node.key_count; i++) {
            new_node.children[i] = node.children[split_pos + 1 + i];
        }
        
        for (int i = 0; i <= new_node.key_count; i++) {
            BPTNode child;
            read_node(new_node.children[i], child);
            child.parent_id = new_node_id;
            write_node(new_node.children[i], child);
        }
        
        node.key_count = split_pos;
        
        write_node(new_node_id, new_node);
        write_node(node_id, node);
        
        insert_into_parent(node_id, promoted_key, new_node_id, node.parent_id);
    }
    
    void insert_into_parent(int left_id, const char* key, int right_id, int parent_id) {
        std::cout << "insert_into_parent: left=" << left_id << ", right=" << right_id << ", parent=" << parent_id << ", key=" << key << std::endl;
        
        if (parent_id == -1) {
            std::cout << "  Creating new root" << std::endl;
            
            int new_root_id = new_node();
            BPTNode new_root;
            new_root.init();
            new_root.is_leaf = false;
            new_root.key_count = 1;
            strcpy(new_root.keys[0], key);
            new_root.children[0] = left_id;
            new_root.children[1] = right_id;
            
            BPTNode left, right;
            read_node(left_id, left);
            read_node(right_id, right);
            left.parent_id = new_root_id;
            right.parent_id = new_root_id;
            write_node(left_id, left);
            write_node(right_id, right);
            
            root_id = new_root_id;
            write_node(new_root_id, new_root);
            save_header();
            return;
        }
        
        BPTNode parent;
        read_node(parent_id, parent);
        
        int i = 0;
        while (i < parent.key_count && strcmp(key, parent.keys[i]) > 0) {
            i++;
        }
        
        for (int j = parent.key_count; j > i; j--) {
            strcpy(parent.keys[j], parent.keys[j-1]);
        }
        for (int j = parent.key_count + 1; j > i + 1; j--) {
            parent.children[j] = parent.children[j-1];
        }
        
        strcpy(parent.keys[i], key);
        parent.children[i+1] = right_id;
        parent.key_count++;
        
        write_node(parent_id, parent);
        
        if (parent.key_count > MAX_KEYS) {
            split_internal(parent_id, parent);
        }
    }

public:
    BPTree() {
        std::ifstream check(NODE_FILE, std::ios::binary);
        if (check.good()) {
            check.close();
            file.open(NODE_FILE, std::ios::in | std::ios::out | std::ios::binary);
            file.seekg(0);
            file.read(reinterpret_cast<char*>(&root_id), sizeof(int));
            file.read(reinterpret_cast<char*>(&node_count), sizeof(int));
        } else {
            file.open(NODE_FILE, std::ios::out | std::ios::binary);
            file.close();
            file.open(NODE_FILE, std::ios::in | std::ios::out | std::ios::binary);
            
            root_id = 0;
            node_count = 1;
            
            BPTNode root;
            root.init();
            write_node(0, root);
            save_header();
        }
    }
    
    ~BPTree() {
        save_header();
        file.close();
    }
    
    void insert(const char* key, int value) {
        std::cout << "=== INSERT: " << key << "=" << value << " ===" << std::endl;
        
        int leaf_id = find_leaf(key);
        BPTNode leaf;
        read_node(leaf_id, leaf);
        
        for (int i = 0; i < leaf.key_count; i++) {
            if (strcmp(leaf.keys[i], key) == 0 && leaf.values[i] == value) {
                std::cout << "Duplicate, skipping" << std::endl;
                return;
            }
        }
        
        int pos = 0;
        while (pos < leaf.key_count) {
            int cmp = strcmp(key, leaf.keys[pos]);
            if (cmp < 0) break;
            if (cmp == 0 && value < leaf.values[pos]) break;
            pos++;
        }
        
        for (int i = leaf.key_count; i > pos; i--) {
            strcpy(leaf.keys[i], leaf.keys[i-1]);
            leaf.values[i] = leaf.values[i-1];
        }
        strcpy(leaf.keys[pos], key);
        leaf.values[pos] = value;
        leaf.key_count++;
        
        write_node(leaf_id, leaf);
        
        if (leaf.key_count > MAX_KEYS) {
            split_leaf(leaf_id, leaf);
        }
    }
    
    std::vector<int> find(const char* key) {
        std::cout << "=== FIND: " << key << " ===" << std::endl;
        std::vector<int> result;
        
        int leaf_id = find_leaf(key);
        BPTNode leaf;
        read_node(leaf_id, leaf);
        
        for (int i = 0; i < leaf.key_count; i++) {
            if (strcmp(leaf.keys[i], key) == 0) {
                result.push_back(leaf.values[i]);
            }
        }
        
        std::sort(result.begin(), result.end());
        return result;
    }
};

int main() {
    {
        std::cout << "=== PHASE 1: Insert ===" << std::endl;
        BPTree tree;
        for (int i = 1; i <= 5; i++) {
            char key[64];
            sprintf(key, "key%02d", i);
            tree.insert(key, i);
        }
        std::cout << std::endl;
    }
    
    {
        std::cout << "=== PHASE 2: Find ===" << std::endl;
        BPTree tree;
        for (int i = 1; i <= 5; i++) {
            char key[64];
            sprintf(key, "key%02d", i);
            std::vector<int> result = tree.find(key);
            std::cout << "key" << i << "->";
            if (result.empty()) {
                std::cout << "NOT FOUND";
            } else {
                std::cout << result[0];
            }
            std::cout << std::endl;
        }
    }
    
    return 0;
}
