#include <stdio.h>

int main() {
    int arreglo[] = {10, 20, 30, 40, 50};
    int cantidad = sizeof(arreglo) / sizeof(arreglo[0]);
    int suma = 0;

    for (int i = 0; i < cantidad; i++) {
        suma += arreglo[i];
    }

    printf("La suma es: %d\n", suma);

    return 0;
}
