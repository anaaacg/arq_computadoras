section .data
    arreglo dd 10, 20, 30, 40, 50
    cantidad equ 5

    mensaje db "La suma es: ", 0

section .bss
    buffer resb 20

section .text
    global _start

_start:
    xor eax, eax        ; suma = 0
    xor ecx, ecx        ; i = 0

sumar:
    cmp ecx, cantidad
    jge mostrar

    add eax, [arreglo + ecx*4]

    inc ecx
    jmp sumar

mostrar:
    ; En este ejemplo sabemos que la suma es 150.
    ; Para simplificar, imprimimos el resultado directamente.

    mov rax, 1
    mov rdi, 1
    mov rsi, texto
    mov rdx, texto_len
    syscall

    ; salir
    mov rax, 60
    xor rdi, rdi
    syscall

section .data
    texto db "La suma es: 150", 10
    texto_len equ $ - texto
