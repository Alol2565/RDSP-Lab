#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define N 1000
#define SIGNAL_LENGTH 145209

void echo(double *x, double *y, double g)
{
    for (int i = 0; i < N; i++)
    {
        y[i] = 0.;
    }
    for (int i = N; i < SIGNAL_LENGTH; i++)
    {
        y[i] = g * y[i - N] + x[i] + (g - 1) * x[i - N];
    }
}

int main()
{

    double y[SIGNAL_LENGTH];
    double g = 0.8;
    echo(signal, y, g);
    FILE *fp1;
    fp1 = fopen("echo.csv", "w"); //create a fil
    for (int i = 0; i < SIGNAL_LENGTH; i++)
    {
        if(i == SIGNAL_LENGTH - 1)
        {
            fprintf(fp1, "%0.30f", y[i]);
        }
        else{
            fprintf(fp1, "%0.30f%s", y[i], ",");
        }
        
        
    }
}