#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

int main() {

    const int TAM = 1000;
    const int TAM_SEGUNDO = 100;

    int arreglo[TAM];
    int segundo[TAM_SEGUNDO];

    // --------------------------------------------------
    // 1. Crear archivo con 1000 enteros
    //    Simula la memoria secundaria
    // --------------------------------------------------

    ofstream archivoSalida("datos.bin", ios::binary);

    if (!archivoSalida) {
        cout << "Error al crear el archivo." << endl;
        return 1;
    }

    for (int i = 0; i < TAM; i++) {
        int numero = i;
        archivoSalida.write(
            reinterpret_cast<char*>(&numero),
            sizeof(int)
        );
    }

    archivoSalida.close();

    // --------------------------------------------------
    // 2. Leer los 1000 enteros desde memoria secundaria
    // --------------------------------------------------

    ifstream archivoEntrada("datos.bin", ios::binary);

    if (!archivoEntrada) {
        cout << "Error al abrir el archivo." << endl;
        return 1;
    }

    archivoEntrada.read(
        reinterpret_cast<char*>(arreglo),
        sizeof(arreglo)
    );

    archivoEntrada.close();

    // --------------------------------------------------
    // 3. Mostrar primeros y ultimos 5 elementos
    //    ANTES del traslado
    // --------------------------------------------------

    cout << "Primeros 5 elementos:" << endl;

    for (int i = 0; i < 5; i++) {
        cout << arreglo[i] << " ";
    }

    cout << endl;

    cout << "Ultimos 5 elementos:" << endl;

    for (int i = TAM - 5; i < TAM; i++) {
        cout << arreglo[i] << " ";
    }

    cout << endl;

    // --------------------------------------------------
    // 4. Copiar indices 500 al 599
    // --------------------------------------------------

    memcpy(
        segundo,
        &arreglo[500],
        TAM_SEGUNDO * sizeof(int)
    );

    // --------------------------------------------------
    // 5. Limpiar posiciones 500 al 599
    // --------------------------------------------------

    memset(
        &arreglo[500],
        0,
        TAM_SEGUNDO * sizeof(int)
    );

    // --------------------------------------------------
    // 6. Mostrar los 100 elementos transferidos
    // --------------------------------------------------

    cout << "\nElementos transferidos:" << endl;

    for (int i = 0; i < TAM_SEGUNDO; i++) {
        cout << segundo[i] << " ";

        if ((i + 1) % 10 == 0) {
            cout << endl;
        }
    }

    // --------------------------------------------------
    // 7. Verificar posiciones 500 al 599
    // --------------------------------------------------

    bool correcto = true;

    for (int i = 500; i <= 599; i++) {

        if (arreglo[i] != 0) {
            correcto = false;
            break;
        }
    }

    cout << "\nValidacion:" << endl;

    if (correcto) {
        cout << "Los indices 500 al 599 contienen cero."
             << endl;
    }
    else {
        cout << "Error: existen valores diferentes de cero."
             << endl;
    }

    return 0;
}
