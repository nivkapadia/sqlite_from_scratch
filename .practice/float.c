#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(void)
{
    int n = 2;
    double x = 1.0;


    printf("%lu", sizeof(double));
    printf("%lu", sizeof(int));
    printf("%.2f\n", n / x);
}
