#include <stdio.h>
#include <stdlib.h>

#define UNITQ15 0x7fff

float fcos1(float x)
{
    float fcoef[4] = {1.0, -1 / 2.0, 1.0 / (2.0 * 3.0 * 4.0), -1.0 / (2.0 * 3.0 * 4.0 * 5.0 * 6.0)};
    float out;
    out = fcoef[0];
    out += fcoef[1] * x * x;
    out += fcoef[2] * x * x * x * x;
    out += fcoef[3] * x * x * x * x * x * x;
    return out;
}

float fcos2(float x)
{
    float fcoef[4] = {1.0, -1 / 2.0, 1.0 / (2.0 * 3.0 * 4.0), -1.0 / (2.0 * 3.0 * 4.0 * 5.0 * 6.0)};
    float out, x2;
    x2 = x * x;
    out = x2 * fcoef[3];
    out = (out + fcoef[2]) * x2;
    out = (out + fcoef[1]) * x2;
    out += fcoef[0];
    return out;
}

short icos(short x)
{
    short iCoef[4] = {(short)(UNITQ15), (short)(-(UNITQ15 / 2.0)), (short)(UNITQ15 / (2.0 * 3.0 * 4.0)), (short)(-(UNITQ15 / (2.0 * 3.0 * 4.0 * 5.0 * 6.0)))};
    long cosine, z;
    short x2;
    z = (long)x * x;
    x2 = (short)(z >> 15); // x2 has x(Q14)*x(Q14)
    cosine = (long)iCoef[3] * x2;
    cosine = cosine >> 13;
    cosine = (cosine + (long)iCoef[2]) * x2;
    cosine = cosine >> 13;
    cosine = (cosine + (long)iCoef[1]) * x2;
    cosine = cosine >> 13;
    cosine = cosine + iCoef[0];
    return ((short)cosine);
}

int main()
{
    int i;
    float out1[7], out2[7];
    short out3[7];
    float theta[7] = {0.0, 0.1, -0.8, 19.373155, -0.523592, 1.5707963, 3.141592};
    float real_cos[7] = {1.0, 0.9950, 0.6967, 0.8660, 0.8660, 0.0000, -1.0};
    for (i = 0; i < 7; i++)
    {
        out1[i] = fcos1(theta[i]);
        out2[i] = fcos2(theta[i]);
        out3[i] = icos((short)(UNITQ15 / 2 * theta[i]));
        printf("real cos(%f): %f fcos1: %f fcos2: %f icos: %f\n", theta[i], real_cos[i], out1[i], out2[i], (float)out3[i] / UNITQ15);
    }
}