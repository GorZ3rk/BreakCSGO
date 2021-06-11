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
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))
				{
					procID = procEntry.th32ProcessID;
					break;
				}


			} while (Process32Next(hSnap, &procEntry));
		}
	}

	this->handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, procID);

	CloseHandle(hSnap);
	return procID;
}

uintptr_t memMain::getModuleAddr(uintptr_t procID, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procID);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hSnap, &modEntry))
		{
			MODULEENTRY32 modEntry = { 0 };
			modEntry.dwSize = sizeof(MODULEENTRY32);

			if (Module32First(hSnap, &modEntry))
			{
				do
				{
					if (!_wcsicmp(modEntry.szModule, modName))
					{
						modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
						break;
					}
				} while (Module32Next(hSnap, &modEntry));
			}
		}


	}

	CloseHandle(hSnap);
	return modBaseAddr;
}

