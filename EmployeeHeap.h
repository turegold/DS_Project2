#pragma once

#include "EmployeeData.h"

class EmployeeHeap
{
private:
    int datanum;
    EmployeeData **heapArr;
    int maxCapacity = 10;

public:
    EmployeeHeap()
    {
        datanum = 0;
        heapArr = new EmployeeData *[maxCapacity + 1];
    }
    ~EmployeeHeap()
    {
        delete[] heapArr;
    }

    // Insert a new employee into the heap
    void Insert(EmployeeData *data);
    // Return the top employee
    EmployeeData *Top();
    // Remove the top employee from the heap
    void Delete();

    // Check if the heap is empty
    bool IsEmpty();

    // Move an element up to restore heap order
    void UpHeap(int index);
    // Move an element down to restore heap order
    void DownHeap(int index);
    // Resize the heap array when capacity is exceeded
    void ResizeArray();
};