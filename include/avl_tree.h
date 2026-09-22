#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

struct Record
{
    string name;
    int age;
    double weight;
    double cgpa;

    Record() : name(""), age(0), weight(0), cgpa(0) {}
    Record(string n, int a, double w, double c)
        : name(n), age(a), weight(w), cgpa(c) {}
};

struct AVLNode
{
    Record data;
    AVLNode *left;
    AVLNode *right;
    int height;

    AVLNode(Record d) : data(d), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree
{
private:
    AVLNode *root;

    int getHeight(AVLNode *node)
    {
        if (node == nullptr)
            return 0;
        return node->height;
    }

    int getBalanceFactor(AVLNode *node)
    {
        if (node == nullptr)
            return 0;
        return getHeight(node->left) - getHeight(node->right);
    }

    AVLNode *rotateRight(AVLNode *y)
    {
        if (y == NULL || y->left == NULL)
            return y;
        AVLNode *tmp = y->left;
        y->left = tmp->right;
        tmp->right = y;

        y->height = 1 + max(getHeight(y->left),
                            getHeight(y->right));

        tmp->height = 1 + max(getHeight(tmp->left),
                              getHeight(tmp->right));
        y = tmp;

        return y;
    }

    AVLNode *rotateLeft(AVLNode *x)
    {

        if (x == NULL || x->right == NULL)
            return x;
        AVLNode *tmp = x->right;
        x->right = tmp->left;
        tmp->left = x;

        x->height = 1 + max(getHeight(x->left),
                            getHeight(x->right));

        tmp->height = 1 + max(getHeight(tmp->left),
                              getHeight(tmp->right));

        x = tmp;
        return x;
    }

    AVLNode *insertHelper(AVLNode *node, Record data)
    {
        if (node == nullptr)
        {
            return new AVLNode(data);
        }
        if (data.name < node->data.name)
        {
            node->left = insertHelper(node->left, data);
        }
        else if (data.name > node->data.name)
        {
            node->right = insertHelper(node->right, data);
        }
        else
        {
            return node;
        }

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int b_factor = getBalanceFactor(node);
        if (b_factor > 1 && data.name < node->left->data.name)
        {
            node = rotateRight(node);
        }
        else if (b_factor > 1 && data.name > node->left->data.name)
        {
            node->left = rotateLeft(node->left);
            node = rotateRight(node);
        }
        else if (b_factor < -1 && data.name > node->right->data.name)
        {
            node = rotateLeft(node);
        }
        else if (b_factor < -1 && data.name < node->right->data.name)
        {
            node->right = rotateRight(node->right);
            node = rotateLeft(node);
        }

        return node;
    }

    AVLNode *minValueNode(AVLNode *node)
    {
        AVLNode *current = node;
        while (current && current->left != nullptr)
        {
            current = current->left;
        }
        return current;
    }

    AVLNode *deleteHelper(AVLNode *node, string name)
    {
        if (node == nullptr)
            return node;

        if (name < node->data.name)
        {
            node->left = deleteHelper(node->left, name);
        }
        else if (name > node->data.name)
        {
            node->right = deleteHelper(node->right, name);
        }
        else
        {
            if (node->left == nullptr || node->right == nullptr)
            {
                AVLNode *temp = node->left ? node->left : node->right;
                if (temp == nullptr)
                {
                    temp = node;
                    node = nullptr;
                }
                else
                {
                    *node = *temp;
                }
                delete temp;
            }
            else
            {
                AVLNode *temp = minValueNode(node->right);
                node->data = temp->data;
                node->right = deleteHelper(node->right, temp->data.name);
            }
        }

        if (node == nullptr)
            return node;

        node->height = 1 + max(getHeight(node->left), getHeight(node->right));

        int b_factor = getBalanceFactor(node);

        // LL case
        if (b_factor > 1 && getBalanceFactor(node->left) >= 0)
        {
            node = rotateRight(node);
        }

        // LR case
        else if (b_factor > 1 && getBalanceFactor(node->left) < 0)
        {
            node->left = rotateLeft(node->left);
            node = rotateRight(node);
        }

        // RR case
        else if (b_factor < -1 && getBalanceFactor(node->right) <= 0)
        {
            node = rotateLeft(node);
        }

        // RL case
        else if (b_factor < -1 && getBalanceFactor(node->right) > 0)
        {
            node->right = rotateRight(node->right);
            node = rotateLeft(node);
        }

        return node;
    }

    AVLNode *searchHelper(AVLNode *node, string name)
    {
        if (node == nullptr || node->data.name == name)
            return node;
        if (name < node->data.name)
            return searchHelper(node->left, name);
        return searchHelper(node->right, name);
    }

    void inOrderHelper(AVLNode *node)
    {
        if (node == nullptr)
            return;
        inOrderHelper(node->left);
        cout << node->data.name << " ";
        inOrderHelper(node->right);
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(Record data)
    {
        root = insertHelper(root, data);
    }

    void remove(string name)
    {
        root = deleteHelper(root, name);
    }

    Record *search(string name)
    {
        AVLNode *result = searchHelper(root, name);
        return result ? &(result->data) : nullptr;
    }

    void printInOrder()
    {
        inOrderHelper(root);
        cout << endl;
    }

    int height()
    {
        return getHeight(root);
    }
};

#endif
