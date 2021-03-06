// ConsoleApplicationRTree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "RTree.h"
#include "RTreeRenderer/RTreeWindow.h"

#include <iostream>
#include <libhelpers/Window/WindowContainer.h>

int main() {
    RTree tree(2, 2);

    WindowContainer<RTreeWindow> rtreeWindow(L"RTreeWindow");

    //Sleep(5000);

    for (int i = 0; i < 10; i++) {
        Rect r(i * 2, 0, (i + 1) * 2, 2);

        if (i == 4) {
            int stop = 23;
        }

        tree.Insert(r);

        {
            auto tmpRoot = tree.GetRoot();
            auto rootCopy = tmpRoot->Clone();

            auto sameContent = tmpRoot->CompareContent(rootCopy);

            auto renderer = rtreeWindow->GetRenderer();

            renderer->SetTree(tree);
        }

        Sleep(1000);
    }

    //tree.Insert(Rect(0, 0, 2, 10)); // vertical left rect
    //tree.Insert(Rect(2, 0, 4, 10)); // vertical right rect
    //tree.Insert(Rect(-4, 0, 0, 2)); // horizontal left rect
    //tree.Insert(Rect(4, 0, 8, 2)); // horizontal right rect

    rtreeWindow.WaitForClose();
}