
import math
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

# Create a circuit with 3 qubits and 3 classical bits
qc = QuantumCircuit(3, 3)

# 1. Prepare the initial quantum state on q0
qc.ry(math.pi / 3, 0)
qc.barrier()

# 2. Create an entangled Bell pair between Alice and Bob
qc.h(1)
qc.cx(1, 2)
qc.barrier()

# 3. Alice's operations
qc.cx(0, 1)
qc.h(0)
qc.barrier()

# 4. Alice measures her qubits
qc.measure(0, 0)
qc.measure(1, 1)

# 5. Bob's correction operations
qc.barrier()
qc.cx(1, 2)
qc.cz(0, 2)

# 6. Bob measures the final qubit
qc.barrier()
qc.measure(2, 2)

# 7. Execute the simulation
counts = AerSimulator().run(
    qc, shots=10000
).result().get_counts()

# 8. Calculate Bob's measurement results
bob = {'0': 0, '1': 0}

for bitstring, number in counts.items():
    bob[bitstring[0]] += number

# 9. Display verification results
print("QUANTUM TELEPORTATION VERIFICATION (BOB - q2):")

print(
    f"P(|0>): {bob['0']/100:.2f}% "
    "(Theoretical: 75.00%)"
)

print(
    f"P(|1>): {bob['1']/100:.2f}% "
    "(Theoretical: 25.00%)"
)
