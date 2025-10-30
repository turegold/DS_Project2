#pragma once
#include <fstream>
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

    void setRoot(SelectionTreeNode *pN) { this->root = pN; }
    SelectionTreeNode *getRoot() { return root; }

    void setTree();

    bool Insert(EmployeeData *newData);
    bool Delete();
    bool printEmployeeData(int dept_no);
    SelectionTreeNode *getRun(int index) { return run[index]; }
};