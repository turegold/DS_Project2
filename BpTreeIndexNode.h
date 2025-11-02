#pragma once
#ifndef _BPTREEINDEXNODE_H_
#define _BPTREEINDEXNODE_H_

#include "BpTreeNode.h"
#include <map>

class BpTreeIndexNode : public BpTreeNode
{
private:
	map<string, BpTreeNode *> mapIndex;

public:
	BpTreeIndexNode() {}
	~BpTreeIndexNode()
	{
	}

	// Insert a new pair into the index map
	void insertIndexMap(string name, BpTreeNode *pN)
	{
		mapIndex.insert(map<string, BpTreeNode *>::value_type(name, pN));
	}

	// Delete a key from the index map
	void deleteMap(string name)
	{
		mapIndex.erase(name);
	}

	// Return pointer to the index map
	map<string, BpTreeNode *> *getIndexMap() { return &mapIndex; }
};

#endif