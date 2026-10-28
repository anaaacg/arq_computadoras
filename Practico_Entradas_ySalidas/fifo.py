
RAM = [0] * 256

HEAD = 0x78
TAIL = 0x79
COUNT = 0x7A
BASE = 0x70
CAPACITY = 8

def insert(char):
    if RAM[COUNT] == CAPACITY:
        print(f"OVERFLOW: '{char}' descartado")
        return

    RAM[BASE + RAM[TAIL]] = ord(char)
    RAM[TAIL] = (RAM[TAIL] + 1) % CAPACITY
    RAM[COUNT] += 1
    print(f"INSERT: {char}")

def extract():
    if RAM[COUNT] == 0:
        print("EMPTY: cola vacia")
        return

    char = chr(RAM[BASE + RAM[HEAD]])
    RAM[HEAD] = (RAM[HEAD] + 1) % CAPACITY
    RAM[COUNT] -= 1
    print(f"EXTRACT: {char}")

def status():
    print(f"HEAD={RAM[HEAD]} TAIL={RAM[TAIL]} COUNT={RAM[COUNT]}")

print("=== FIFO KEYBOARD BUFFER (8 BYTES) ===")
for char in "ABCDEFGH":
    insert(char)
status()

insert("I")
extract()
extract()
status()

insert("I")
insert("J")
status()

for _ in range(8):
    extract()
status()
extract()
