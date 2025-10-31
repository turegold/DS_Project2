#include "SelectionTree.h"

void SelectionTree::setTree()
{
    // 이미 루트가 있더라도 Winner Tree를 다시 구성하도록 변경
    if (!root)
    {
        root = new SelectionTreeNode();
        root->HeapInit();
    }

    EmployeeData *best = nullptr;
    SelectionTreeNode *bestNode = nullptr;

    // run[] 중 가장 높은 연봉자 찾기
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap() && !run[i]->getHeap()->IsEmpty())
        {
            EmployeeData *top = run[i]->getHeap()->Top();

            if (!best || top->getIncome() > best->getIncome())
            {
                best = top;
                bestNode = run[i];
            }
        }
    }

    // 루트에 최고 연봉자 설정
    if (best)
    {
        root->setEmployeeData(best);
        root->setHeap(bestNode->getHeap());
    }
    else
    {
        // 모든 run이 비었을 경우
        root->setEmployeeData(nullptr);
        root->setHeap(nullptr);
    }
}

bool SelectionTree::Insert(EmployeeData *newData)
{
    if (!newData)
        return false;

    int dept = newData->getDeptNo();

    // 이미 같은 부서번호의 run이 존재하면 해당 Heap에 추가
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getEmployeeData()->getDeptNo() == dept)
        {
            run[i]->getHeap()->Insert(newData);
            return true;
        }
    }

    // 빈 슬롯(run)이 있으면 새 노드 생성
    for (int i = 0; i < 8; i++)
    {
        if (run[i] == nullptr)
        {
            SelectionTreeNode *newNode = new SelectionTreeNode();
            newNode->setEmployeeData(newData);
            newNode->HeapInit();
            newNode->getHeap()->Insert(newData);
            run[i] = newNode;
            return true;
        }
    }

    // run[]이 꽉 찬 경우
    return false;
}

bool SelectionTree::Delete()
{
    if (!root || !root->getHeap())
        return false;

    EmployeeHeap *heap = root->getHeap();
    if (heap->IsEmpty())
        return false;

    // 루트 힙에서 최고 연봉자 제거
    heap->Delete();

    // ✅ 여기서 새 top으로 갱신 (핵심)
    if (!heap->IsEmpty())
    {
        EmployeeData *newTop = heap->Top();
        root->setEmployeeData(newTop);
    }

    // 힙이 비면 run[]에서 제거
    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap() == heap && heap->IsEmpty())
        {
            delete run[i];
            run[i] = nullptr;
            break;
        }
    }

    // Winner Tree 재선정 (나머지 run 중 최고 연봉자 찾기)
    EmployeeData *best = nullptr;
    SelectionTreeNode *bestNode = nullptr;

    for (int i = 0; i < 8; i++)
    {
        if (run[i] && run[i]->getHeap() && !run[i]->getHeap()->IsEmpty())
        {
            EmployeeData *top = run[i]->getHeap()->Top();
            if (!best || top->getIncome() > best->getIncome())
            {
                best = top;
                bestNode = run[i];
            }
        }
    }

    if (bestNode)
    {
        root->setEmployeeData(best);
        root->setHeap(bestNode->getHeap());
        return true;
    }
    else
    {
        delete root;
        root = nullptr;
        return false;
    }
}

bool SelectionTree::printEmployeeData(int dept_no)
{
    if (!fout)
        return false;
    std::ostream &out = *fout;

    bool found = false;

    for (int i = 0; i < 8; i++)
    {
        if (!run[i] || !run[i]->getEmployeeData())
            continue;

        if (run[i]->getEmployeeData()->getDeptNo() == dept_no)
        {
            EmployeeHeap *heap = run[i]->getHeap();
            if (!heap || heap->IsEmpty())
                return false;

            out << "========PRINT_ST " << dept_no << "========\n";

            // ① 힙에서 다 꺼내 임시 보관
            std::vector<EmployeeData *> buf;
            while (!heap->IsEmpty())
            {
                buf.push_back(heap->Top());
                heap->Delete();
            }

            // ② 임시 버퍼로 출력
            for (auto *e : buf)
            {
                out << e->getName() << "/"
                    << e->getDeptNo() << "/"
                    << e->getID() << "/"
                    << e->getIncome() << "\n";
            }

            // ③ 다시 힙에 되돌려 넣기 (원본 보존)
            for (auto *e : buf)
                heap->Insert(e);

            out << "=========================\n\n";
            found = true;
            break;
        }
    }

    if (!found)
    {
        out << "========ERROR========\n";
        out << "400\n";
        out << "=====================\n\n";
        return false;
    }
    return true;
}