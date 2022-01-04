#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Complex.h"

#define NUM_SECTIONS 3
#define N 600000

void read_file(double *arr1, double *arr2)
{
    int i, j;
    FILE *f;
    char c;
    for (j = 0; j < 2; j++)
    {
        i = 0;
        if (j == 0)
        {
            f = fopen("sos_matrix.txt", "r");
            while (i < 18)
            {
                fscanf(f, "%lf %c", &arr1[i], &c);
                i += 1;
            }
            fclose(f);
        }
        else if (j == 1)
        {
            f = fopen("gain.txt", "r");
            while (!feof(f))
            {
                fscanf(f, "%lf %c", &arr2[i], &c);
                i += 1;
            }
            fclose(f);
        }
    }
}

void IIR_Filter(double b[3][3], double a[3][2], double scale[4], double x[N], double *y)
{
    int section;
    double w[NUM_SECTIONS][2] = {0}, wn, yn, input;
    for (int i = 0; i < N; i++)
    {
        input = x[i];
        for (section = 0; section < NUM_SECTIONS; section++)
        {
            wn = input - a[section][0] * w[section][0] - a[section][1] * w[section][1];
            yn = (b[section][0] * wn + b[section][1] * w[section][0] + b[section][2] * w[section][1]) * scale[section];
            w[section][1] = w[section][0];
            w[section][0] = wn;
            input = yn;
        }
        y[i] = yn;
    }
    return;
}

double normal(void)
{
    double x = (double)random() / RAND_MAX, y = (double)random() / RAND_MAX, z = sqrt(-2 * log(x)) * cos(2 * M_PI * y);
    return z;
}

void white_noise_gen(double mu, double var, double *white_noise)
{
    int i;
    for (i = 0; i < N; i++)
    {
        white_noise[i] = sqrt(var) * normal() + mu;
    }
}

void my_fft(complex *v, int n, complex *tmp)
{
    if (n > 1)
    {
        int k, m;
        complex z, w, *vo, *ve;
        ve = tmp;
        vo = tmp + n / 2;
        for (k = 0; k < n / 2; k++)
        {
            ve[k] = v[2 * k];
            vo[k] = v[2 * k + 1];
        }
        my_fft(ve, n / 2, v);
        my_fft(vo, n / 2, v);
        for (m = 0; m < n / 2; m++)
        {
            w.x = cos(2 * M_PI * m / (double)n);
            w.y = -sin(2 * M_PI * m / (double)n);
            z.x = w.x * vo[m].x - w.y * vo[m].y;
            z.y = w.x * vo[m].y + w.y * vo[m].x;
            v[m].x = ve[m].x + z.x;
            v[m].y = ve[m].y + z.y;
            v[m + n / 2].x = ve[m].x - z.x;
            v[m + n / 2].y = ve[m].y - z.y;
        }
    }
    return;
}

void plot_spectrum(double *y, float fs)
{
    int i, j;
    complex *Yf = (complex *)malloc(sizeof(complex) * N);
    complex *tmp = (complex *)malloc(sizeof(complex) * N);
    double *Yf_s = (double *)malloc(sizeof(double) * N);
    double *f = (double *)malloc(sizeof(double) * N);

    for (i = 0; i < N; i++)
        f[i] = (double)(i) / (double)(N)*fs - (fs / 2);

    for (i = 0; i < N; i++)
    {
        Yf[i].x = y[i];
        Yf[i].y = 0;
    }

    float dt = 1. / fs;

    // fftr(Yf, N, dt);
    my_fft(Yf, N, tmp);

    for (i = 0; i < N; i++)
        Yf_s[i] = sqrt(Yf[i].x * Yf[i].x + Yf[i].y * Yf[i].y);

    FILE *gnuplot = popen("gnuplot", "w");
    if (!gnuplot)
    {
        perror("popen");
        exit(EXIT_FAILURE);
    }
    fprintf(gnuplot, "plot '-' u 1:2 t 'N(4, 1)' w lp\n");
    for (i = N / 2; i < N; ++i)
    {
        fprintf(gnuplot, "%1f %.17f \n", f[i - N / 2], Yf_s[i]);
    }
    for (i = 0; i < N / 2; ++i)
    {
        fprintf(gnuplot, "%1f %.17f \n", f[i + N / 2], Yf_s[i]);
    }
    fprintf(gnuplot, "e\n");
    fprintf(stdout, "Click Ctrl+d to quit...\n");
    fflush(gnuplot);
    getchar();
    pclose(gnuplot);
    exit(EXIT_SUCCESS);

    free(Yf);
    free(Yf_s);
}

int main()
{
    int i, j, k;
    double input, SOS[18], scale[4], b[3][3], a[3][2];
    double *x = (double *)malloc(sizeof(double) * N);
    double *y = (double *)malloc(sizeof(double) * N);
    float fs = 8000;

    read_file(SOS, scale);
    for (i = 0; i < 18; i++)
    {
        j = i % 6;
        k = i / 6;
        if (j < 3)
            b[k][j] = SOS[i];
        if (j > 3)
            a[k][j - 4] = SOS[i];
    }

    white_noise_gen(4., 1., x);
    IIR_Filter(b, a, scale, x, y);
    plot_spectrum(y, fs);

    free(x);
    free(y);
    return 0;
}