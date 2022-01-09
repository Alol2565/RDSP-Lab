#include <stdio.h>
#include <stdlib.h>

#define NUM_SECTIONS 2
#define UNITQ15 0x7fff
#define N 4000

void read_file(short *arr1, short *arr2, short *arr3)
{
    double help;
    int i, j;
    FILE *f;
    char c;
    for (j = 0; j < 3; j++)
    {
        i = 0;
        if (j == 0)
        {
            f = fopen("SOS_mat.txt", "r");
            while (i < 12)
            {
                fscanf(f, "%lf %c", &help, &c);
                arr1[i] = (short)(UNITQ15 * help);
                i += 1;
            }
            fclose(f);
        }
        else if (j == 1)
        {
            f = fopen("Gain.txt", "r");
            while (!feof(f))
            {
                fscanf(f, "%lf %c", &help, &c);
                arr2[i] = (short)(UNITQ15 * help);
                i += 1;
            }
            fclose(f);
        }
        else if (j == 2)
        {
            f = fopen("input.txt", "r");
            while (!feof(f))
            {
                fscanf(f, "%lf %c", &help, &c);
                arr3[i] = (short)(UNITQ15 * help);
                i += 1;
            }
            fclose(f);
        }
    }
}

void IIR_Filter(short b[2][3], short a[2][2], short scale[3], short x[N], short *y)
{
    int section;
    short w[NUM_SECTIONS][2] = {0}, wn, yn, input;
    long temp;
    short help;
    for (int i = 0; i < N; i++)
    {
        input = x[i];
        for (section = 0; section < NUM_SECTIONS; section++)
        {
            temp = (long)input - (long)a[section][0] * w[section][0] - (long)a[section][1] * w[section][1];
            wn = (short)(temp >> 11);
            temp = (long)b[section][0] * wn + (long)b[section][1] * w[section][0] + (long)b[section][2] * w[section][1];
            help = (long)(temp >> 11);
            temp = (long)help * scale[section];
            yn = (short)(temp >> 11);
            w[section][1] = w[section][0];
            w[section][0] = wn;
            input = yn;
        }
        y[i] = (short)(yn >> 4);
    }
    return;
}

int main()
{
    int i, j, k;
    short SOS[12], scale[3], b[2][3], a[2][2];
    short *x = (short *)malloc(sizeof(short) * N);
    short *y = (short *)malloc(sizeof(short) * N);
    float *z = (float *)malloc(sizeof(float) * N);

    read_file(SOS, scale, x);
    for (i = 0; i < 12; i++)
    {
        j = i % 6;
        k = i / 6;
        if (j < 3)
            b[k][j] = SOS[i];
        if (j > 3)
            a[k][j - 4] = SOS[i];
    }

    IIR_Filter(b, a, scale, x, y);

    for (i = 0; i < N; i++)
    {
        z[i] = (float)y[i] / UNITQ15;
    }

    FILE *fp = fopen("out.txt", "w");
    if (fp == NULL)
    {
        return -1;
    }
    for (i = 0; i < N; i++)
        fprintf(fp, "%f\n", z[i]);
    fclose(fp);

    free(x);
    free(y);

    return 0;
}