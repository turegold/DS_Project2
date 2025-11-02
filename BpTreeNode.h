#pragma once
#include "EmployeeData.h"

class BpTreeNode
{
private:
	BpTreeNode *pParent;
	BpTreeNode *pMostLeftChild;

public:
	BpTreeNode()
	{
		pParent = NULL;
		pMostLeftChild = NULL;
	}
	~BpTreeNode()
	{
	}

	// Set the most left child node
	void setMostLeftChild(BpTreeNode *pN) { pMostLeftChild = pN; }
	// Set parent node
	void setParent(BpTreeNode *pN) { pParent = pN; }

	// Get parent node
	BpTreeNode *getParent() { return pParent; }
	// Get the most left child node
	BpTreeNode *getMostLeftChild() { return pMostLeftChild; }

	// Set next node
	virtual void setNext(BpTreeNode *pN) {}
	// Set prev node
	virtual void setPrev(BpTreeNode *pN) {}
	// Get next node
	virtual BpTreeNode *getNext() { return NULL; }
	// Get prev node
	virtual BpTreeNode *getPrev() { return NULL; }

	// Insert data into data map
	virtual void insertDataMap(string n, EmployeeData *pN) {}
	// Insert index into index map
	virtual void insertIndexMap(string n, BpTreeNode *pN) {}
	// Delete key from map
	virtual void deleteMap(string n) {}

	// Get index map
	virtual map<string, BpTreeNode *> *getIndexMap() { return {}; }
	// Get data map
	virtual map<string, EmployeeData *> *getDataMap() { return {}; }
};
