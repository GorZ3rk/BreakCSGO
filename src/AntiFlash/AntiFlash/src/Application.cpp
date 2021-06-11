#include <iostream>
#include "mem.h"
#include "csgo.hpp"

memMain mem;

struct variables
{
	uintptr_t localPlayer;
	uintptr_t gameModule;
	int myTeam;
} val;




int main()
{
	uintptr_t procID = mem.getProcID(L"csgo.exe");

	val.gameModule = mem.getModuleAddr(procID, L"client.dll");

	val.localPlayer = mem.readMemory<uintptr_t>(val.gameModule + hazedumper::signatures::dwLocalPlayer);

	while (true)
	{
		uintptr_t antiFlash = mem.readMemory<uintptr_t>(val.localPlayer + hazedumper::netvars::m_flFlashDuration);

		if (antiFlash > 0)
		{
			mem.writeMemory<uintptr_t>(val.localPlayer + hazedumper::netvars::m_flFlashDuration, 0);
		}

	}


	return 0;
}