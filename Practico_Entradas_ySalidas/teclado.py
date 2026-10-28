
# Experimento 1: Lectura de teclado por puertos
# Simulacion de E/S aislada en Ubuntu

puertos = {
    0x01: 0,  # Estado del teclado
    0x02: 0   # Dato ASCII
}

def presionar_tecla(tecla):
    puertos[0x02] = ord(tecla)
    puertos[0x01] = 1

def leer_puerto(direccion):
    valor = puertos[direccion]

    if direccion == 0x02:
        puertos[0x01] = 0

    return valor

print("=== KEYBOARD PORT INPUT ===")

while True:
    tecla = input("Presiona una tecla (salir: q): ")

    if tecla == "q":
        break

    if not tecla:
        continue

    presionar_tecla(tecla[0])

    if leer_puerto(0x01) == 1:
        ascii_valor = leer_puerto(0x02)
        print(f"ASCII capturado: {ascii_valor}")
        print(f"Estado del puerto: {leer_puerto(0x01)}")
