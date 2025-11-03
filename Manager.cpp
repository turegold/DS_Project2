#include "Manager.h"

void Manager::run(const char *command)
{
	// Open command.txt
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
			getline(fin, line);
			stringstream ss(line);

			string arg1, arg2;
			ss >> arg1 >> arg2;

			if (arg1.empty())
			{
				printErrorCode(300);
			}
			else if (arg2.empty())
			{
				SEARCH_BP_NAME(arg1);
			}
			else
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
			if (fin.fail())
			{
				fin.clear();
			}
			string type;

			if (!(fin >> type))
			{
				printErrorCode(500);

				fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				fin.clear();
				continue;
			}

			if (type == "dept_no")
			{
				int dept;
				if (!(fin >> dept))
				{
					printErrorCode(500);

					fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					fin.clear();
					continue;
				}
				ADD_ST_DEPTNO(dept);
			}
			else if (type == "name")
			{
				string name;
				if (!(fin >> name))
				{
					printErrorCode(500);

					fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
					fin.clear();
					continue;
				}
				ADD_ST_NAME(name);
			}
			else
			{

				printErrorCode(500);

				fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
				fin.clear();
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

			fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			fin.clear();
		}
	}

	fin.close();
}

// Load data from employee.txt into B+ Tree
void Manager::LOAD()
{
	if (bptree->getRoot())
	{
		printErrorCode(100);
		return;
	}

	// Open employee.txt
	ifstream emp("employee.txt");
	if (!emp.is_open())
	{
		printErrorCode(100);
		return;
	}

	string name;
	int dept_no, id, income;

	// Read data and insert data into B+ tree
	while (emp >> name >> dept_no >> id >> income)
	{
		EmployeeData *data = new EmployeeData();
		data->setData(name, dept_no, id, income);
		bptree->Insert(data);
	}

	emp.close();

	printSuccessCode("LOAD");
}

// Add a single employee to the B+ Tree
void Manager::ADD_BP()
{
	string name;
	int dept_no, id, income;

	// Read datas
	if (!(fin >> name >> dept_no >> id >> income))
	{
		printErrorCode(200);

		fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		fin.clear();
		return;
	}

	// Create EmployeeData and Insert to the B+ Tree
	EmployeeData *data = new EmployeeData();
	data->setData(name, dept_no, id, income);

	bptree->Insert(data);

	// Print
	flog << "========ADD_BP========\n";
	flog << name << "/" << dept_no << "/" << id << "/" << income << "\n";
	flog << "=====================\n\n";
}

// Search an employee by exact name in B+ Tree
void Manager::SEARCH_BP_NAME(string name)
{

	if (!bptree->getRoot())
	{
		printErrorCode(300);
		return;
	}

	// Find leaf node which contains specific data
	BpTreeNode *leaf = bptree->searchDataNode(name);
	if (!leaf)
	{
		printErrorCode(300);
		return;
	}

	// Search data from leaf node
	auto dataMap = leaf->getDataMap();
	auto it = dataMap->find(name);

	// Not found
	if (it == dataMap->end())
	{
		printErrorCode(300);
		return;
	}

	// Print if success
	EmployeeData *data = it->second;
	flog << "========SEARCH_BP========\n";
	flog << data->getName() << "/"
		 << data->getDeptNo() << "/"
		 << data->getID() << "/"
		 << data->getIncome() << "\n";
	flog << "=========================\n\n";
}

// Search all employees whose names are in [start, end]
void Manager::SEARCH_BP_RANGE(string start, string end)
{
	// Search data in range
	auto results = bptree->searchRange(start, end);
	if (results.empty())
	{
		printErrorCode(300);
		return;
	}

	// Print
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

// Add all employees in the given department to Selection Tree
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

	// Set tree
	stree->setTree();

	// Move mostLeftChild
	BpTreeNode *cur = bptree->getRoot();
	while (cur->getMostLeftChild())
	{
		cur = cur->getMostLeftChild();
	}

	bool found = false;

	int leafIndex = 0;

	// Traverse all leaf nodes from left to right
	while (cur)
	{

		auto dataMap = cur->getDataMap();
		if (!dataMap)
		{

			break;
		}

		// check each employee in the current leaf
		for (auto &kv : *dataMap)
		{
			EmployeeData *data = kv.second;

			// Insert employees with matching dept_no
			if (data->getDeptNo() == dept_no)
			{
				stree->Insert(data);
				found = true;
			}
		}

		// Move to the next leaf node
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

// Add a single employee to the Selection Tree by name
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

	name.erase(name.find_last_not_of(" \n\r\t") + 1);

	// Find the employee in the B+ Tree by name
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

	// Insert the found employee into Selection Tree
	stree->Insert(it->second);

	printSuccessCode("ADD_ST");
}

// Print all employees stored in B+ Tree in ascending order by name
void Manager::PRINT_BP()
{

	if (!bptree->getRoot())
	{
		printErrorCode(400);
		return;
	}

	// Move to the leftmost leaf node
	BpTreeNode *cur = bptree->getRoot();
	while (cur->getMostLeftChild())
	{
		cur = cur->getMostLeftChild();
	}

	flog << "========PRINT_BP========\n";

	// Traverse leaf nodes and print all data
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

		// Move to next leaf
		cur = cur->getNext();
	}

	flog << "=========================\n\n";
}

// Print all employees in the given department from Selection Tree
void Manager::PRINT_ST(int dept_no)
{

	if (!stree || !stree->getRoot())
	{
		printErrorCode(600);

		fin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		fin.clear();
		return;
	}

	// Print department data using Selection Tree
	bool ok = stree->printEmployeeData(dept_no);
	if (!ok)
	{
		return;
	}
}

// Delete the root employee from Selection Tree
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

// Print error code
void Manager::printErrorCode(int n)
{
	flog << "========ERROR========\n";
	flog << n << "\n";
	flog << "=====================\n\n";
}

// Print success message
void Manager::printSuccessCode(string success)
{
	flog << "========" << success << "========\n";
	flog << "Success" << "\n";
	flog << "====================\n\n";
}