#include <stdio.h>
#include <stdint.h>

typedef union {
    unsigned char valor;
    struct {
        unsigned char b0 : 1;
        unsigned char b1 : 1;
        unsigned char b2 : 1;
        unsigned char b3 : 1;
        unsigned char b4 : 1;
        unsigned char b5 : 1;
        unsigned char b6 : 1;
        unsigned char b7 : 1;
    } bits;
} RegBits;

int main()
{
    int8_t a = 120, b = 20;
    int8_t resultado = a + b;

    RegBits reg_a, reg_b, reg_res;
    reg_a.valor = (unsigned char)a;
    reg_b.valor = (unsigned char)b;
    reg_res.valor = (unsigned char)resultado;
    
    printf("%d%d%d%d%d%d%d%d %d%d%d%d%d%d%d%d\n",
           reg_a.bits.b7, reg_a.bits.b6, reg_a.bits.b5, reg_a.bits.b4,
           reg_a.bits.b3, reg_a.bits.b2, reg_a.bits.b1, reg_a.bits.b0,
           reg_b.bits.b7, reg_b.bits.b6, reg_b.bits.b5, reg_b.bits.b4,
           reg_b.bits.b3, reg_b.bits.b2, reg_b.bits.b1, reg_b.bits.b0);
    
    printf("%d%d%d%d%d%d%d%d\n",
           reg_res.bits.b7, reg_res.bits.b6, reg_res.bits.b5, reg_res.bits.b4,
           reg_res.bits.b3, reg_res.bits.b2, reg_res.bits.b1, reg_res.bits.b0);
    

    if ((a > 0 && b > 0 && resultado < 0) || (a < 0 && b < 0 && resultado > 0)) {
        printf("desbordamiento\n");
    }
    
    return 0;
}