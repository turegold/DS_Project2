#pragma once
#include <fstream>
#include <algorithm>
#include <vector>
#include "SelectionTreeNode.h"

class SelectionTree
{
private:
    SelectionTreeNode *root;
    ofstream *fout;
    SelectionTreeNode *run[8];

public:
    SelectionTree(ofstream *fout)
    {
        this->fout = fout;
        this->root = nullptr;
        for (int i = 0; i < 8; i++)
        {
            this->run[i] = nullptr;
        }
    }

    ~SelectionTree()
    {
        for (int i = 0; i < 8; i++)
        {
            if (run[i])
            {
                delete run[i];
            }
        }
        root = nullptr;
    }

    // Set the root node
    void setRoot(SelectionTreeNode *pN) { this->root = pN; }
    // Get the root node
    SelectionTreeNode *getRoot() { return root; }

    // Build the selection tree
    void setTree();

    // Insert a new employee into the run
    bool Insert(EmployeeData *newData);
    // Delete the top employee and reorganize the tree
    bool Delete();
    // Print all employees in a given department in ascending order by name
    bool printEmployeeData(int dept_no);
    // Access a specific run by index
    SelectionTreeNode *getRun(int index) { return run[index]; }
};