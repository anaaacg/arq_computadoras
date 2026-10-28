
# SUBTASK 3: VRAM DISPLAY AND INTERACTIVE TERMINAL
# Simulacion de teclado, cola FIFO y VRAM en Ubuntu

RAM = [0] * 256

PORT_STATUS = 0x01
PORT_DATA = 0x02

FIFO_BASE = 0x70
HEAD = 0x78
TAIL = 0x79
COUNT = 0x7A

VRAM_BASE = 0xF0
VRAM_SIZE = 16

ports = {PORT_STATUS: 0, PORT_DATA: 0}
cursor = 0


# Keyboard ports
def press_key(char):
    ports[PORT_DATA] = ord(char)
    ports[PORT_STATUS] = 1


def read_port(port):
    value = ports[port]
    if port == PORT_DATA:
        ports[PORT_STATUS] = 0
    return value


# Circular FIFO queue
def fifo_insert(value):
    if RAM[COUNT] == 8:
        print("FIFO OVERFLOW")
        return False

    RAM[FIFO_BASE + RAM[TAIL]] = value
    RAM[TAIL] = (RAM[TAIL] + 1) % 8
    RAM[COUNT] += 1
    return True


def fifo_extract():
    if RAM[COUNT] == 0:
        return None

    value = RAM[FIFO_BASE + RAM[HEAD]]
    RAM[HEAD] = (RAM[HEAD] + 1) % 8
    RAM[COUNT] -= 1
    return value


# Video memory
def vram_address(row, col):
    return VRAM_BASE + row * 4 + col


def vram_write(row, col, value):
    RAM[vram_address(row, col)] = value


def vram_read(row, col):
    return RAM[vram_address(row, col)]


def clear_screen():
    for i in range(VRAM_SIZE):
        RAM[VRAM_BASE + i] = 0x20


def scroll():
    for row in range(3):
        for col in range(4):
            value = vram_read(row + 1, col)
            vram_write(row, col, value)

    for col in range(4):
        vram_write(3, col, 0x20)


def process_character(value):
    global cursor

    # Backspace: ASCII 0x08
    if value == 0x08:
        if cursor > 0:
            cursor -= 1
            RAM[VRAM_BASE + cursor] = 0x20
        return

    if cursor >= VRAM_SIZE:
        scroll()
        cursor = 12

    RAM[VRAM_BASE + cursor] = value
    cursor += 1


def show_screen():
    print("\n=== VRAM DISPLAY 4x4 ===")

    for row in range(4):
        line = ""
        for col in range(4):
            value = vram_read(row, col)
            line += chr(value) + " "

        print("| " + line + "|")

    print(
        f"HEAD={RAM[HEAD]} "
        f"TAIL={RAM[TAIL]} "
        f"COUNT={RAM[COUNT]}"
    )
    print(f"CURSOR=0x{VRAM_BASE + cursor:02X}")


def main():
    clear_screen()

    print("=== INTERACTIVE VRAM TERMINAL ===")
    print("Keyboard ports: 0x01, 0x02")
    print("FIFO buffer: 0x70 - 0x77")
    print("VRAM: 0xF0 - 0xFF")
    print("Commands:")
    print("  /back = Backspace")
    print("  /quit = Exit")

    while True:
        text = input("\nInput: ")

        if text == "/quit":
            break

        if text == "/back":
            text = "\b"

        for char in text:
            press_key(char)

            if read_port(PORT_STATUS) == 1:
                value = read_port(PORT_DATA)
                fifo_insert(value)

            # Main CPU consumer cycle
            while RAM[COUNT] > 0:
                value = fifo_extract()
                process_character(value)

        show_screen()

    print("\nProgram finished.")


if __name__ == "__main__":
    main()
