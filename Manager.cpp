#include "Manager.h"

void Manager::run(const char *command)
{
	fin.open(command);
	if (!fin.is_open())
	{
		return;
	}

	string cmd;
	while (fin >> cmd)
	{
		if (cmd == "LOAD")
		{
			LOAD();
		}
		else if (cmd == "ADD_BP")
		{
			ADD_BP();
		}
		else if (cmd == "SEARCH_BP")
		{
			string line;
			getline(fin, line); // 나머지 한 줄 통째로 읽기 (탭/공백 포함)
			stringstream ss(line);

			string arg1, arg2;
			ss >> arg1 >> arg2;

			if (arg1.empty()) // 인자 없음
			{
				printErrorCode(300);
			}
			else if (arg2.empty()) // 인자 1개
			{
				SEARCH_BP_NAME(arg1);
			}
			else // 인자 2개
			{
				SEARCH_BP_RANGE(arg1, arg2);
			}
		}
		else if (cmd == "PRINT_BP")
		{
			PRINT_BP();
		}
		else if (cmd == "ADD_ST")
		{
			string type;
			fin >> type;

			if (type == "dept_no")
			{
				int dept;
				fin >> dept;
				ADD_ST_DEPTNO(dept);
			}
			else if (type == "name")
			{
				string name;
				fin >> name;
				ADD_ST_NAME(name);
			}
			else
			{
				printErrorCode(600);
			}
		}
		else if (cmd == "PRINT_ST")
		{
			int dept;
			if (!(fin >> dept))
			{ // 인자 누락/형식 오류
				printErrorCode(600);
			}
			else
			{
				PRINT_ST(dept);
			}
		}
		else if (cmd == "DELETE")
		{
			DELETE();
		}
		else if (cmd == "EXIT")
		{
			if (bptree)
			{
				delete bptree;
				bptree = nullptr;
			}
			if (stree)
			{
				delete stree;
				stree = nullptr;
			}

			printSuccessCode("EXIT");
			break;
		}
		else
		{
			printErrorCode(800);
		}
	}

	fin.close();
}

void Manager::LOAD()
{
	// 이미 B+트리에 데이터가 있으면 ERROR
	if (bptree->getRoot())
	{
		printErrorCode(100);
		return;
	}

	// 파일 열기
	ifstream emp("employee.txt");
	if (!emp.is_open())
	{
		printErrorCode(100);
		return;
	}

	string name;
	int dept_no, id, income;

	// 데이터 읽고 삽입
	while (emp >> name >> dept_no >> id >> income)
	{
		EmployeeData *data = new EmployeeData();
		data->setData(name, dept_no, id, income);
		bptree->Insert(data);
	}

	emp.close();

	printSuccessCode("LOAD");
}

void Manager::ADD_BP()
{
	string name;
	int dept_no, id, income;

	// 인자 읽기
	if (!(fin >> name >> dept_no >> id >> income))
	{
		printErrorCode(200);
		return;
	}

	// EmployeeData 생성 및 삽입
	EmployeeData *data = new EmployeeData();
	data->setData(name, dept_no, id, income);

	bptree->Insert(data);

	// 출력
	flog << "========ADD_BP========\n";
	flog << name << "/" << dept_no << "/" << id << "/" << income << "\n";
	flog << "=====================\n\n";
}

void Manager::SEARCH_BP_NAME(string name)
{
	// 트리 자체가 비었으면 에러
	if (!bptree->getRoot())
	{
		printErrorCode(300);
		return;
	}

	// 해당 데이터가 포함된 리프 노드 찾기
	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (!leaf)
	{
		printErrorCode(300);
		return;
	}

	// 리프에서 데이터 검색
	auto dataMap = leaf->getDataMap();
	auto it = dataMap->find(name);

	// 찾는 이름이 없음
	if (it == dataMap->end())
	{
		printErrorCode(300);
		return;
	}

	// 성공 시 로그 출력
	EmployeeData *data = it->second;
	flog << "========SEARCH_BP========\n";
	flog << data->getName() << "/"
		 << data->getDeptNo() << "/"
		 << data->getID() << "/"
		 << data->getIncome() << "\n";
	flog << "=========================\n\n";
}

void Manager::SEARCH_BP_RANGE(string start, string end)
{
	auto results = bptree->searchRange(start, end);
	if (results.empty())
	{
		printErrorCode(400);
		return;
	}

	flog << "========SEARCH_BP========\n";
	for (auto *data : results)
	{
		flog << data->getName() << "/"
			 << data->getDeptNo() << "/"
			 << data->getID() << "/"
			 << data->getIncome() << "\n";
	}
	flog << "=========================\n\n";
}

void Manager::ADD_ST_DEPTNO(int dept_no)
{
	if (!bptree->getRoot())
	{
		printErrorCode(500);
		return;
	}

	if (!stree)
	{
		stree = new SelectionTree(&flog);
	}
	stree->setTree();

	// B+Tree의 가장 왼쪽 리프 노드로 이동
	BpTreeNode *cur = bptree->getRoot();
	while (cur->getMostLeftChild())
	{
		cur = cur->getMostLeftChild();
	}

	bool found = false;

	int leafIndex = 0;

	// 리프 순회
	while (cur)
	{

		auto dataMap = cur->getDataMap();
		if (!dataMap)
		{

			break;
		}

		for (auto &kv : *dataMap)
		{
			EmployeeData *data = kv.second;

			if (data->getDeptNo() == dept_no)
			{

				stree->Insert(data);
				found = true;
			}
		}

		// 다음 리프 노드로 이동
		BpTreeNode *nextNode = cur->getNext();
		cur = nextNode;
	}

	if (!found)
	{
		printErrorCode(500);
		return;
	}

	printSuccessCode("ADD_ST");
}

void Manager::ADD_ST_NAME(string name)
{

	if (!bptree->getRoot())
	{

		printErrorCode(500);
		return;
	}

	if (!stree)
	{

		stree = new SelectionTree(&flog);
	}
	stree->setTree();

	// 🔍 name 끝의 공백/개행 제거
	name.erase(name.find_last_not_of(" \n\r\t") + 1);

	// B+Tree에서 해당 직원 찾기
	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (!leaf)
	{

		printErrorCode(500);
		return;
	}

	auto dataMap = leaf->getDataMap();
	if (!dataMap)
	{

		printErrorCode(500);
		return;
	}

	auto it = dataMap->find(name);
	if (it == dataMap->end())
	{

		printErrorCode(500);
		return;
	}

	stree->Insert(it->second);

	printSuccessCode("ADD_ST");
}

void Manager::PRINT_BP()
{
	// 트리가 비어있으면 에러
	if (!bptree->getRoot())
	{
		printErrorCode(400);
		return;
	}

	// 루트에서 가장 왼쪽 리프 노드까지 이동
	BpTreeNode *cur = bptree->getRoot();
	while (cur->getMostLeftChild())
	{
		cur = cur->getMostLeftChild();
	}

	flog << "========PRINT_BP========\n";

	// 리프 순회
	while (cur)
	{
		auto dataMap = cur->getDataMap();
		if (!dataMap)
			break;

		for (auto it = dataMap->begin(); it != dataMap->end(); it++)
		{
			EmployeeData *data = it->second;
			flog << data->getName() << "/"
				 << data->getDeptNo() << "/"
				 << data->getID() << "/"
				 << data->getIncome() << "\n";
		}

		cur = cur->getNext(); // 다음 리프
	}

	flog << "=========================\n\n";
}

void Manager::PRINT_ST(int dept_no)
{
	// Selection Tree 미구성 or 루트 없음
	if (!stree || !stree->getRoot())
	{
		printErrorCode(600); // 문제 명세의 PRINT_ST 에러코드가 600
		return;
	}

	// SelectionTree 쪽 함수를 그대로 사용 (해당 함수가 flog로 직접 출력)
	bool ok = stree->printEmployeeData(dept_no);
	if (!ok)
	{
		// printEmployeeData 가 이미 에러 블록을 flog에 찍는다면 여기선 추가로 아무 것도 하지 않음
		// 만약 그 함수가 에러 출력 안한다면 아래 한 줄을 사용:
		// printErrorCode(600);
		return;
	}
}

void Manager::DELETE()
{
	if (!stree || !stree->getRoot())
	{
		printErrorCode(700);
		return;
	}

	bool success = stree->Delete();
	if (!success)
	{
		printErrorCode(700);
		return;
	}

	printSuccessCode("DELETE");
}

void Manager::printErrorCode(int n)
{
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

void Manager::printSuccessCode(string success)
{
	flog << "========" << success << "========\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}