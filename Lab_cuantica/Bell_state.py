
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

# 1. Create a circuit with 2 qubits and 2 classical bits
qc = QuantumCircuit(2, 2)

# 2. Create superposition
qc.h(0)

# 3. Create quantum entanglement
qc.cx(0, 1)

# 4. Measure both qubits
qc.measure([0, 1], [0, 1])

# 5. Display the circuit
print("QUANTUM CIRCUIT:")
print(qc.draw(output="text"))

# 6. Initialize the simulator
backend = AerSimulator()
shots = 10000

job = backend.run(qc, shots=shots)
counts = job.result().get_counts(qc)

# 7. Display measurement results
print(f"\nMEASUREMENT RESULTS ({shots} shots):")

for bitstring, count in sorted(counts.items()):
    percentage = (count / shots) * 100
    print(
        f"State |{bitstring}>: "
        f"{count:5d} occurrences ({percentage:6.2f}%)"
    )
