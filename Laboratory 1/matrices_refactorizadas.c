#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define N 1024
#define TOTAL ((size_t)N * N)

/* =========================================================
   TIEMPO
   ========================================================= */

static double medir_tiempo(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (double)ts.tv_sec +
           (double)ts.tv_nsec / 1e9;
}


/* =========================================================
   INICIALIZACION
   ========================================================= */

static void inicializar_matrices(float *A, float *B)
{
    for (int i = 0; i < N; ++i) {

        for (int j = 0; j < N; ++j) {

            size_t pos = (size_t)i * N + j;

            A[pos] =
                (float)((i + j) % 50) * 0.02f + 1.0f;

            B[pos] =
                (float)((i * 2 + j) % 50) * 0.02f + 0.5f;
        }
    }
}


/* =========================================================
   1. ALGORITMO NAIVE
   i -> j -> k

   B se recorre por columnas.
   Mala localidad espacial.
   ========================================================= */

static void algoritmo_naive(
    const float * restrict A,
    const float * restrict B,
    float * restrict C)
{
    for (int i = 0; i < N; ++i) {

        for (int j = 0; j < N; ++j) {

            float suma = 0.0f;

            for (int k = 0; k < N; ++k) {

                suma +=
                    A[(size_t)i * N + k] *
                    B[(size_t)k * N + j];
            }

            C[(size_t)i * N + j] = suma;
        }
    }
}


/* =========================================================
   2. LOCALIDAD ESPACIAL
   i -> k -> j

   B y C se recorren secuencialmente.
   ========================================================= */

static void algoritmo_localidad(
    const float * restrict A,
    const float * restrict B,
    float * restrict C)
{
    memset(C, 0, TOTAL * sizeof(float));

    for (int i = 0; i < N; ++i) {

        float *fila_c = C + (size_t)i * N;
        const float *fila_a = A + (size_t)i * N;

        for (int k = 0; k < N; ++k) {

            const float a = fila_a[k];
            const float *fila_b = B + (size_t)k * N;

            for (int j = 0; j < N; ++j) {

                fila_c[j] +=
                    a * fila_b[j];
            }
        }
    }
}


/* =========================================================
   3. PUNTEROS + VARIABLES LOCALES

   El compilador puede mantener "a" en registro.
   No necesitamos usar register.
   ========================================================= */

static void algoritmo_registros(
    const float * restrict A,
    const float * restrict B,
    float * restrict C)
{
    memset(C, 0, TOTAL * sizeof(float));

    for (int i = 0; i < N; ++i) {

        const float *fila_a =
            A + (size_t)i * N;

        float *fila_c =
            C + (size_t)i * N;

        for (int k = 0; k < N; ++k) {

            const float a = fila_a[k];

            const float *fila_b =
                B + (size_t)k * N;

            for (int j = 0; j < N; ++j) {

                fila_c[j] +=
                    a * fila_b[j];
            }
        }
    }
}


/* =========================================================
   4. LOOP UNROLLING 4X

   Cuatro operaciones por iteracion.
   ========================================================= */

static void algoritmo_unrolling(
    const float * restrict A,
    const float * restrict B,
    float * restrict C)
{
    memset(C, 0, TOTAL * sizeof(float));

    for (int i = 0; i < N; ++i) {

        const float *fila_a =
            A + (size_t)i * N;

        float *fila_c =
            C + (size_t)i * N;

        for (int k = 0; k < N; ++k) {

            const float a =
                fila_a[k];

            const float *fila_b =
                B + (size_t)k * N;

            int j = 0;

            for (; j + 3 < N; j += 4) {

                fila_c[j]     += a * fila_b[j];
                fila_c[j + 1] += a * fila_b[j + 1];
                fila_c[j + 2] += a * fila_b[j + 2];
                fila_c[j + 3] += a * fila_b[j + 3];
            }

            /*
             * Permite que funcione aunque N
             * no sea multiplo de 4.
             */
            for (; j < N; ++j) {

                fila_c[j] +=
                    a * fila_b[j];
            }
        }
    }
}


/* =========================================================
   CHECKSUM
   ========================================================= */

static double calcular_checksum(const float *matriz)
{
    double suma = 0.0;

    for (size_t i = 0; i < TOTAL; ++i)
        suma += matriz[i];

    return suma;
}


/* =========================================================
   BENCHMARK
   ========================================================= */

typedef void (*Algoritmo)(
    const float *,
    const float *,
    float *
);

static double ejecutar_benchmark(
    Algoritmo algoritmo,
    const float *A,
    const float *B,
    float *C)
{
    double inicio = medir_tiempo();

    algoritmo(A, B, C);

    return medir_tiempo() - inicio;
}


static void mostrar_resultado(
    const char *nombre,
    double tiempo,
    double tiempo_naive,
    double operaciones_gflop)
{
    printf(
        "%-30s : %7.4f s | %7.2f GFLOPS | Speedup: %6.2fx\n",
        nombre,
        tiempo,
        operaciones_gflop / tiempo,
        tiempo_naive / tiempo
    );
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    const size_t bytes =
        TOTAL * sizeof(float);

    /*
     * Multiplicacion de matrices:
     *
     * N^3 multiplicaciones
     * N^3 sumas
     *
     * aproximadamente 2*N^3 FLOPs
     */
    const double gflop =
        2.0 *
        (double)N *
        (double)N *
        (double)N /
        1e9;


    float *A  = malloc(bytes);
    float *B  = malloc(bytes);

    float *C1 = malloc(bytes);
    float *C2 = malloc(bytes);
    float *C3 = malloc(bytes);
    float *C4 = malloc(bytes);


    if (!A || !B || !C1 || !C2 || !C3 || !C4) {

        fprintf(
            stderr,
            "Error: no se pudo reservar memoria.\n"
        );

        free(A);
        free(B);
        free(C1);
        free(C2);
        free(C3);
        free(C4);

        return EXIT_FAILURE;
    }


    inicializar_matrices(A, B);


    double t_naive =
        ejecutar_benchmark(
            algoritmo_naive,
            A,
            B,
            C1
        );


    double t_localidad =
        ejecutar_benchmark(
            algoritmo_localidad,
            A,
            B,
            C2
        );


    double t_registros =
        ejecutar_benchmark(
            algoritmo_registros,
            A,
            B,
            C3
        );


    double t_unroll =
        ejecutar_benchmark(
            algoritmo_unrolling,
            A,
            B,
            C4
        );


    printf(
        "\n=== RESULTADOS N = %d ===\n\n",
        N
    );


    mostrar_resultado(
        "1. Naive i-j-k",
        t_naive,
        t_naive,
        gflop
    );


    mostrar_resultado(
        "2. Localidad espacial",
        t_localidad,
        t_naive,
        gflop
    );


    mostrar_resultado(
        "3. Variables / registros",
        t_registros,
        t_naive,
        gflop
    );


    mostrar_resultado(
        "4. Loop unrolling 4x",
        t_unroll,
        t_naive,
        gflop
    );


    double chk1 = calcular_checksum(C1);
    double chk2 = calcular_checksum(C2);
    double chk3 = calcular_checksum(C3);
    double chk4 = calcular_checksum(C4);


    printf("\n=== VALIDACION ===\n");

    printf(
        "Naive     : %.8e\n",
        chk1
    );

    printf(
        "Localidad : %.8e | Error: %.8e\n",
        chk2,
        fabs(chk1 - chk2)
    );

    printf(
        "Registros : %.8e | Error: %.8e\n",
        chk3,
        fabs(chk1 - chk3)
    );

    printf(
        "Unrolling : %.8e | Error: %.8e\n",
        chk4,
        fabs(chk1 - chk4)
    );


    free(A);
    free(B);

    free(C1);
    free(C2);
    free(C3);
    free(C4);

    return EXIT_SUCCESS;
}
