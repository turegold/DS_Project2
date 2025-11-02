#pragma once
#ifndef _BpTree_H_
#define _BpTree_H_

#include "BpTreeDataNode.h"
#include "BpTreeIndexNode.h"
#include "EmployeeData.h"
#include "SelectionTree.h"

class BpTree
{
private:
	BpTreeNode *root;
	int order; // m children

public:
	BpTree(ofstream *fout, int order = 3)
	{
		this->root = nullptr;
		this->order = order;
	}

	~BpTree()
	{
		root = nullptr;
	}

	// Insert a new employee into the B+ Tree
	bool Insert(EmployeeData *newData);
	// Check if the data node exceeds its maximum capacity
	bool excessDataNode(BpTreeNode *pDataNode);
	// Check if the index node exceeds its maximum capacity
	bool excessIndexNode(BpTreeNode *pIndexNode);
	// Split a full data node into two nodes
	void splitDataNode(BpTreeNode *pDataNode);
	// Split a full index node into two nodes
	void splitIndexNode(BpTreeNode *pIndexNode);
	// Return pointer to the root node
	BpTreeNode *getRoot() { return root; }
	// Find and return the leaf node containing the given key
	BpTreeNode *searchDataNode(string name);
	// Search and return all employees whose names are with in [start, end]
	vector<EmployeeData *> searchRange(string start, string end);
};

#endif
