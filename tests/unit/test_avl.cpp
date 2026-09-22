#include <iostream>
#include <cassert>
#include "../../include/avl_tree.h"
#include "../../include/hash_table.h"

using namespace std;

void testInsertAndSearch() {
    AVLTree tree;
    tree.insert(Record("mayhul", 19, 197, 9.7));
    tree.insert(Record("rohan", 20, 250, 9.5));
    tree.insert(Record("prerit", 17, 150, 9.4));

    Record* r = tree.search("mayhul");
    assert(r != nullptr);
    assert(r->age == 19);
    cout << "[PASS] testInsertAndSearch\n";
}

void testSearchMissing() {
    AVLTree tree;
    tree.insert(Record("mayhul", 19, 197, 9.7));
    Record* r = tree.search("doesnotexist");
    assert(r == nullptr);
    cout << "[PASS] testSearchMissing\n";
}

// THIS is the test that will fail until you implement rotations.
// Insert in increasing order — a plain BST would degrade into a straight
// line (height == n). A correctly balanced AVL tree should keep
// height ~ log2(n).
void testStaysBalanced() {
    AVLTree tree;
    string names[] = {"a", "b", "c", "d", "e", "f", "g"};
    for (string n : names) {
        tree.insert(Record(n, 0, 0, 0));
    }
    // For 7 nodes, a balanced tree has height 3. An unbalanced
    // (linear) insert-in-order BST would have height 7.
    cout << "Tree height after inserting 7 sorted names: " << tree.height() << endl;
    assert(tree.height() <= 4); // will FAIL until rotations are implemented
    cout << "[PASS] testStaysBalanced\n";
}

void testHashTableBasic() {
    HashTable table;
    assert(table.createKey("rachna") == true);
    assert(table.createKey("rachna") == false); // duplicate
    assert(table.exists("rachna") == true);
    assert(table.exists("archna") == false);

    AVLTree* tree = table.getTree("rachna");
    assert(tree != nullptr);
    tree->insert(Record("mayhul", 19, 197, 9.7));
    assert(tree->search("mayhul") != nullptr);

    cout << "[PASS] testHashTableBasic\n";
}

int main() {
    cout << "Running unit tests...\n\n";

    testInsertAndSearch();
    testSearchMissing();
    testHashTableBasic();

    cout << "\n--- Now testing AVL balancing (expected to fail until you implement rotations) ---\n";
    testStaysBalanced();

    cout << "\nAll tests passed!\n";
    return 0;
}
