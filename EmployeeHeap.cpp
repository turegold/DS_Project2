#pragma once
#include "EmployeeHeap.h"

// Insert a new employee into the heap
void EmployeeHeap::Insert(EmployeeData *data)
{
    // Expand heap size if capacity is exceeded
    if (datanum + 1 >= maxCapacity)
    {
        ResizeArray();
    }
    // Add the new element at the end and restore heap order
    datanum++;
    heapArr[datanum] = data;
    UpHeap(datanum);
}

// Return the top employee
EmployeeData *EmployeeHeap::Top()
{
    if (IsEmpty())
    {
        return nullptr;
    }
    return heapArr[1];
}

// Remove the top employee from the heap
void EmployeeHeap::Delete()
{
    if (IsEmpty())
        return;

    // Only one element in the heap
    if (datanum == 1)
    {
        heapArr[1] = nullptr;
        datanum = 0;
        return;
    }

    // Move the last element to the root and heapify down
    heapArr[1] = heapArr[datanum];
    heapArr[datanum] = nullptr;
    datanum--;

    // Restore max-heap
    DownHeap(1);
}

// Check if the heap is empty
bool EmployeeHeap::IsEmpty()
{
    if (datanum == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Move an element upward to maintain max-heap
void EmployeeHeap::UpHeap(int index)
{
    // If node is root, stop
    if (index == 1)
    {
        return;
    }

    int parent = index / 2;
    // Swap if the current node has higher income than its parent
    if (heapArr[index]->getIncome() > heapArr[parent]->getIncome())
    {
        swap(heapArr[index], heapArr[parent]);
        UpHeap(parent);
    }
}

// Move an element downward to maintain max-heap
void EmployeeHeap::DownHeap(int index)
{
    int left = 2 * index;
    int right = 2 * index + 1;
    int largest = index;

    // Compare with left and right children to find the largest income
    if (left <= datanum && heapArr[left]->getIncome() > heapArr[largest]->getIncome())
        largest = left;
    if (right <= datanum && heapArr[right]->getIncome() > heapArr[largest]->getIncome())
        largest = right;

    // If a child is larger, swap and continue downheap
    if (largest != index)
    {
        swap(heapArr[index], heapArr[largest]);
        DownHeap(largest);
    }
}

// Double the array size when the heap capacity is exceeded
void EmployeeHeap::ResizeArray()
{
    int newCapacity = maxCapacity * 2;
    EmployeeData **newArr = new EmployeeData *[newCapacity + 1];

    for (int i = 1; i <= datanum; i++)
        newArr[i] = heapArr[i];

    // Replace old array with new one
    delete[] heapArr;
    heapArr = newArr;
    maxCapacity = newCapacity;
}