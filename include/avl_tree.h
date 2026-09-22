#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

// A single record stored in the tree.
// This is your FIXED SCHEMA (Decision 4) — adjust fields to whatever
// your project actually needs, but keep it fixed (not dynamic key-value).
struct Record {
    string name;
    int age;
    double weight;
    double cgpa;

    Record() : name(""), age(0), weight(0), cgpa(0) {}
    Record(string n, int a, double w, double c)
        : name(n), age(a), weight(w), cgpa(c) {}
};

// A single node in the AVL tree.
struct AVLNode {
    Record data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Record d) : data(d), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    AVLNode* root;

    int getHeight(AVLNode* node) {
        if (node == nullptr) return 0;
        return node->height;
    }

    int getBalanceFactor(AVLNode* node) {
        if (node == nullptr) return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    // ---- TODO: implement this ----
    // Standard LL-case rotation. Reference: visualgo.net/en/bst (AVL mode).
    // Trace it by hand on paper BEFORE writing code.
    AVLNode* rotateRight(AVLNode* y) {
        cout << "[TODO] rotateRight not implemented yet\n";
        return y; // placeholder — replace with real rotation
    }

    // ---- TODO: implement this ----
    // Standard RR-case rotation.
    AVLNode* rotateLeft(AVLNode* x) {
        cout << "[TODO] rotateLeft not implemented yet\n";
        return x; // placeholder — replace with real rotation
    }

    // ---- TODO: implement rebalancing (rotations) here ----
    // 1. Normal BST insert (compare by data.name, alphabetically) -- already done below
    // 2. Update node->height = 1 + max(getHeight(left), getHeight(right)) -- already done below
    // 3. int balance = getBalanceFactor(node)
    // 4. Four cases:
    //    - balance > 1  and name < node->left->data.name   -> return rotateRight(node)          [LL]
    //    - balance < -1 and name > node->right->data.name  -> return rotateLeft(node)           [RR]
    //    - balance > 1  and name > node->left->data.name   -> node->left = rotateLeft(node->left); return rotateRight(node)  [LR]
    //    - balance < -1 and name < node->right->data.name  -> node->right = rotateRight(node->right); return rotateLeft(node) [RL]
    // 5. Otherwise return node unchanged
    AVLNode* insertHelper(AVLNode* node, Record data) {
        if (node == nullptr) {
            return new AVLNode(data);
        }
        if (data.name < node->data.name) {
            node->left = insertHelper(node->left, data);
        } else if (data.name > node->data.name) {
            node->right = insertHelper(node->right, data);
        } else {
            return node; // duplicate name — ignore or handle as update, your call
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        // TODO: replace the line below with real balance-factor check + 4 rotation cases
        // (see comment block above this function)
        return node;
    }

    AVLNode* minValueNode(AVLNode* node) {
        AVLNode* current = node;
        while (current && current->left != nullptr) {
            current = current->left;
        }
        return current;
    }

    // ---- TODO: implement rebalancing here too (same 4 cases as insert) ----
    AVLNode* deleteHelper(AVLNode* node, string name) {
        if (node == nullptr) return node;

        if (name < node->data.name) {
            node->left = deleteHelper(node->left, name);
        } else if (name > node->data.name) {
            node->right = deleteHelper(node->right, name);
        } else {
            // Node to delete found
            if (node->left == nullptr || node->right == nullptr) {
                AVLNode* temp = node->left ? node->left : node->right;
                if (temp == nullptr) {
                    temp = node;
                    node = nullptr;
                } else {
                    *node = *temp;
                }
                delete temp;
            } else {
                AVLNode* temp = minValueNode(node->right);
                node->data = temp->data;
                node->right = deleteHelper(node->right, temp->data.name);
            }
        }

        if (node == nullptr) return node;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        // TODO: add balance factor check + 4 rotation cases here, same pattern as insertHelper.
        return node;
    }

    AVLNode* searchHelper(AVLNode* node, string name) {
        if (node == nullptr || node->data.name == name) return node;
        if (name < node->data.name) return searchHelper(node->left, name);
        return searchHelper(node->right, name);
    }

    void inOrderHelper(AVLNode* node) {
        if (node == nullptr) return;
        inOrderHelper(node->left);
        cout << node->data.name << " ";
        inOrderHelper(node->right);
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(Record data) {
        root = insertHelper(root, data);
    }

    void remove(string name) {
        root = deleteHelper(root, name);
    }

    Record* search(string name) {
        AVLNode* result = searchHelper(root, name);
        return result ? &(result->data) : nullptr;
    }

    void printInOrder() {
        inOrderHelper(root);
        cout << endl;
    }

    int height() {
        return getHeight(root);
    }
};

#endif
