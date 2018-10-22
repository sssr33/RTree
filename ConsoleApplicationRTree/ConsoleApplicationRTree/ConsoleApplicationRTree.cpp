// ConsoleApplicationRTree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "RTree.h"

#include <iostream>

int main() {
    RTree tree(2, 2);

    for (int i = 0; i < 10; i++) {
        Rect r(i * 2, 0, (i + 1) * 2, 2);

        if (i == 3) {
            int stop = 23;
        }

        tree.Insert(r);
    }

    //tree.Insert(Rect(0, 0, 2, 10)); // vertical left rect

    //tree.Insert(Rect(2, 0, 4, 10)); // vertical right rect

    //tree.Insert(Rect(-4, 0, 0, 2)); // horizontal left rect

    //tree.Insert(Rect(4, 0, 8, 2)); // horizontal right rect
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
