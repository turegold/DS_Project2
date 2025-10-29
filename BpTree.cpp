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

    // 새 오른쪽 리프 노드 생성
    BpTreeDataNode *rightNode = new BpTreeDataNode();

    // 데이터 개수 절반 계산
    int total = leftMap->size();
    int moveCount = total / 2; // 절반 오른쪽으로 이동
    vector<pair<string, EmployeeData *>> temp;

    // 뒤쪽 moveCount개를 오른쪽 노드로 옮기기 위해 저장
    for (auto it = leftMap->rbegin(); it != leftMap->rend() && (int)temp.size() < moveCount; it++)
    {
        temp.push_back({it->first, it->second});
    }

    // 이동: 오른쪽 노드에 추가하고, 왼쪽에서 삭제
    for (auto &kv : temp)
    {
        rightNode->insertDataMap(kv.first, kv.second);
        leftMap->erase(kv.first);
    }

    // 리프 간 next / prev 연결
    rightNode->setNext(pDataNode->getNext());
    if (pDataNode->getNext())
    {
        pDataNode->getNext()->setPrev(rightNode);
    }
    rightNode->setPrev(pDataNode);
    pDataNode->setNext(rightNode);

    // 오른쪽 리프의 첫 번째 key를 승격 키로 설정
    string promoteKey = rightNode->getDataMap()->begin()->first;

    // 부모 인덱스 노드 처리
    BpTreeNode *parent = pDataNode->getParent();
    if (!parent)
    {
        // 부모가 없으면 새 루트 인덱스 노드 생성
        BpTreeIndexNode *newRoot = new BpTreeIndexNode();
        newRoot->setMostLeftChild(pDataNode);

        // 오른쪽 리프를 승격키로 연결
        newRoot->insertIndexMap(promoteKey, rightNode);
        pDataNode->setParent(newRoot);
        rightNode->setParent(newRoot);

        // 루트 갱신
        root = newRoot;
    }
    else
    {
        // 기존 부모가 있다면 부모의 map에 승격 키 추가
        parent->insertIndexMap(promoteKey, rightNode);
        rightNode->setParent(parent);

        // 부모도 꽉 찼으면 인덱스 분할
        if (excessIndexNode(parent))
        {
            splitIndexNode(parent);
        }
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

    int total = leftMap->size();
    vector<pair<string, BpTreeNode *>> vec(leftMap->begin(), leftMap->end());
    int midIdx = total / 2;
    string promoteKey = vec[midIdx].first;

    // 오른쪽 노드에 midIdx 이후의 데이터 이동
    for (int i = midIdx + 1; i < total; i++)
    {
        rightIndex->insertIndexMap(vec[i].first, vec[i].second);
        vec[i].second->setParent(rightIndex);
        leftMap->erase(vec[i].first);
    }

    // promoteKey는 부모로 올리기 전에 삭제
    leftMap->erase(promoteKey);

    // 승격키의 오른쪽 child를 rightIndex의 mostLeftChild로 설정
    rightIndex->setMostLeftChild(vec[midIdx + 1].second);

    BpTreeNode *parent = pIndexNode->getParent();
    if (!parent)
    {
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

BpTreeNode *BpTree::searchDataNode(string name)
{
    // 빈 트리면 nullptr
    if (!root)
    {
        return nullptr;
    }

    BpTreeNode *cur = root;

    while (cur)
    {
        // 인덱스 노드인지 확인
        auto idxMap = cur->getIndexMap();
        if (idxMap)
        {
            // name보다 큰 첫 키
            auto it = idxMap->upper_bound(name);
            if (it == idxMap->begin())
            {
                // 가장 왼쪽 자식으로
                cur = cur->getMostLeftChild();
            }
            else
            {
                // 바로 이전 키의 child로
                it--;
                cur = it->second;
            }
        }
        else
        {
            // 리프 노드 도착
            return cur;
        }
    }
    return nullptr;
}

void BpTree::searchRange(string start, string end)
{
    if (!root)
        return;

    // ① start가 포함된 리프 노드 탐색
    BpTreeNode *cur = searchDataNode(start);
    if (!cur)
        return;

    bool found = false;

    // ② 리프를 오른쪽으로 순회 (next 포인터 이용)
    while (cur)
    {
        auto dataMap = cur->getDataMap();
        if (!dataMap)
            break; // 혹시 인덱스 노드일 경우 방어

        // ③ 현재 리프의 모든 데이터 확인
        for (auto it = dataMap->begin(); it != dataMap->end(); ++it)
        {
            string name = it->first;
            if (name >= start && name <= end)
            {
                found = true;
                EmployeeData *data = it->second;
                cout << "name: " << data->getName()
                     << ", dept_no: " << data->getDeptNo()
                     << ", ID: " << data->getID()
                     << ", Income: " << data->getIncome() << endl;
            }
            else if (name > end)
            {
                // end를 넘어서면 탐색 종료
                return;
            }
        }

        // ④ 다음 리프 노드로 이동
        cur = cur->getNext();
    }

    if (!found)
    {
        cout << "No data in range [" << start << ", " << end << "]" << endl;
    }
}