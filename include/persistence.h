#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "hash_table.h"

using namespace std;

const string LOG_FILE_PATH = "data/log.txt";

inline void appendLog(const string &line)
{
    ofstream file(LOG_FILE_PATH, ios::app);

    if (!file)
    {
        cerr << "Error: could not open log file for writing\n";
        return;
    }

    file << line << '\n';
}

inline void replayLog(HashTable &db)
{
    ifstream file(LOG_FILE_PATH);

    if (!file)
    {
        return;
    }

    string line;

    while (getline(file, line))
    {
        if (line.empty())
        {
            continue;
        }

        stringstream ss(line);

        string op;
        ss >> op;

        if (op == "CREATE")
        {
            string ownerKey;

            ss >> ownerKey;

            db.createKey(ownerKey);
        }

        else if (op == "INSERT")
        {
            string ownerKey;
            string name;
            int age;
            double weight;
            double cgpa;

            ss >> ownerKey >> name >> age >> weight >> cgpa;

            AVLTree *tree = db.getTree(ownerKey);

            if (tree == nullptr)
            {
                cerr << "Warning: INSERT for missing database: "
                     << ownerKey << '\n';

                continue;
            }

            Record record(name, age, weight, cgpa);

            tree->insert(record);
        }

        else if (op == "DELETE")
        {
            string ownerKey;
            string name;

            ss >> ownerKey >> name;

            AVLTree *tree = db.getTree(ownerKey);

            if (tree == nullptr)
            {
                cerr << "Warning: DELETE for missing database: "
                     << ownerKey << '\n';

                continue;
            }

            tree->remove(name);
        }

        else
        {
            cerr << "Warning: unknown log operation: "
                 << op << '\n';
        }
    }
}

#endif