#include <stdio.h>
#include <stdint.h>

static void print_u32_binary(uint32_t x) {
    for (int i = 31; i >= 0; --i) {
        putchar((x >> i) & 1 ? '1' : '0');
        if (i % 4 == 0 && i != 0) putchar(' ');
    }
    putchar('\n');
}

int main() {
    float a_f = 12345.0;
    int a_i = 12345;
    printf("a_f: %x\n", *(uint32_t*)&a_f);
    printf("a_i: %x\n", a_i); 
    union { float f; uint32_t u; } fu;
    fu.f = a_f;
    printf("a_f bits: ");
    print_u32_binary(fu.u);
    printf("a_i bits: ");
    print_u32_binary((uint32_t)a_i);
    return 0;
}