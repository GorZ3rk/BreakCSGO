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


### Triggerbot

