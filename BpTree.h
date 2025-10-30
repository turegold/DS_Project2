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
		// 여기 구현
		this->root = nullptr;
		this->order = order;
	}

	~BpTree()
	{
		// 여기 구현
		root = nullptr;
	}

	/* essential */
	bool Insert(EmployeeData *newData);
	bool excessDataNode(BpTreeNode *pDataNode);
	bool excessIndexNode(BpTreeNode *pIndexNode);
	void splitDataNode(BpTreeNode *pDataNode);
	void splitIndexNode(BpTreeNode *pIndexNode);
	BpTreeNode *getRoot() { return root; }
	BpTreeNode *searchDataNode(string name);
	vector<EmployeeData *> searchRange(string start, string end);
};

#endif
