#include <iostream>
#include "mem.h"
#include "csgo.hpp"

memMain mem;

struct variables
{
	uintptr_t localPlayer;
	uintptr_t gameModule;
	BYTE flag;

} val;




int main()
{
	uintptr_t procID = mem.getProcID(L"csgo.exe");

	val.gameModule = mem.getModuleAddr(procID, L"client.dll");

	val.localPlayer = mem.readMemory<uintptr_t>(val.gameModule + hazedumper::signatures::dwLocalPlayer);

	while (true)
	{

		val.flag = mem.readMemory<BYTE>(val.localPlayer + hazedumper::netvars::m_fFlags);

		if (GetAsyncKeyState(VK_SPACE) && val.flag & (1 << 0))
		{
			mem.writeMemory<uintptr_t>(val.gameModule + hazedumper::signatures::dwForceJump, 6);
		}

		Sleep(1);

	}

	return 0;
}