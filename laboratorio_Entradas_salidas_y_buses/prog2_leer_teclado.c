
#include <stdio.h>
 #include <stdint.h>
 #include <stdbool.h>
 #include <string.h>
 #include <stdlib.h>

 #if defined(__unix__) || defined(__APPLE__)
 #include <unistd.h>
 #endif

 #define BDA_FILE "/tmp/bios_keyboard_bda.bin"

 #pragma pack(push, 1)
 typedef struct {
     uint16_t head;         /* Offset 0x001A: Puntero de lectura */
     uint16_t tail;         /* Offset 0x001C: Puntero de escritura */
     uint16_t buffer[16];   /* Offset 0x001E a 0x003D: 16 palabras */
     uint8_t port_60_data; /* Registro de dato del Puerto 0x60 */
     uint8_t port_64_status;/* Registro de estado del Puerto 0x64 */
 } BiosKeyboardMemory;
 #pragma pack(pop)

 void cargar_bda(BiosKeyboardMemory *bda) {
     FILE *f = fopen(BDA_FILE, "rb");
     if (!f) {
         bda->head = 0x001E;
         bda->tail = 0x001E;
         memset(bda->buffer, 0, sizeof(bda->buffer));
         bda->port_60_data = 0;
         bda->port_64_status = 0;
     } else {
         if (fread(bda, sizeof(BiosKeyboardMemory), 1, f) != 1) {
             bda->head = 0x001E;
             bda->tail = 0x001E;
         }
         fclose(f);
     }
 }

 void guardar_bda(const BiosKeyboardMemory *bda) {




    FILE *f = fopen(BDA_FILE, "wb");
    if (f) {
        fwrite(bda, sizeof(BiosKeyboardMemory), 1, f);
        fclose(f);
    }
}

void lectura_con_scanf(void) {
    char palabra[100];
    printf("\n--- [OPCIÓN 1: LECTURA CON scanf()] ---\n");
    printf(">> Ingrese o espere entrada: ");
    if (scanf("%99s", palabra) == 1) {
        printf("\n======================================================\n");
        printf(" [RESULTADO scanf]: \"%s\"\n", palabra);
        printf("======================================================\n");
    }
}

void lectura_con_getc(void) {
    int c;
    printf("\n--- [OPCIÓN 2: LECTURA CON getc(stdin)] ---\n");
    printf(">> Ingrese o espere un carácter: ");
    c = getc(stdin);
    if (c != EOF) {
        printf("\n======================================================\n");
        printf(" [RESULTADO getc]: Carácter '%c' | Código ASCII: %d (0x%02X)\n", c, c, (uint8_t)c);
        printf("======================================================\n");
    }
}

void lectura_desde_puerto(void) {
    printf("\n--- [OPCIÓN 3: LECTURA DESDE PUERTO DE HARDWARE (0x60)] ---\n");
    printf("Instrucción física x86: in al, 60h\n");
    BiosKeyboardMemory bda;
    cargar_bda(&bda);

    printf(">> Estado del Puerto 0x64 (Status): 0x%02X\n", bda.port_64_status);
    if ((bda.port_64_status & 0x01) == 0 && bda.port_60_data == 0) {
        printf(">> [PUERTO 0x60] No hay datos pendientes en el puerto.\n");
    } else {
        uint8_t scan_code = bda.port_60_data;
        bda.port_64_status &= ~0x01;
        guardar_bda(&bda);
        printf("======================================================\n");
        printf(" [RESULTADO PUERTO 0x60]: Scan-code = 0x%02X\n", scan_code);
        printf("======================================================\n");
    }
}

void lectura_directa_memoria(void) {
    printf("\n--- [OPCIÓN 4: LECTURA DIRECTA DESDE MEMORIA RAM (BDA)] ---\n");
    BiosKeyboardMemory bda;
