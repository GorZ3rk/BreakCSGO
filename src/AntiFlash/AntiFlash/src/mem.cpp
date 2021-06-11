#include <Windows.h>
#include "mem.h"

memMain::memMain()
{

}

memMain::~memMain()
{

}

uintptr_t memMain::getProcID(const wchar_t* procName)
{
	uintptr_t procID = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{

	}




}