### Basics

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



### Triggerbot

