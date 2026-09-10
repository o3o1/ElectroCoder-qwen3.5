#include <stdio.h>
#include <math.h>

int main() {
    printf("Testing math library...\n");
    float x = sinf(3.14159f/2.0f);
    printf("sin(pi/2) = %.6f\n", x);
    return 0;
}