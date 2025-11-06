#include "BpTree.h"
#include <cassert>

// Insert a new employee record into the B+ Tree
bool BpTree::Insert(EmployeeData *newData)
{
    if (!newData)
    {
        return false;
    }
    string key = newData->getName();

    // If tree is empty, create the first data node as root
    if (!root)
    {
        BpTreeDataNode *newLeaf = new BpTreeDataNode();
        newLeaf->insertDataMap(key, newData);
        root = newLeaf;
        return true;
    }

    // Find the leaf node where data should be insrted
    BpTreeNode *pLeaf = searchDataNode(key);
    if (!pLeaf)
    {
        return false;
    }

    // If a record with the same name already exists, update income
    auto cur_dataMap = pLeaf->getDataMap();
    auto cur_iter = cur_dataMap->find(key);
    if (cur_iter != cur_dataMap->end())
    {
        cur_iter->second->setIncome(newData->getIncome());
        return true;
    }

    // Insert new record into the leaf node
    pLeaf->insertDataMap(key, newData);

    // Check if the leaf node exceeds its capacity
    if (excessDataNode(pLeaf))
    {
        splitDataNode(pLeaf);
    }

    return true;
}

// Check if a data node has exceeded its maximum key count
bool BpTree::excessDataNode(BpTreeNode *pDataNode)
{
    if (!pDataNode)
    {
        return false;
    }

    auto cur_dataMap = pDataNode->getDataMap();
    if (!cur_dataMap)
    {
        return false;
    }

    int limit_size = (int)cur_dataMap->size();
    // return true if number of keys > (order - 1)
    if (limit_size > (order - 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Check if an index node has exceeded its maximum key count
bool BpTree::excessIndexNode(BpTreeNode *pIndexNode)
{
    if (!pIndexNode)
    {
        return false;
    }

    auto indexMap = pIndexNode->getIndexMap();
    if (!indexMap)
    {
        return false;
    }

    int limit_size = (int)indexMap->size();
    // Return true if number of keys > (order - 1)
    if (limit_size > (order - 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// Split an index node that has exceeded its maximum capacity
void BpTree::splitIndexNode(BpTreeNode *pIndexNode)
{
    if (!pIndexNode)
    {
        return;
    }

    auto leftMap = pIndexNode->getIndexMap();
    if (!leftMap)
    {
        return;
    }

    BpTreeIndexNode *rightIndex = new BpTreeIndexNode();

    int total = (int)leftMap->size();
    vector<pair<string, BpTreeNode *>> vec(leftMap->begin(), leftMap->end());
    int midIdx = total / 2;
    string promoteKey = vec[midIdx].first;

    // The promoted key's right child becomes the mostLeftChild of the new right node
    rightIndex->setMostLeftChild(vec[midIdx].second);

    // Move keys after the middle key to the new right node
    for (int i = midIdx + 1; i < total; i++)
    {
        rightIndex->insertIndexMap(vec[i].first, vec[i].second);
        vec[i].second->setParent(rightIndex);
        leftMap->erase(vec[i].first);
    }

    // Remove promoted key from the left node
    leftMap->erase(promoteKey);

    // Handle parent connection
    BpTreeNode *parent = pIndexNode->getParent();

    // If no parent, create a new root
    if (!parent)
    {
        BpTreeIndexNode *newRoot = new BpTreeIndexNode();
        newRoot->setMostLeftChild(pIndexNode);
        pIndexNode->setParent(newRoot);
        newRoot->insertIndexMap(promoteKey, rightIndex);
        rightIndex->setParent(newRoot);
        root = newRoot;
    }
    // Insert promoted key into the parent
    else
    {
        parent->insertIndexMap(promoteKey, rightIndex);
        rightIndex->setParent(parent);
        if (excessIndexNode(parent))
        {
            splitIndexNode(parent);
        }
    }
}

// Split a data node that has exceeded its maximum capacity
void BpTree::splitDataNode(BpTreeNode *pDataNode)
{
    if (!pDataNode)
    {
        return;
    }
    auto leftMap = pDataNode->getDataMap();
    if (!leftMap)
    {
        return;
    }

    BpTreeDataNode *rightNode = new BpTreeDataNode();

    int total = leftMap->size();
    int moveCount = total / 2;
    vector<pair<string, EmployeeData *>> temp;

    // Move the last half of the keys to the right node
    for (auto cur_iter = leftMap->rbegin(); cur_iter != leftMap->rend() && (int)temp.size() < moveCount; ++cur_iter)
    {
        temp.push_back({cur_iter->first, cur_iter->second});
    }

    for (auto &t : temp)
    {
        rightNode->insertDataMap(t.first, t.second);
        leftMap->erase(t.first);
    }

    // Connect leaf nodes in sequence
    rightNode->setNext(pDataNode->getNext());
    if (pDataNode->getNext())
    {
        pDataNode->getNext()->setPrev(rightNode);
    }
    rightNode->setPrev(pDataNode);
    pDataNode->setNext(rightNode);

    // Promoted key = first key in the new right node
    string promoteKey = rightNode->getDataMap()->begin()->first;

    BpTreeNode *parent = pDataNode->getParent();
    // If no parent, create a new root
    if (!parent)
    {
        BpTreeIndexNode *newRoot = new BpTreeIndexNode();
        newRoot->setMostLeftChild(pDataNode);
        newRoot->insertIndexMap(promoteKey, rightNode);
        pDataNode->setParent(newRoot);
        rightNode->setParent(newRoot);
        root = newRoot;
    }
    else
    {
        // Insert promoted key into parent
        parent->insertIndexMap(promoteKey, rightNode);
        rightNode->setParent(parent);

        // Update mostLeftChild if necessary
        if (parent->getMostLeftChild() == pDataNode)
        {
            parent->setMostLeftChild(pDataNode);
        }

        if (excessIndexNode(parent))
        {
            splitIndexNode(parent);
        }
    }
}

// Search for the leaf node containing the given key
BpTreeNode *BpTree::searchDataNode(string name)
{
    if (!root)
    {
        return nullptr;
    }
    BpTreeNode *cur = root;

    while (cur)
    {
        auto idxMap = cur->getIndexMap();
        if (idxMap)
        {

            auto cur_iter = idxMap->lower_bound(name);

            // If name is gerater than all keys, move to the last child
            if (cur_iter == idxMap->end())
            {
                auto last = std::prev(idxMap->end());

                cur = last->second;
            }
            // If matched exactly, move to the corresponding child
            else if (cur_iter->first == name)
            {
                cur = cur_iter->second;
            }
            // If name is smaller than all keys, go to mostLeftChild
            else
            {
                if (cur_iter == idxMap->begin())
                {
                    cur = cur->getMostLeftChild();
                }
                // Move to the child fo the previous key
                else
                {
                    auto prev = std::prev(cur_iter);
                    cur = prev->second;
                }
            }
        }
        // Reached a leaf node
        else
        {
            return cur;
        }
    }

    return nullptr;
}

// Search all employees whose names are within [start, end]
vector<EmployeeData *> BpTree::searchRange(string start, string end)
{
    vector<EmployeeData *> result;
    if (!root)
    {
        return result;
    }

    BpTreeNode *cur = searchDataNode(start);
    if (!cur)
    {
        return result;
    }

    // Traverse leaf nodes sequentially
    while (cur)
    {
        auto cur_dataMap = cur->getDataMap();
        if (!cur_dataMap)
            break;

        for (auto &t : *cur_dataMap)
        {
            string name = t.first;
            if (name >= start && name <= end)
            {
                result.push_back(t.second);
            }
            else if (name > end)
            {
                return result;
            }
        }
        // Move to the next leaf node
        cur = cur->getNext();
    }
    return result;
}