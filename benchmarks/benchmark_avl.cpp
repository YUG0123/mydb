#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <string>
#include <cmath>
#include "../include/avl_tree.h"

using namespace std;
using namespace std::chrono;

int main()
{
    vector<int> sizes = {100, 1000, 10000, 100000};

    cout << "n\tinsert_time(us)\tsearch_time_avg(us)\theight\tlog2(n)\n";

    for (int n : sizes)
    {
        AVLTree tree;

        auto start = high_resolution_clock::now();

        for (int i = 0; i < n; i++)
        {
            Record record(
                "item" + to_string(i),
                20,
                60.0,
                9.0);

            tree.insert(record);
        }

        auto end = high_resolution_clock::now();

        auto insert_time =
            duration_cast<microseconds>(end - start).count();

        int searches = min(n, 1000);

        auto search_start = high_resolution_clock::now();

        for (int i = 0; i < searches; i++)
        {
            tree.search("item" + to_string(i));
        }

        auto search_end = high_resolution_clock::now();

        auto search_total =
            duration_cast<microseconds>(
                search_end - search_start)
                .count();

        double search_avg =
            (double)search_total / searches;

        double log_n = log2(n);

        cout << n << "\t"
             << insert_time << "\t\t"
             << search_avg << "\t\t"
             << tree.height() << "\t"
             << log_n << '\n';
    }

    return 0;
}