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

    EmployeeData *best = nullptr;
    SelectionTreeNode *bestNode = nullptr;

    // Find the employee with the highest income among all runs
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap() && !run[i]->getHeap()->IsEmpty())
        {
            EmployeeData *top = run[i]->getHeap()->Top();

            if (!best || top->getIncome() > best->getIncome())
            {
                best = top;
                bestNode = run[i];
            }
        }
    }

    // Set the top employee as the root node
    if (best)
    {
        root->setEmployeeData(best);
        root->setHeap(bestNode->getHeap());
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
        return false;

    int dept = newData->getDeptNo();

    // If a run for the same department already exist, insert into its heap
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getEmployeeData()->getDeptNo() == dept)
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
        return false;
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
    EmployeeData *best = nullptr;
    SelectionTreeNode *bestNode = nullptr;
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap() && !run[i]->getHeap()->IsEmpty())
        {
            EmployeeData *top = run[i]->getHeap()->Top();
            if (!best || top->getIncome() > best->getIncome())
            {
                best = top;
                bestNode = run[i];
            }
        }
    }

    // Update root or delete it if all runs are empty
    if (bestNode)
    {
        root->setEmployeeData(best);
        root->setHeap(bestNode->getHeap());
    }
    else
    {
        delete root;
        root = nullptr;
        return true;
    }

    return true;
}

// Print all employees in a given department in ascending order by name
bool SelectionTree::printEmployeeData(int dept_no)
{
    if (!fout)
        return false;
    std::ostream &out = *fout;

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

            out << "========PRINT_ST " << dept_no << "========\n";

            // Extract all employees from the heap into a temporary buffer
            vector<EmployeeData *> buf;
            while (!heap->IsEmpty())
            {
                buf.push_back(heap->Top());
                heap->Delete();
            }

            // Sort employees by name
            sort(buf.begin(), buf.end(), [](EmployeeData *a, EmployeeData *b)
                 { return a->getName() < b->getName(); });

            // Print sorted results
            for (auto *e : buf)
            {
                out << e->getName() << "/"
                    << e->getDeptNo() << "/"
                    << e->getID() << "/"
                    << e->getIncome() << "\n";
            }

            // Reinsert employees back into the heap
            for (auto *e : buf)
                heap->Insert(e);

            out << "=========================\n\n";
            found = true;
            break;
        }
    }

    // If the department was not found, print an error
    if (!found)
    {
        out << "========ERROR========\n";
        out << "600\n";
        out << "=====================\n\n";
        return false;
    }
    return true;
}