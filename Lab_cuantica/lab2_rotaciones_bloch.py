
import math
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator

def simulate_angle(theta: float):
    # 1. Create a circuit with 1 qubit and 1 classical bit
    qc = QuantumCircuit(1, 1)

    # 2. Apply rotation around the Y axis
    qc.ry(theta, 0)

    # 3. Measure the qubit
    qc.measure(0, 0)

    # 4. Simulate 10,000 measurements
    backend = AerSimulator()
    counts = backend.run(
        qc, shots=10000
    ).result().get_counts()

    return counts


# Evaluate four important Bloch sphere positions
angles = [
    (0.0, "North Pole (theta = 0)"),
    (math.pi / 4, "45 degrees (theta = pi/4)"),
    (math.pi / 2, "Equator (theta = pi/2)"),
    (math.pi, "South Pole (theta = pi)")
]

for theta, description in angles:
    counts = simulate_angle(theta)

    p0 = (counts.get('0', 0) / 10000) * 100
    p1 = (counts.get('1', 0) / 10000) * 100

    print(
        f"{description:35s} -> "
        f"P(|0>): {p0:5.1f}% | P(|1>): {p1:5.1f}%"
    )
