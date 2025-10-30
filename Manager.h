#pragma once
#include "SelectionTree.h"
#include "BpTree.h"
#include <fstream>
#include <sstream>
class Manager
{
private:
	char *cmd;
	BpTree *bptree;
	SelectionTree *stree;

public:
	Manager(int bpOrder)
	{ // constructor
		flog.open("log.txt", ios::out | ios::trunc);
		bptree = new BpTree(&flog, bpOrder);
		stree = nullptr;
	}

	~Manager()
	{ // destructor
		if (flog.is_open())
		{
			flog.close();
		}
		if (bptree)
		{
			delete bptree;
		}
		if (stree)
		{
			delete stree;
		}
	}

	ifstream fin;
	ofstream flog;

	void run(const char *command);
	void LOAD();
	void ADD_BP();
	void SEARCH_BP_NAME(string name);
	void SEARCH_BP_RANGE(string start, string end);
	void PRINT_BP();
	void ADD_ST_DEPTNO(int dept_no);
	void ADD_ST_NAME(string name);
	void PRINT_ST(int dept_no);
	void DELETE();

	void printErrorCode(int n);
	void printSuccessCode(string success);
};
