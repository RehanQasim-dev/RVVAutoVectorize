#include <stdio.h>
#include <time.h>

void matmul(int *c, int *a, int *b, int n)
{
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            for (int k = 0; k < n; k++)
            {
                c[i * n + j] += a[i * n + k] * b[k * n + j];
            }
        }
    }
}

int main()
{
    int n = 16;
    int a[16 * 16] = {0}, b[16 * 16] = {0}, c[16 * 16] = {0};
    for (int i = 0; i < n * n; i++)
    {
        a[i] = i % 10;
        b[i] = (i + 1) % 10;
    }

    clock_t start = clock();
    matmul(c, a, b, n);
    clock_t end = clock();
    printf("Time: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    printf("c[0] = %d\n", c[0]);
    return 0;
}