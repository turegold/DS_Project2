#include "SelectionTree.h"

// Build or rebuild the winner tree
void SelectionTree::setTree()
{
    // Initialize root if it doesn't exist
    if (!root)
    {
        root = new SelectionTreeNode();
        root->HeapInit();
    }

    EmployeeData *highest_salary = nullptr;
    SelectionTreeNode *highest_salary_Node = nullptr;

    // Find the employee with the highest income among all runs
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap() && !run[i]->getHeap()->IsEmpty())
        {
            EmployeeData *top = run[i]->getHeap()->Top();

            if (!highest_salary || top->getIncome() > highest_salary->getIncome())
            {
                highest_salary = top;
                highest_salary_Node = run[i];
            }
        }
    }

    // Set the top employee as the root node
    if (highest_salary)
    {
        root->setEmployeeData(highest_salary);
        root->setHeap(highest_salary_Node->getHeap());
    }
    // If all runs are empty, clear the root
    else
    {
        root->setEmployeeData(nullptr);
        root->setHeap(nullptr);
    }
}

// Insert a new employee into the selection tree
bool SelectionTree::Insert(EmployeeData *newData)
{
    if (!newData)
    {
        return false;
    }

    int deptNo = newData->getDeptNo();

    // Check duplication
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap())
        {
            EmployeeHeap *heap = run[i]->getHeap();
            for (int j = 1; j <= heap->getSize(); j++)
            {
                if (heap->getEmployee(j)->getID() == newData->getID())
                {
                    return false;
                }
            }
        }
    }

    // If a run for the same department already exist, insert into its heap
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getEmployeeData()->getDeptNo() == deptNo)
        {
            run[i]->getHeap()->Insert(newData);
            return true;
        }
    }

    // If there is an empty slot, create a new run
    for (int i = 0; i < 8; i++)
    {
        if (run[i] == nullptr)
        {
            SelectionTreeNode *newNode = new SelectionTreeNode();
            newNode->setEmployeeData(newData);
            newNode->HeapInit();
            newNode->getHeap()->Insert(newData);
            run[i] = newNode;
            return true;
        }
    }

    // All runs are full
    return false;
}

// Delete the top employee and reorganize the tree
bool SelectionTree::Delete()
{
    if (!root)
    {
        return false;
    }
    EmployeeHeap *heap = root->getHeap();
    if (!heap || heap->IsEmpty())
        return false;

    // Remove the top employee from the root heap
    heap->Delete();

    // If the heap becomes empty, remove the corresponding run
    if (heap->IsEmpty())
    {
        for (int i = 0; i < 8; i++)
        {
            if (run[i] && run[i]->getHeap() == heap)
            {
                delete run[i];
                run[i] = nullptr;
                break;
            }
        }
    }

    // Rebuild the root node
    setTree();

    // Find the new top employee across all remaining runs
    EmployeeData *highest_salary = nullptr;
    SelectionTreeNode *highest_salary_Node = nullptr;
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap() && !run[i]->getHeap()->IsEmpty())
        {
            EmployeeData *top = run[i]->getHeap()->Top();
            if (!highest_salary || top->getIncome() > highest_salary->getIncome())
            {
                highest_salary = top;
                highest_salary_Node = run[i];
            }
        }
    }

    // Update root or delete it if all runs are empty
    if (highest_salary_Node)
    {
        root->setEmployeeData(highest_salary);
        root->setHeap(highest_salary_Node->getHeap());
    }
    else
    {
        delete root;
        root = nullptr;
    }

    return true;
}

// Print all employees in a given department in descending order by income
bool SelectionTree::printEmployeeData(int dept_no)
{
    if (!fout)
        return false;

    bool found = false;

    // Search for the run that matches the given department number
    for (int i = 0; i < 8; i++)
    {
        if (!run[i] || !run[i]->getEmployeeData())
            continue;

        if (run[i]->getEmployeeData()->getDeptNo() == dept_no)
        {
            EmployeeHeap *heap = run[i]->getHeap();
            if (!heap || heap->IsEmpty())
                return false;

            (*fout) << "========PRINT_ST " << dept_no << "========\n";

            // Extract all employees from the heap into a temporary buffer
            vector<EmployeeData *> temp;
            while (!heap->IsEmpty())
            {
                temp.push_back(heap->Top());
                heap->Delete();
            }

            // Print sorted results
            for (auto *cur : temp)
            {
                (*fout) << cur->getName() << "/"
                        << cur->getDeptNo() << "/"
                        << cur->getID() << "/"
                        << cur->getIncome() << "\n";
            }

            // Reinsert employees back into the heap
            for (auto *cur : temp)
                heap->Insert(cur);

            (*fout) << "=========================\n\n";
            found = true;
            break;
        }
    }

    // If the department was not found, print an error
    if (!found)
    {
        (*fout) << "========ERROR========\n";
        (*fout) << "600\n";
        (*fout) << "=====================\n\n";
        return false;
    }

    return true;
}