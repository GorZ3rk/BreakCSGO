### Memory Address

*Some important stuffs to calculate the address*

```
value = baseAddress + offset
```

```
localPlayer     = gameModule + localPlayer
myTeam          = localPlayer + team
myLocation      = localPlayer + vectorOrigin

crossHair       = localplayer + crossHair
leftButton      = gameModule + forceLeft
entity          = gameModule + entityList + ((crosshair - 1) * 0x10)
enermyTeam      = entity + team
enermyLocation  = entity + vectorOrigin
enermyHealth    = entity + health
```

### Structure

*offset*

[CSGO update offset GitHub](https://github.com/frk1/hazedumper)

*for glow rendering*
```C++
struct GlowStruct
{
    BYTE base[4];
    float red;          // 4 bytes
    float green;
    float blue;
    float alpha;
    BYTE buffer1[16];
    bool renderwhenOccluded;
    bool renderwhenUnOccluded;
    bool fullBloom;
    BYTE buffer2[5];
    int glowStyle;       
}
```

*location structure*
```C++
struct vector
{
    float x, y, z;
}
```



*user-defined structure*

```C++
struct variables
{
    uintptr_t localPlayer;
    uintptr_t gameModule;
    uintptr_t glowObject;
    int myTeam;
    ...
}
```


### TriggerBot




### AimBot

*calculate the distance*

```C++
struct vec3
{
    float x, y, z;  // location
}

vec3 operater+(const vec3 src, const vec3 dst)
{
    vec3 sum;
    sum.x = src.x + dst.x;
    sum.y = src.y + dst.y;
    sum.z = src.z + dst.z;
    return sum;
}

vec3 operator-(const vec3 src, const vec3 dst)
{
    vec3 diff;
    diff.x = src.x - dst.x;
    diff.y = src.y - dst.y;
    diff.z = src.z - dst.z;
    return diff;
}

```


internal
```
#include "stdafx.h"
#include <iostream>
#include "csgo.h"

using namespace offsets;

DWORD WINAPI HackThread(HMODULE hModule)
{
    //Create Console
    //AllocConsole();
    //FILE* f;
    //freopen_s(&f, "CONOUT$", "w", stdout);

    uintptr_t clientdll = (uintptr_t)GetModuleHandle("client.dll");

    bool bTriggerBot = false, bRadar = false;

    while (!(GetAsyncKeyState(VK_INSERT) &1))
    {
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
            bTriggerBot = !bTriggerBot;

        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
            bRadar = !bRadar;

        uintptr_t* localEntPtr = (uintptr_t*)(clientdll + dwLocalPlayer);

        if (localEntPtr)
        {
            uintptr_t localEnt = *localEntPtr;

            int localTeam = *(int*)(localEnt + m_iTeamNum);
          
            if (bTriggerBot)
            {
                int crosshairId = *(int*)(localEnt + m_iCrosshairId);

                if (crosshairId <= 64 && crosshairId != 0)
                {
                    uintptr_t crossHairEnt = *(uintptr_t*)(clientdll + dwEntityList + (crosshairId - 1) * 0x10);

                    if (crossHairEnt)
                    {
                        int crosshairTeam = *(int*)(crossHairEnt + m_iTeamNum);
                        int crosshairLifeState = *(int*)(crossHairEnt + m_lifeState);

                        if ((localTeam != crosshairTeam) && (crosshairLifeState == 0))
                        {
                            *(int*)(clientdll + dwForceAttack) = 5;
                            Sleep(30);
                            *(int*)(clientdll + dwForceAttack) = 4;
                        }
                        else *(int*)(clientdll + dwForceAttack) = 4;
                    }
                }
            }

            if (bRadar)
            {
                for (int i = 0; i < 64; i++)
                {
                    uintptr_t ent = *(uintptr_t*)(clientdll + dwEntityList + i * 0x10);
                    if (ent)
                    {
                        *(DWORD*)(ent + m_bSpotted) = true;
                    }
                }
            }
        }

        Sleep(5);
    }

    //fclose(f);
    //FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
```