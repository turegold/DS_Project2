#include "BpTree.h"
#include <cassert>

bool BpTree::Insert(EmployeeData *newData)
{
    if (!newData)
    {
        return false;
    }
    string key = newData->getName();

    // 트리가 비어있을 경우
    if (!root)
    {
        BpTreeDataNode *newLeaf = new BpTreeDataNode();
        newLeaf->insertDataMap(key, newData);
        root = newLeaf;
        return true;
    }

    // 삽입할 리프 노드 찾기
    BpTreeNode *pLeaf = searchDataNode(key);
    if (!pLeaf)
    {
        return false;
    }

    // 이미 존재하는 name일 경우 연봉만 갱신
    auto dataMap = pLeaf->getDataMap();
    auto it = dataMap->find(key);
    if (it != dataMap->end())
    {
        it->second->setIncome(newData->getIncome());
        return true;
    }

    // 데이터 삽입
    pLeaf->insertDataMap(key, newData);

    // 리프 노드가 꽉 찼는지 검사
    if (excessDataNode(pLeaf))
    {
        splitDataNode(pLeaf);
    }

    return true;
}

bool BpTree::excessDataNode(BpTreeNode *pDataNode)
{
    if (!pDataNode)
    {
        return false;
    }

    // 리프 노드만 검사 (인덱스 노드가 들어오면 false)
    auto dataMap = pDataNode->getDataMap();
    if (!dataMap)
    {
        return false;
    }

    // 데이터 개수가 (order - 1)을 초과하면 true
    if ((int)dataMap->size() > (order - 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

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

    // 키 개수가 (order - 1)을 초과하면 true
    if ((int)indexMap->size() > (order - 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void BpTree::splitIndexNode(BpTreeNode *pIndexNode)
{
    if (!pIndexNode)
        return;

    auto leftMap = pIndexNode->getIndexMap();
    if (!leftMap)
        return;

    BpTreeIndexNode *rightIndex = new BpTreeIndexNode();

    int total = (int)leftMap->size();
    vector<pair<string, BpTreeNode *>> vec(leftMap->begin(), leftMap->end());
    int midIdx = total / 2;
    string promoteKey = vec[midIdx].first;

    // ✅ 오른쪽 인덱스 노드의 mostLeftChild는 승격키의 "오른쪽 child"
    rightIndex->setMostLeftChild(vec[midIdx].second);

    // 오른쪽 노드로 midIdx+1 ~ end 이동
    for (int i = midIdx + 1; i < total; i++)
    {
        rightIndex->insertIndexMap(vec[i].first, vec[i].second);
        vec[i].second->setParent(rightIndex);
        leftMap->erase(vec[i].first);
    }

    // promoteKey는 왼쪽 노드에서 제거
    leftMap->erase(promoteKey);

    // 부모 처리
    BpTreeNode *parent = pIndexNode->getParent();
    if (!parent)
    {
        // 새 루트 생성
        BpTreeIndexNode *newRoot = new BpTreeIndexNode();
        newRoot->setMostLeftChild(pIndexNode);
        pIndexNode->setParent(newRoot);
        newRoot->insertIndexMap(promoteKey, rightIndex);
        rightIndex->setParent(newRoot);
        root = newRoot;
    }
    else
    {
        parent->insertIndexMap(promoteKey, rightIndex);
        rightIndex->setParent(parent);
        if (excessIndexNode(parent))
            splitIndexNode(parent);
    }
}

void BpTree::splitDataNode(BpTreeNode *pDataNode)
{
    if (!pDataNode)
        return;
    auto leftMap = pDataNode->getDataMap();
    if (!leftMap)
        return;

    BpTreeDataNode *rightNode = new BpTreeDataNode();

    int total = leftMap->size();
    int moveCount = total / 2;
    vector<pair<string, EmployeeData *>> temp;

    for (auto it = leftMap->rbegin(); it != leftMap->rend() && (int)temp.size() < moveCount; ++it)
        temp.push_back({it->first, it->second});

    for (auto &kv : temp)
    {
        rightNode->insertDataMap(kv.first, kv.second);
        leftMap->erase(kv.first);
    }

    // 리프 간 연결
    rightNode->setNext(pDataNode->getNext());
    if (pDataNode->getNext())
        pDataNode->getNext()->setPrev(rightNode);
    rightNode->setPrev(pDataNode);
    pDataNode->setNext(rightNode);

    // 승격 키 = 오른쪽 리프의 첫 번째 키
    string promoteKey = rightNode->getDataMap()->begin()->first;

    BpTreeNode *parent = pDataNode->getParent();
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
        // ✅ 기존 부모에 승격키 추가 (오른쪽 구간을 rightNode로 연결)
        parent->insertIndexMap(promoteKey, rightNode);
        rightNode->setParent(parent);

        // ✅ 혹시 부모의 가장 왼쪽 child였을 경우 갱신 필요
        // (즉, pDataNode가 parent's mostLeftChild였을 때)
        if (parent->getMostLeftChild() == pDataNode)
            parent->setMostLeftChild(pDataNode);

        if (excessIndexNode(parent))
            splitIndexNode(parent);
    }
}

BpTreeNode *BpTree::searchDataNode(string name)
{
    if (!root)
        return nullptr;

    BpTreeNode *cur = root;

    while (cur)
    {
        auto idxMap = cur->getIndexMap();
        if (idxMap)
        {

            auto it = idxMap->lower_bound(name);

            if (it == idxMap->end())
            {
                // name이 모든 키보다 클 때: 마지막 키의 child로
                auto last = std::prev(idxMap->end());

                cur = last->second;
            }
            else if (it->first == name)
            {
                // 정확히 일치: 그 키의 child로

                cur = it->second;
            }
            else
            {
                // it->first > name
                if (it == idxMap->begin())
                {
                    // name이 가장 작은 키보다 작을 때: mostLeftChild로

                    cur = cur->getMostLeftChild();
                }
                else
                {
                    // name보다 바로 작은 키의 child로
                    auto prev = std::prev(it);

                    cur = prev->second;
                }
            }
        }
        else
        {

            return cur;
        }
    }

    return nullptr;
}

vector<EmployeeData *> BpTree::searchRange(string start, string end)
{
    vector<EmployeeData *> result;
    if (!root)
        return result;

    BpTreeNode *cur = searchDataNode(start);
    if (!cur)
        return result;

    while (cur)
    {
        auto dataMap = cur->getDataMap();
        if (!dataMap)
            break;

        for (auto &kv : *dataMap)
        {
            string name = kv.first;
            if (name >= start && name <= end)
                result.push_back(kv.second);
            else if (name > end)
                return result;
        }
        cur = cur->getNext();
    }
    return result;
}