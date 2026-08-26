#include <stdio.h>

int main()
{
    int a = 10, b = 20;
    int resultado;

    __asm__ (
        "movl %[valor_a], %%eax\n\t"
        "addl %[valor_b], %%eax\n\t"
        "movl %%eax, %[valor_resultado]"
        : [valor_resultado] "=r" (resultado)
        : [valor_a] "r" (a), [valor_b] "r" (b)
        : "%eax"
    );

    printf("%d + %d = %d\n", a, b, resultado);
    return 0;
}