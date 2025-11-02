#pragma once
#ifndef _BpTreeDataNode_H_
#define _BpTreeDataNode_H_

#include "BpTreeNode.h"

class BpTreeDataNode : public BpTreeNode
{
private:
	map<string, EmployeeData *> mapData;
	BpTreeNode *pNext;
	BpTreeNode *pPrev;

public:
	BpTreeDataNode()
	{
		pNext = NULL;
		pPrev = NULL;
	}
	~BpTreeDataNode()
	{
	}

	// Set the next data node
	void setNext(BpTreeNode *pN) { pNext = pN; }
	// Set the prev data node
	void setPrev(BpTreeNode *pN) { pPrev = pN; }
	// Get the next data node
	BpTreeNode *getNext() { return pNext; }
	// Get the prev data node
	BpTreeNode *getPrev() { return pPrev; }

	// Insert a new data into the data map
	void insertDataMap(string name, EmployeeData *pN)
	{
		mapData.insert(map<string, EmployeeData *>::value_type(name, pN));
	}

	// Delete a record by name
	void deleteMap(string name)
	{
		mapData.erase(name);
	}
	// Return pointer to the data map
	map<string, EmployeeData *> *getDataMap() { return &mapData; }
};

#endif
