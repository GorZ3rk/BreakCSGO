#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <vector>


class memMain
{
public:
	memMain();
	~memMain();

	uintptr_t getProcID(const wchar_t*);

	uintptr_t getModuleAddr(uintptr_t, const wchar_t*);

	template <class T>
	T readMemory(uintptr_t addr)
	{
		T val;
		ReadProcessMemory(handle, (BYTE*)addr, &val, sizeof(T), NULL);
		return val;
	}

	template <class T>
	void writeMemory(uintptr_t addr, T val)
	{
		WriteProcessMemory(handle, (BYTE*)addr, &val, sizeof(T), NULL);
	}


	HANDLE handle;
};