
vec3 CalcAngle(vec3 src, vec3 dst)
{
    vec3 vForward = dst - src;
    vec3 vAngle;

    if (vForward.y == 0 && vForward.x == 0)
    {
        vAngle.y = 0;
        if (vForward.z > 0)
            vAngle.x = 270;
        else 
            vAngle.x = 90;
    }
    else
    {
        vAngle.y = (atan2(vForward.y, vForward.x) * 180 / pi);

        if (vAngle.y < 0)
            vAngle.y += 360;

        float dotproduct  = sqrt(vForward.x * vForward.x + vForward.y * vForward.y);
        vAngle.x = (atan2(-vForward.z, dotproduct) * 180 / pi);

        if (vAngle.x < 0)
            vAngle.x += 360;
    }

    vAngle.z = 0;

    return vAngle;
}

void CalcAngle(Vector src, Vector dst, Vector &angles)
{
    Vector delta = src - dst;
    double hyp = delta.Length2D();
    angles.y = (atan(delta.y / delta.x) * 57.295779513082f);
    angles.x = (atan(delta.z / hyp) * 57.295779513082f);
    angles[2] = 0.00;

    if (delta.x >= 0.0)
        angles.y += 180.0f;
}



```
    Netsh interface IP set address "local connection" source=dhcp
     Netsh interface IP set dns "local connection" source=dhcp
    pause

    @echo off
 Echo is setting ip, subnet, please wait...
 Rem "local connection" is the name of the network card, which can be queried in the network connection, such as "local connection"
 Netsh interface ip set address name="local connection" source=static addr=10.200.49.136 mask=255.0.0.0 gateway=10.200.49.58 gwmetric=1
echo.
 Echo is setting DNS, please wait.....
 Netsh int ip set dns name="local connection" source=static addr=172.20.0.11
 Netsh int ip add dns name="local connection" addr=172.20.0.12 index=2
 Echo is set!
ipconfig
pause
close

```