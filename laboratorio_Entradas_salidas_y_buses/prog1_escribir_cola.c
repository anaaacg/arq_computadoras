}

void inyectar_tty(void) {
    char texto[100];
    puts("Texto a inyectar:");
    if (!fgets(texto, sizeof(texto), stdin)) return;
    int fd = open("/dev/tty", O_RDWR);
    if (fd < 0) { perror("/dev/tty"); return; }
    int cantidad = 0;
    for (size_t i = 0; texto[i]; i++) {
        if (ioctl(fd, TIOCSTI, &texto[i]) < 0) {
            perror("TIOCSTI bloqueado (habitual en Ubuntu moderno)");
            close(fd);
            return;
        }
        cantidad++;
    }
    close(fd);
    printf("Caracteres inyectados: %d\n", cantidad);
}

int main(void) {
    int opcion;
    char caracter;
    puts("===============================================");
    puts(" PROGRAMA 1: ESCRITOR DE COLA DE TECLADO");
    puts("===============================================");
    puts("[1] Inyectar entrada TTY (TIOCSTI)");
    puts("[2] Simular BIOS INT 16h (servicio 05h)");
    puts("[3] Escribir directamente en RAM BDA simulada");
    printf("Ingrese opcion (1-3): ");
    if (scanf("%d", &opcion) != 1) return 1;
    while ((caracter = getchar()) != '\n' && caracter != EOF) {}
    if (opcion == 1) { inyectar_tty(); return 0; }
    if (opcion != 2 && opcion != 3) { puts("Opcion invalida"); return 1; }
    printf("Ingrese un caracter: ");
    caracter = getchar();
    if (caracter == EOF || caracter == '\n') return 1;
    escribir_bda(caracter, 0x1E, opcion == 2);
    return 0;
}
