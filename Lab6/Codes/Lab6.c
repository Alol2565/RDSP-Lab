#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Complex.h"

#define N 1024
#define PI 3.1415926

double AWGN_generator()
{
    double temp1;
    double temp2;
    double result;
    int p;

    p = 1;

    while (p > 0)
    {
        temp2 = (rand() / ((double)RAND_MAX));
        if (temp2 == 0)
        {
            p = 1;
        }
        else
        {
            p = -1;
        }
    }

    temp1 = cos((2.0 * (double)PI) * rand() / ((double)RAND_MAX));
    result = sqrt(-2.0 * log(temp2)) * temp1;
    return result;
}

void fft(complex *a, int n, float dt);

void ISR(complex *ping, complex *pong, float *input)
{
    for (int i = 0; i < N; i++)
    {
        ping[i].x = input[i];
        pong[i].x = input[i];
    }
    for (int i = N; i < 2 * N; i++)
    {
        ping[i - N].y = input[i];
        pong[i - N].y = input[i];
    }
}


int main()
{
    // Initialization

    int i, fs = 16000;
    int L = 8;
    float dt = 1. / fs;
    complex x[N];

    float spectrum[513];

    for (int k = 0; k < 513; k++)
    {
        spectrum[k] = 0.0;
    }

    complex ping[N];
    complex pong[N];

    float input[N * L];
    float serial[N * 2];

    // Signal Generator

    for (int n = 0; n < N * L; n++)
    {
        input[n] = 10000 * cos(n * 300 * 2 * PI / 1024);
    }

    for (int n = 0; n < N * L; n++)
    {
        input[n] = 10000 * AWGN_generator();
    }

    for (int n = 0; n < N * L; n++)
    {
        if (sin(n * 100 * 2 * PI / 1024) > 0)
        {
            input[n] = 10000;
        }
        else
        {
            input[n] = -10000;
        }
    }

    // Framing and FFT

    for (int l = 0; l < L / 2; l++)
    {
        for (int i = 0; i < N; i++)
        {
            serial[i] = input[i + 2 * l * N];
            serial[N + i] = input[N + i + 2 * l * N];
        }

        ISR(ping, pong, serial);

        fft(pong, N, dt);

        for (int k = 0; k < 513; k++)
        {
            spectrum[k] += (pow(ccabs(pong[k]), 2) + pow(ccabs(pong[N - 1 - k]), 2)) / 2;
        }
    }

    // Normalization

    for (int k = 0; k < 513; k++)
    {
        spectrum[k] = spectrum[k] / (L * N);
    }

    // Plot the spectrum

    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    fprintf(gnuplot, "plot '-' u 1:2 t 'fs = 16 KHz, n = 300' w lp\n");
    for (i = 0; i < N / 2; ++i)
    {
        fprintf(gnuplot, "%1f %1f \n", (float)i, spectrum[i]);
    }

    fprintf(gnuplot, "e\n");
    fflush(gnuplot);
    getchar();
    pclose(gnuplot);
    exit(EXIT_SUCCESS);
}