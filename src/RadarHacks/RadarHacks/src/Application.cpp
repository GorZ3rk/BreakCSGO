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

	//val.localPlayer = mem.readMemory<uintptr_t>(val.gameModule + hazedumper::signatures::dwLocalPlayer);

	while (true)
	{
		for (short int i = 0; i < 64; i++)
		{
			uintptr_t entity = mem.readMemory<uintptr_t>(val.gameModule + hazedumper::signatures::dwEntityList + i * 0x10);
			if (entity != NULL)
				mem.writeMemory<bool>(entity + hazedumper::netvars::m_bSpotted, true);
		}

	}

	return 0;
}