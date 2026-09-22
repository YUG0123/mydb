#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <string>
#include <cstdint>
#include "avl_tree.h"

using namespace std;

const int TABLE_CAPACITY = 101; // prime, fixed size for v1 (Decision 9: no dynamic resize yet)

// FNV-1a hash — simple, well-known, better distribution than a naive char-sum.
inline uint32_t fnv1aHash(const string& key) {
    uint32_t hash = 2166136261u; // FNV offset basis
    for (unsigned char c : key) {
        hash ^= c;
        hash *= 16777619u; // FNV prime
    }
    return hash;
}

// One entry in a hash bucket's chain.
struct HashEntry {
    string key;       // e.g. "teacher_name" / owner namespace
    AVLTree* tree;     // this owner's records, stored in an AVL tree
    HashEntry* next;   // separate chaining for collisions

    HashEntry(string k) : key(k), tree(new AVLTree()), next(nullptr) {}
};

class HashTable {
private:
    HashEntry* buckets[TABLE_CAPACITY];

    int getIndex(const string& key) {
        return fnv1aHash(key) % TABLE_CAPACITY;
    }

public:
    HashTable() {
        for (int i = 0; i < TABLE_CAPACITY; i++) buckets[i] = nullptr;
    }

    // Creates a new namespace/owner entry if it doesn't already exist.
    // Returns false if it already exists (mirrors "create_database" semantics).
    bool createKey(const string& key) {
        int idx = getIndex(key);
        HashEntry* current = buckets[idx];
        while (current != nullptr) {
            if (current->key == key) return false; // already exists
            current = current->next;
        }
        HashEntry* newEntry = new HashEntry(key);
        newEntry->next = buckets[idx];
        buckets[idx] = newEntry;
        return true;
    }

    // Returns the AVL tree for this key, or nullptr if the key doesn't exist.
    AVLTree* getTree(const string& key) {
        int idx = getIndex(key);
        HashEntry* current = buckets[idx];
        while (current != nullptr) {
            if (current->key == key) return current->tree;
            current = current->next;
        }
        return nullptr;
    }

    bool exists(const string& key) {
        return getTree(key) != nullptr;
    }
};

#endif
