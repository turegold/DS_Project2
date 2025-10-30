#pragma once
#include "EmployeeHeap.h"

void EmployeeHeap::Insert(EmployeeData *data)
{
    // 용량 초과 시 배열 크기 확장
    if (datanum + 1 >= maxCapacity)
    {
        ResizeArray();
    }
    datanum++;
    heapArr[datanum] = data;
    UpHeap(datanum);
}

EmployeeData *EmployeeHeap::Top()
{
    if (IsEmpty())
    {
        return nullptr;
    }
    return heapArr[1];
}

void EmployeeHeap::Delete()
{
    if (IsEmpty())
    {
        return;
    }

    // 마지막 노드를 루트로 이동
    heapArr[1] = heapArr[datanum];
    datanum--;
    DownHeap(1);
}

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

void EmployeeHeap::UpHeap(int index)
{
    // 루트일 경우
    if (index == 1)
    {
        return;
    }

    int parent = index / 2;
    if (heapArr[index]->getIncome() > heapArr[parent]->getIncome())
    {
        swap(heapArr[index], heapArr[parent]);
        UpHeap(parent);
    }
}

void EmployeeHeap::DownHeap(int index)
{
    int left = 2 * index;
    int right = 2 * index + 1;
    int largest = index;

    if (left <= datanum && heapArr[left]->getIncome() > heapArr[largest]->getIncome())
        largest = left;
    if (right <= datanum && heapArr[right]->getIncome() > heapArr[largest]->getIncome())
        largest = right;

    if (largest != index)
    {
        swap(heapArr[index], heapArr[largest]);
        DownHeap(largest);
    }
}

void EmployeeHeap::ResizeArray()
{
    int newCapacity = maxCapacity * 2;
    EmployeeData **newArr = new EmployeeData *[newCapacity + 1]; // index 1부터 사용

    for (int i = 1; i <= datanum; i++)
        newArr[i] = heapArr[i];

    delete[] heapArr;
    heapArr = newArr;
    maxCapacity = newCapacity;
}