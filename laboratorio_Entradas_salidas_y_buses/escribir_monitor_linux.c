#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#if defined(__linux__)
#include <sys/ioctl.h>
#include <linux/fb.h>
#endif

void opcion_linux_framebuffer(void) {
    printf("\n====================================================================\n");
    printf(" OPCIÓN 1: LINUX DIRECT FRAMEBUFFER (/dev/fb0 con mmap)              \n");
    printf("====================================================================\n");
#if defined(__linux__)
    int fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror(">> Error al abrir /dev/fb0 (Ejecuta con sudo o en consola TTY)");
        return;
    }
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo);
    ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo);

    long screensize = vinfo.yres_virtual * finfo.line_length;
    uint8_t *vram = (uint8_t *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (vram == MAP_FAILED) { perror(">> Error mmap"); close(fbfd); return; }

    int start_x = vinfo.xres / 2 - 50;
    int start_y = vinfo.yres / 2 - 50;
    int bpp = vinfo.bits_per_pixel / 8;

    for (int y = start_y; y < start_y + 100; y++) {
        for (int x = start_x; x < start_x + 100; x++) {
            long loc = (x + vinfo.xoffset) * bpp + (y + vinfo.yoffset) * finfo.line_length;
            if (bpp == 4) { vram[loc] = 255; vram[loc+1] = 255; vram[loc+2] = 255; vram[loc+3] = 0; }
        }
    }
    printf(">> Cuadrado blanco dibujado en VRAM física.\n");
    munmap(vram, screensize); close(fbfd);
#else



   printf(">> Funcionalidad exclusiva del kernel Linux (/dev/fb0).\n");
#endif
}

void opcion_linux_vcsa(void) {
   printf("\n====================================================================\n");
   printf(" OPCIÓN 2: MEMORIA DE VIDEO DE CONSOLA VIRTUAL (/dev/vcsa)           \n");
   printf("====================================================================\n");
#if defined(__linux__)
   int fd = open("/dev/vcsa", O_RDWR);
   if (fd == -1) { perror(">> Error al abrir /dev/vcsa (Requiere sudo)"); return; }
   lseek(fd, 4, SEEK_SET);
   const char *mensaje = "HOLA DESDE MEMORIA /dev/vcsa";
   for (int i = 0; mensaje[i] != '\0'; i++) {
       uint8_t celda[2] = {(uint8_t)mensaje[i], 0x1E};
       write(fd, celda, 2);
   }
   printf(">> Texto escrito en la memoria de la consola virtual.\n");
   close(fd);
#else
   printf(">> Funcionalidad exclusiva de consolas TTY en Linux (/dev/vcsa).\n");
#endif
}

void opcion_linux_dev_mem(void) {
   printf("\n====================================================================\n");
   printf(" OPCIÓN 3: MEMORIA FÍSICA BRUTA (/dev/mem con mmap y ASM)            \n");
   printf("====================================================================\n");
   printf("Código en C + Ensamblador x86 embebido:\n");
   printf(" int mem_fd = open(\"/dev/mem\", O_RDWR | O_SYNC);\n");
   printf(" uint8_t *vram = (uint8_t *)mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0xB8000);\n");
   printf(" char caracter = 'X';\n");
   printf(" uint8_t color = 0x4F;\n");
   printf(" __asm__ __volatile__ (\n");
   printf("      \"movb %%0, (%%1)    \\n\\t\"\n");
   printf("      \"movb %%2, 1(%%1)   \\n\\t\"\n");
   printf("      : : \"r\"(caracter), \"r\"(vram), \"r\"(color) : \"memory\"\n");
   printf(" );\n");
}

#define SHM_FILAS 10
#define SHM_COLS 40

typedef struct {
   uint8_t ascii;
   uint8_t attribute;
} __attribute__((packed)) CeldaVram;
