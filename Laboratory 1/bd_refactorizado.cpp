#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>

using namespace std;

/* =========================================================
   FECHA
   ========================================================= */

class fecha {
private:
    int year;
    int month;
    int day;

public:
    fecha() {
        year = 0;
        month = 0;
        day = 0;
    }

    fecha(int y, int m, int d) {
        year = y;
        month = m;
        day = d;
    }

    int getYear() {
        return year;
    }

    int getMonth() {
        return month;
    }

    int getDay() {
        return day;
    }

    void setYear(int y) {
        year = y;
    }

    void setMonth(int m) {
        month = m;
    }

    void setDay(int d) {
        day = d;
    }

    bool fechasIguales(fecha other) {
        return (
            year == other.year &&
            month == other.month &&
            day == other.day
        );
    }

    int compararFechas(fecha other) {

        if (fechasIguales(other))
            return 0;

        if (year > other.year)
            return 1;

        if (year < other.year)
            return -1;

        if (month > other.month)
            return 1;

        if (month < other.month)
            return -1;

        if (day > other.day)
            return 1;

        if (day < other.day)
            return -1;

        return 0;
    }

    void formatoFecha(char* s, int size) {

        char ystr[6];
        char mstr[3];
        char dstr[3];

        int y = year;
        int i = 0;

        if (y < 0) {
            y = -y;
            ystr[i] = '-';
            i++;
        }

        ystr[i]     = '0' + ((y / 1000) % 10);
        ystr[i + 1] = '0' + ((y / 100) % 10);
        ystr[i + 2] = '0' + ((y / 10) % 10);
        ystr[i + 3] = '0' + (y % 10);
        ystr[i + 4] = '\0';

        int m = month;

        mstr[0] = '0' + ((m / 10) % 10);
        mstr[1] = '0' + (m % 10);
        mstr[2] = '\0';

        int d = day;

        dstr[0] = '0' + ((d / 10) % 10);
        dstr[1] = '0' + (d % 10);
        dstr[2] = '\0';

        strcpy(s, ystr);
        strcat(s, "-");
        strcat(s, mstr);
        strcat(s, "-");
        strcat(s, dstr);
    }
};


/* =========================================================
   EVENTO
   ========================================================= */

class evento {
private:
    fecha date;
    char* nombre;

public:
    evento() {
        date = fecha(0, 0, 0);
        nombre = nullptr;
    }

    evento(fecha f, const char* nom) {
        date = f;

        nombre = new char[strlen(nom) + 1];
        strcpy(nombre, nom);
    }

    evento(const evento& other) {
        date = other.date;

        if (other.nombre == nullptr) {
            nombre = nullptr;
        } else {
            nombre = new char[strlen(other.nombre) + 1];
            strcpy(nombre, other.nombre);
        }
    }

    evento& operator=(const evento& other) {

        if (this != &other) {

            delete[] nombre;

            date = other.date;

            if (other.nombre == nullptr) {
                nombre = nullptr;
            } else {
                nombre = new char[strlen(other.nombre) + 1];
                strcpy(nombre, other.nombre);
            }
        }

        return *this;
    }

    ~evento() {
        delete[] nombre;
    }

    fecha getFecha() {
        return date;
    }

    char* getNombre() {
        return nombre;
    }

    void setFecha(fecha f) {
        date = f;
    }

    void setNombre(const char* n) {

        delete[] nombre;

        if (n == nullptr) {
            nombre = nullptr;
            return;
        }

        nombre = new char[strlen(n) + 1];
        strcpy(nombre, n);
    }

    bool mismaFechaYNombre(fecha f, const char* nom) {

        return (
            date.fechasIguales(f) &&
            strcmp(nombre, nom) == 0
        );
    }

    bool mismaFecha(fecha f) {
        return date.fechasIguales(f);
    }

    bool contieneSubcadena(const char* patron) {
        return strstr(nombre, patron) != nullptr;
    }
};


/* =========================================================
   DECLARACIONES
   ========================================================= */

bool convertirAFecha(char* texto, fecha& f);

void separarLinea(
    char* linea,
    char** palabras,
    int maxPalabras,
    int& cantPalabras
);


/* =========================================================
   GESTOR / BASE DE DATOS
   ========================================================= */

class dataBase {

private:
    evento* eventos;
    int cantidad;
    int capacidad;


    /* Búsqueda binaria O(log n) */
    int encontrarIndice(fecha f, const char* nom) {

        int izquierda = 0;
        int derecha = cantidad - 1;

        while (izquierda <= derecha) {

            int medio =
                izquierda +
                (derecha - izquierda) / 2;

            int cmpFecha =
                eventos[medio]
                .getFecha()
                .compararFechas(f);

            if (cmpFecha == 0) {

                int cmpNombre =
                    strcmp(
                        eventos[medio].getNombre(),
                        nom
                    );

                if (cmpNombre == 0)
                    return medio;

                if (cmpNombre < 0)
                    izquierda = medio + 1;
                else
                    derecha = medio - 1;
            }
            else if (cmpFecha < 0) {
                izquierda = medio + 1;
            }
            else {
                derecha = medio - 1;
            }
        }

        return -1;
    }


    /* Capacidad exponencial: 1, 2, 4, 8, 16... */
    void asegurarCapacidad() {

        if (cantidad < capacidad)
            return;

        int nuevaCapacidad =
            capacidad * 2;

        evento* nuevo =
            new evento[nuevaCapacidad];

        for (int i = 0; i < cantidad; i++) {
            nuevo[i] = eventos[i];
        }

        delete[] eventos;

        eventos = nuevo;
        capacidad = nuevaCapacidad;
    }


    dataBase(const dataBase& other);
    dataBase& operator=(const dataBase& other);


public:

    dataBase() {

        capacidad = 1;
        cantidad = 0;

        eventos =
            new evento[capacidad];
    }


    ~dataBase() {
        delete[] eventos;
    }


    /* =====================================================
       AGREGAR

       Busca posición con búsqueda binaria.
       Inserta directamente en orden.
       Ya no ejecuta Bubble Sort completo.
       ===================================================== */

    void agregarEvento(fecha f, char* nom) {

        int izquierda = 0;
        int derecha = cantidad;

        while (izquierda < derecha) {

            int medio =
                izquierda +
                (derecha - izquierda) / 2;

            int cmpFecha =
                eventos[medio]
                .getFecha()
                .compararFechas(f);

            int comparacion;

            if (cmpFecha != 0) {
                comparacion = cmpFecha;
            }
            else {
                comparacion =
                    strcmp(
                        eventos[medio].getNombre(),
                        nom
                    );
            }

            if (comparacion < 0)
                izquierda = medio + 1;
            else
                derecha = medio;
        }

        int posicion = izquierda;


        /* Verificar duplicado */
        if (
            posicion < cantidad &&
            eventos[posicion]
                .getFecha()
                .fechasIguales(f) &&
            strcmp(
                eventos[posicion].getNombre(),
                nom
            ) == 0
        ) {

            cout
                << "Duplicate event"
                << endl;

            return;
        }


        asegurarCapacidad();


        /* Abrir espacio solamente donde corresponde */
        for (
            int i = cantidad;
            i > posicion;
            i--
        ) {

            eventos[i] =
                eventos[i - 1];
        }


        eventos[posicion].setFecha(f);
        eventos[posicion].setNombre(nom);

        cantidad++;
    }


    /* =====================================================
       ELIMINAR UNO
       ===================================================== */

    bool eliminarEvento(
        fecha f,
        const char* nom
    ) {

        int indice =
            encontrarIndice(
                f,
                nom
            );

        if (indice == -1)
            return false;


        for (
            int i = indice;
            i < cantidad - 1;
            i++
        ) {

            eventos[i] =
                eventos[i + 1];
        }

        cantidad--;

        return true;
    }


    /* =====================================================
       ELIMINAR POR FECHA

       Compactación en una sola pasada.
       ===================================================== */

    int eliminarEventosPorFecha(fecha f) {

        int escritura = 0;
        int eliminados = 0;


        for (
            int lectura = 0;
            lectura < cantidad;
            lectura++
        ) {

            if (
                eventos[lectura]
                .getFecha()
                .fechasIguales(f)
            ) {

                eliminados++;

            }
            else {

                if (escritura != lectura) {

                    eventos[escritura] =
                        eventos[lectura];
                }

                escritura++;
            }
        }


        cantidad = escritura;

        return eliminados;
    }


    /* =====================================================
       ELIMINAR POR NOMBRE
       ===================================================== */

    int eliminarEventosPorNombre(char* nombre) {

        int escritura = 0;
        int eliminados = 0;


        for (
            int lectura = 0;
            lectura < cantidad;
            lectura++
        ) {

            if (
                strcmp(
                    eventos[lectura].getNombre(),
                    nombre
                ) == 0
            ) {

                eliminados++;

            }
            else {

                if (escritura != lectura) {

                    eventos[escritura] =
                        eventos[lectura];
                }

                escritura++;
            }
        }


        cantidad = escritura;

        return eliminados;
    }


    /* =====================================================
       EDITAR
       ===================================================== */

    bool editarEvento(
        fecha f,
        char* nombre,
        fecha nuevaFecha,
        char* nuevoNombre
    ) {

        int indice =
            encontrarIndice(
                f,
                nombre
            );

        if (indice == -1)
            return false;


        int duplicado =
            encontrarIndice(
                nuevaFecha,
                nuevoNombre
            );


        if (
            duplicado != -1 &&
            duplicado != indice
        ) {

            return false;
        }


        /* Eliminar el evento viejo */
        for (
            int i = indice;
            i < cantidad - 1;
            i++
        ) {

            eventos[i] =
                eventos[i + 1];
        }

        cantidad--;


        /* Reinsertar ordenadamente */
        agregarEvento(
            nuevaFecha,
            nuevoNombre
        );

        return true;
    }


    /* =====================================================
       FIND POR FECHA
       ===================================================== */

    void findPorFecha(fecha f) {

        bool encontrado = false;
        char sfecha[20];


        for (
            int i = 0;
            i < cantidad;
            i++
        ) {

            if (
                eventos[i]
                .getFecha()
                .fechasIguales(f)
            ) {

                eventos[i]
                    .getFecha()
                    .formatoFecha(
                        sfecha,
                        20
                    );

                cout
                    << sfecha
                    << " "
                    << eventos[i].getNombre()
                    << endl;

                encontrado = true;
            }
        }


        if (!encontrado) {

            cout
                << "Event not found"
                << endl;
        }
    }


    /* =====================================================
       FIND POR PALABRA
       ===================================================== */

    void findPorPalabra(char* palabra) {

        bool encontrado = false;
        char sfecha[20];


        for (
            int i = 0;
            i < cantidad;
            i++
        ) {

            if (
                strstr(
                    eventos[i].getNombre(),
                    palabra
                ) != nullptr
            ) {

                eventos[i]
                    .getFecha()
                    .formatoFecha(
                        sfecha,
                        20
                    );

                cout
                    << sfecha
                    << " "
                    << eventos[i].getNombre()
                    << endl;

                encontrado = true;
            }
        }


        if (!encontrado) {

            cout
                << "Event not found"
                << endl;
        }
    }


    /* =====================================================
       PRINT
       ===================================================== */

    void print() {

        char sfecha[20];


        for (
            int i = 0;
            i < cantidad;
            i++
        ) {

            eventos[i]
                .getFecha()
                .formatoFecha(
                    sfecha,
                    20
                );

            cout
                << sfecha
                << " "
                << eventos[i].getNombre()
                << endl;
        }
    }


    /* =====================================================
       ARCHIVO
       ===================================================== */

    void guardarEnArchivo(
        const char* nombreArchivo
    ) {

        ofstream out(nombreArchivo);

        if (!out)
            return;


        char sfecha[20];


        for (
            int i = 0;
            i < cantidad;
            i++
        ) {

            eventos[i]
                .getFecha()
                .formatoFecha(
                    sfecha,
                    20
                );

            out
                << sfecha
                << " "
                << eventos[i].getNombre()
                << '\n';
        }
    }


    void cargarDesdeArchivo(
        const char* nombreArchivo
    ) {

        ifstream in(nombreArchivo);

        if (!in)
            return;


        char linea[300];
        char* palabras[10];

        int cant = 0;

        fecha f;


        while (
            in.getline(
                linea,
                300
            )
        ) {

            separarLinea(
                linea,
                palabras,
                10,
                cant
            );


            if (cant >= 2) {

                if (
                    convertirAFecha(
                        palabras[0],
                        f
                    )
                ) {

                    agregarEvento(
                        f,
                        palabras[1]
                    );
                }
            }
        }
    }
};


/* =========================================================
   COMMAND
   ========================================================= */

class comando {

protected:
    dataBase* bd;

public:
    comando() {
        bd = nullptr;
    }

    comando(dataBase* bd) {
        this->bd = bd;
    }

    virtual ~comando() {
    }

    virtual void ejecutar() = 0;
};


/* =========================================================
   ADD
   ========================================================= */

class agregar : public comando {

private:
    fecha f;
    char* n;

public:
    agregar() : comando() {
        f = fecha(0, 0, 0);
        n = nullptr;
    }

    agregar(
        dataBase* db,
        fecha fechaEvento,
        char* nombre
    ) : comando(db) {

        f = fechaEvento;

        n =
            new char[
                strlen(nombre) + 1
            ];

        strcpy(n, nombre);
    }

    ~agregar() {
        delete[] n;
    }

    void ejecutar() override {

        bd->agregarEvento(
            f,
            n
        );
    }
};


/* =========================================================
   DELETE
   ========================================================= */

class eliminar : public comando {

private:
    fecha f;
    char* n;
    bool tieneFecha;

public:
    eliminar() : comando() {

        f = fecha(0, 0, 0);
        n = nullptr;
        tieneFecha = false;
    }


    eliminar(
        dataBase* db,
        fecha fechaEvento,
        char* nombre
    ) : comando(db) {

        f = fechaEvento;

        n =
            new char[
                strlen(nombre) + 1
            ];

        strcpy(n, nombre);

        tieneFecha = true;
    }


    eliminar(
        dataBase* db,
        fecha fechaEvento
    ) : comando(db) {

        f = fechaEvento;

        n = nullptr;

        tieneFecha = true;
    }


    eliminar(
        dataBase* db,
        const char* nombre
    ) : comando(db) {

        f = fecha(0, 0, 0);

        n =
            new char[
                strlen(nombre) + 1
            ];

        strcpy(n, nombre);

        tieneFecha = false;
    }


    ~eliminar() {
        delete[] n;
    }


    void ejecutar() override {

        if (n == nullptr) {

            int c =
                bd
                ->eliminarEventosPorFecha(f);

            cout
                << "Deleted "
                << c
                << " events"
                << endl;

            return;
        }


        if (!tieneFecha) {

            int c =
                bd
                ->eliminarEventosPorNombre(n);

            cout
                << "Deleted "
                << c
                << " events"
                << endl;

            return;
        }


        if (
            bd->eliminarEvento(
                f,
                n
            )
        ) {

            cout
                << "Deleted successfully"
                << endl;

        }
        else {

            cout
                << "Event not found"
                << endl;
        }
    }
};


/* =========================================================
   EDIT
   ========================================================= */

class editar : public comando {

private:
    fecha f;
    char* n;

    fecha nf;
    char* nn;

public:
    editar() : comando() {

        f = fecha(0, 0, 0);
        nf = fecha(0, 0, 0);

        n = nullptr;
        nn = nullptr;
    }


    editar(
        dataBase* db,
        fecha fechaEvento,
        char* nombre,
        fecha nuevaFecha,
        char* nuevoNombre
    ) : comando(db) {

        f = fechaEvento;
        nf = nuevaFecha;


        n =
            new char[
                strlen(nombre) + 1
            ];

        strcpy(n, nombre);


        nn =
            new char[
                strlen(nuevoNombre) + 1
            ];

        strcpy(
            nn,
            nuevoNombre
        );
    }


    ~editar() {

        delete[] n;
        delete[] nn;
    }


    void ejecutar() override {

        if (
            bd->editarEvento(
                f,
                n,
                nf,
                nn
            )
        ) {

            cout
                << "Event Edited"
                << endl;

        }
        else {

            cout
                << "Edit Failed"
                << endl;
        }
    }
};


/* =========================================================
   FIND
   ========================================================= */

class encontrar : public comando {

private:
    fecha f;
    char* n;

public:

    encontrar(
        dataBase* db,
        fecha fechaEvento
    ) : comando(db) {

        f = fechaEvento;
        n = nullptr;
    }


    encontrar(
        dataBase* db,
        const char* nombre
    ) : comando(db) {

        f = fecha(0, 0, 0);

        n =
            new char[
                strlen(nombre) + 1
            ];

        strcpy(
            n,
            nombre
        );
    }


    ~encontrar() {
        delete[] n;
    }


    void ejecutar() override {

        if (n == nullptr) {

            bd->findPorFecha(f);

        }
        else {

            bd->findPorPalabra(n);
        }
    }
};


/* =========================================================
   PRINT
   ========================================================= */

class print : public comando {

public:

    print(
        dataBase* db
    ) : comando(db) {
    }

    void ejecutar() override {
        bd->print();
    }
};


/* =========================================================
   FUNCIONES AUXILIARES
   ========================================================= */

bool leerLinea(
    char* linea,
    int max
) {

    return cin.getline(
        linea,
        max
    );
}


void separarLinea(
    char* linea,
    char** palabras,
    int maxPalabras,
    int& cantPalabras
) {

    cantPalabras = 0;

    char* p =
        strtok(
            linea,
            " "
        );


    while (
        p != nullptr &&
        cantPalabras < maxPalabras
    ) {

        palabras[cantPalabras] =
            p;

        cantPalabras++;

        p =
            strtok(
                nullptr,
                " "
            );
    }
}


bool esDigito(char c) {

    return (
        c >= '0' &&
        c <= '9'
    );
}


bool leerEntero(
    char* texto,
    int& pos,
    int& valor
) {

    valor = 0;

    int signo = 1;


    if (
        texto[pos] == '-' ||
        texto[pos] == '+'
    ) {

        if (texto[pos] == '-')
            signo = -1;

        pos++;
    }


    if (
        !esDigito(
            texto[pos]
        )
    ) {

        return false;
    }


    while (
        esDigito(
            texto[pos]
        )
    ) {

        valor =
            valor * 10 +
            (
                texto[pos] -
                '0'
            );

        pos++;
    }


    valor *= signo;

    return true;
}


void copiarCampoFecha(
    const char* texto,
    int inicio,
    char fin,
    char* salida
) {

    int i = 0;


    while (
        texto[inicio] != '\0' &&
        texto[inicio] != fin
    ) {

        salida[i++] =
            texto[inicio++];
    }


    salida[i] = '\0';
}


bool errorCampoFecha(
    const char* mensaje,
    const char* texto,
    int inicio,
    char fin
) {

    char campo[20];


    copiarCampoFecha(
        texto,
        inicio,
        fin,
        campo
    );


    cout
        << mensaje
        << campo
        << endl;


    return false;
}


bool convertirAFecha(
    char* texto,
    fecha& f
) {

    int pos = 0;

    int y = 0;
    int m = 0;
    int d = 0;


    int inicioYear =
        pos;


    if (
        !leerEntero(
            texto,
            pos,
            y
        )
    ) {

        return errorCampoFecha(
            "Year value is invalid: ",
            texto,
            inicioYear,
            '-'
        );
    }


    if (
        texto[pos] != '-'
    ) {

        cout
            << "Wrong date format: "
            << texto
            << endl;

        return false;
    }


    pos++;


    int inicioMes =
        pos;


    if (
        !leerEntero(
            texto,
            pos,
            m
        )
    ) {

        return errorCampoFecha(
            "Month value is invalid: ",
            texto,
            inicioMes,
            '-'
        );
    }


    if (
        texto[pos] != '-'
    ) {

        return errorCampoFecha(
            "Month value is invalid: ",
            texto,
            inicioMes,
            '-'
        );
    }


    pos++;


    int inicioDia =
        pos;


    if (
        !leerEntero(
            texto,
            pos,
            d
        )
    ) {

        return errorCampoFecha(
            "Day value is invalid: ",
            texto,
            inicioDia,
            '\0'
        );
    }


    if (
        texto[pos] != '\0'
    ) {

        return errorCampoFecha(
            "Day value is invalid: ",
            texto,
            inicioDia,
            '\0'
        );
    }


    int ay =
        (y < 0)
        ? -y
        : y;


    if (
        ay < 1000 ||
        ay > 9999
    ) {

        cout
            << "Year value is invalid: "
            << y
            << endl;

        return false;
    }


    if (
        m < 1 ||
        m > 12
    ) {

        cout
            << "Month value is invalid: "
            << m
            << endl;

        return false;
    }


    if (
        d < 1 ||
        d > 31
    ) {

        cout
            << "Day value is invalid: "
            << d
            << endl;

        return false;
    }


    f =
        fecha(
            y,
            m,
            d
        );


    return true;
}


/* =========================================================
   IDENTIFICAR COMANDO
   ========================================================= */

int identificarComando(
    char* linea,
    fecha& f,
    char* nombre,
    bool& tieneNombre,
    fecha& nf,
    char* nuevoNombre
) {

    char* palabras[10];

    int cant = 0;


    separarLinea(
        linea,
        palabras,
        10,
        cant
    );


    if (cant == 0)
        return 0;


    /* ADD */
    if (
        strcmp(
            palabras[0],
            "Add"
        ) == 0
    ) {

        if (cant != 3) {

            cout
                << "Unknown command: Add"
                << endl;

            return -1;
        }


        if (
            !convertirAFecha(
                palabras[1],
                f
            )
        ) {

            return -1;
        }


        strcpy(
            nombre,
            palabras[2]
        );


        tieneNombre =
            true;


        return 1;
    }


    /* DELETE */
    if (
        strcmp(
            palabras[0],
            "Del"
        ) == 0
    ) {

        if (
            cant != 2 &&
            cant != 3
        ) {

            cout
                << "Unknown command: Del"
                << endl;

            return -1;
        }


        if (
            convertirAFecha(
                palabras[1],
                f
            )
        ) {

            if (cant == 2) {

                tieneNombre =
                    false;

                return 3;
            }


            strcpy(
                nombre,
                palabras[2]
            );


            tieneNombre =
                true;


            return 2;
        }


        if (cant == 2) {

            strcpy(
                nombre,
                palabras[1]
            );


            tieneNombre =
                true;


            return 4;
        }


        return -1;
    }


    /* EDIT */
    if (
        strcmp(
            palabras[0],
            "Edit"
        ) == 0
    ) {

        if (cant != 5)
            return -1;


        if (
            !convertirAFecha(
                palabras[1],
                f
            )
        ) {

            return -1;
        }


        if (
            !convertirAFecha(
                palabras[3],
                nf
            )
        ) {

            return -1;
        }


        strcpy(
            nombre,
            palabras[2]
        );


        strcpy(
            nuevoNombre,
            palabras[4]
        );


        return 5;
    }


    /* FIND */
    if (
        strcmp(
            palabras[0],
            "Find"
        ) == 0
    ) {

        if (cant != 2)
            return -1;


        if (
            strchr(
                palabras[1],
                '-'
            ) != nullptr
        ) {

            if (
                !convertirAFecha(
                    palabras[1],
                    f
                )
            ) {

                return -1;
            }


            tieneNombre =
                false;


            return 6;
        }


        strcpy(
            nombre,
            palabras[1]
        );


        tieneNombre =
            true;


        return 7;
    }


    /* PRINT */
    if (
        strcmp(
            palabras[0],
            "Print"
        ) == 0
    ) {

        if (cant == 1)
            return 8;

        return -1;
    }


    cout
        << "Unknown command: "
        << palabras[0]
        << endl;


    return -1;
}


/* =========================================================
   MAIN
   ========================================================= */

int main() {

    auto inicio =
        chrono::high_resolution_clock::now();


    dataBase bd;


    bd.cargarDesdeArchivo(
        "eventos.txt"
    );


    char linea[1000];

    fecha f;

    char nombre[200];

    bool tieneNombre =
        false;

    fecha nf;

    char nuevoNombre[200];


    while (
        leerLinea(
            linea,
            1000
        )
    ) {

        if (
            linea[0] == '\0'
        ) {

            continue;
        }


        int tipo =
            identificarComando(
                linea,
                f,
                nombre,
                tieneNombre,
                nf,
                nuevoNombre
            );


        if (
            tipo == 0 ||
            tipo == -1
        ) {

            continue;
        }


        comando* cmd =
            nullptr;


        if (tipo == 1) {

            cmd =
                new agregar(
                    &bd,
                    f,
                    nombre
                );
        }
        else if (tipo == 2) {

            cmd =
                new eliminar(
                    &bd,
                    f,
                    nombre
                );
        }
        else if (tipo == 3) {

            cmd =
                new eliminar(
                    &bd,
                    f
                );
        }
        else if (tipo == 4) {

            cmd =
                new eliminar(
                    &bd,
                    nombre
                );
        }
        else if (tipo == 5) {

            cmd =
                new editar(
                    &bd,
                    f,
                    nombre,
                    nf,
                    nuevoNombre
                );
        }
        else if (tipo == 6) {

            cmd =
                new encontrar(
                    &bd,
                    f
                );
        }
        else if (tipo == 7) {

            cmd =
                new encontrar(
                    &bd,
                    nombre
                );
        }
        else if (tipo == 8) {

            cmd =
                new print(
                    &bd
                );
        }


        if (cmd != nullptr) {

            cmd->ejecutar();

            delete cmd;
        }
    }


    bd.guardarEnArchivo(
        "eventos.txt"
    );


    auto fin =
        chrono::high_resolution_clock::now();


    chrono::duration<double> tiempo =
        fin - inicio;


    cout
        << "\n=============================="
        << endl;

    cout
        << "Tiempo de ejecucion: "
        << tiempo.count()
        << " segundos"
        << endl;

    cout
        << "=============================="
        << endl;


    return 0;
}
