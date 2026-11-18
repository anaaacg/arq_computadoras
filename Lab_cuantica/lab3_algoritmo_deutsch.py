
from qiskit import QuantumCircuit
from qiskit_aer import AerSimulator


def build_oracle(function_type: str) -> QuantumCircuit:
    oracle = QuantumCircuit(2, name=f"U_f ({function_type})")

    if function_type == "constant_0":
        pass

    elif function_type == "constant_1":
        oracle.x(1)

    elif function_type == "balanced_identity":
        oracle.cx(0, 1)

    elif function_type == "balanced_inverse":
        oracle.x(0)
        oracle.cx(0, 1)
        oracle.x(0)

    return oracle


def execute_deutsch(function_type: str):
    qc = QuantumCircuit(2, 1)

    # 1. Prepare the ancilla qubit
    qc.x(1)

    # 2. Create superposition
    qc.h(0)
    qc.h(1)

    # 3. Apply the oracle (one quantum query)
    qc.compose(
        build_oracle(function_type),
        inplace=True
    )

    # 4. Apply Hadamard and measure
    qc.h(0)
    qc.measure(0, 0)

    counts = AerSimulator().run(
        qc, shots=1000
    ).result().get_counts()

    return qc, counts


# Test the four possible Boolean functions
cases = [
    "constant_0",
    "constant_1",
    "balanced_identity",
    "balanced_inverse"
]

for case in cases:
    circuit, results = execute_deutsch(case)

    bit = list(results.keys())[0]

    conclusion = (
        "CONSTANT"
        if bit == "0"
        else "BALANCED"
    )

    print(
        f"Oracle: {case:22s} | "
        f"Measurement q0: '{bit}' -> Function {conclusion}"
    )
