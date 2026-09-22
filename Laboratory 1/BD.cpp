#include <iostream>
#include <cstring>
// libreria para guardar y usar txt
#include <fstream>

using namespace std;

/*
===============================
CLASES:
- Fecha
- Evento
- Base de datos

- Comandos
    - Agregar Evento
    - Eliminar evento (uno y varios)
    - Editar
    - Encontrar por nombre o fecha
    - Print
===============================
*/

class fecha{
    //atributos
    private:
    int year;
    int month;
    int day;

    // metodos
    public:
    // constructores
    fecha(){
        year = 0;
        month = 0;
        day = 0;
    }
    fecha(int y, int m, int d){
        year = y;
        month = m;
        day = d;
    }

    // getters
    int getYear(){
        return year;
    }
    int getMonth(){
        return month;
    }
    int getDay(){
        return day;
    }
    // setters
    void setYear(int y){
        year = y;
    }
    void setMonth(int m){
        month = m;
    }
    void setDay(int d){
        day = d;
    }
    // otros
    bool fechasIguales(fecha other){
        if(other.year == year && other.month == month && other.day == day){
            return true;
        }
        return false;
    }
    int compararFechas(fecha other){ // si es mayor = 1, si es igual = 0, si es menor = -1
        if(fechasIguales(other)) return 0;
        if(year > other.year) return 1;
        else if (year < other.year) return -1;
        else if (year == other.year){
            if (month > other.month) return 1;
            else if (month < other.month) return -1;
            else if(month == other.month) {
                if (day > other.day) return 1;
                else if( day< other.day) return -1;
            }
        }
        return 0;
    }
    void formatoFecha(char* s, int size){
        // entero a cadena:
        char ystr[6];
        char mstr[3];
        char dstr[3];
        // anio
        int y = year;
        int i=0;
        if(y<0){
            y= -y;
            ystr[i] = '-';
            i++;
        }

        ystr[i] = '0'+((y/1000)%10);
        ystr[i+1] = '0'+((y/100)%10);
        ystr[i+2] = '0'+((y/10)%10);
        ystr[i+3] = '0'+(y%10);
        ystr[i+4] = '\0';

        // mes
        int m = month;

        mstr[0] = '0'+((m/10)%10);
        mstr[1] = '0'+(m%10);
        mstr[2] = '\0';

        // dia
        int d = day;

        dstr[0] = '0'+((d/10)%10);
        dstr[1] = '0'+(d%10);
        dstr[2] = '\0';

        // copiar y concatenar

        strcpy(s,ystr);
        strcat(s, "-");
        strcat(s, mstr);
        strcat(s, "-");
        strcat(s, dstr);

    }

};

class evento{
    //atributos
    private:
    fecha date;
    char* nombre;
    // metodos
    public:
    //constructores
    evento() {
        date.setYear(0);
        date.setMonth(0);
        date.setDay(0);
        nombre= nullptr;
    }

    evento(fecha f, const char* nom) {
        date = f;
        nombre = new char[strlen(nom)+1];
        strcpy(nombre,nom);
    }

    evento(const evento& other) {
        date = other.date;
        if (other.nombre == nullptr) {
            nombre = nullptr;
        } else {
            nombre = new char[strlen(other.nombre)+1];
            strcpy(nombre, other.nombre);
        }
    }
    // operator = 
    evento& operator=(const evento& other) {
        if(this!= &other){
        delete[] nombre;
        date = other.date;
        if (other.nombre == nullptr) {
            nombre = nullptr;
        } else {
            nombre = new char[strlen(other.nombre)+1];
            strcpy(nombre, other.nombre);
        }
        }
        return *this;
    }
    //destructores
    ~evento() {
        delete[] nombre;
    }
    // getters
    fecha getFecha(){
        return date;
    }
    char* getNombre(){
        return nombre;
    }
    // setters
    void setFecha(fecha f){
        date = f;
    }
    void setNombre(char* n){
        delete[] nombre;
        if (n == nullptr) {
            nombre = nullptr;
        } else {
            nombre = new char[strlen(n)+1];
            strcpy(nombre, n);
        }
    }

    //comprobar repetidos
    bool mismaFechaYNombre(fecha f, const char* nom) {
        if(date.fechasIguales(f) && (strcmp(nombre, nom)==0)){
            return true;
        }
        return false;
    }

    bool mismaFecha(fecha f) {
        return (date.fechasIguales(f));
    }

    
    bool contieneSubcadena(const char* patron){
    return (strstr(nombre, patron) != nullptr);
    }

};
bool convertirAFecha(char* texto, fecha& f);
bool convertirAFecha(char* texto, fecha& f);
void separarLinea(char* linea, char** palabras, int maxPalabras, int& cantPalabras);
class dataBase{

    //atributos
    private:
    evento* eventos;
    int cantidad;
    int capacidad;



    // metodos

    int encontrarIndice(fecha f, const char* nom) {
        for(int i=0; i<cantidad; i++){
            if(eventos[i].mismaFechaYNombre(f,nom)){
                return i;
            }
        }
        return -1;
    }

    void asegurarCapacidad() {
        if (cantidad < capacidad) return;
        int nueva_capacidad = capacidad+1; // capacidad 1x1

        evento* nuevo = new evento[nueva_capacidad];
        for (int i = 0; i < cantidad; i++) {
            nuevo[i] = eventos[i];
        }
        delete[] eventos; 
        eventos = nuevo;
        capacidad = nueva_capacidad;
    }

    void ajustarCapacidad() {
    int nueva_capacidad = (cantidad == 0) ? 1 : cantidad; // mínimo 1
    if (nueva_capacidad == capacidad) return;

    evento* nuevo = new evento[nueva_capacidad];
    for (int i = 0; i < cantidad; i++) nuevo[i] = eventos[i];

    delete[] eventos;
    eventos = nuevo;
    capacidad = nueva_capacidad;
    }

    void ordenarEventos(){
    for(int i = 0; i < cantidad - 1; i++){
        for(int j = 0; j < cantidad - 1 - i; j++){

            int cmp = eventos[j].getFecha().compararFechas(eventos[j+1].getFecha());
            bool intercambiar = false;

            if(cmp > 0){
                intercambiar = true;
            }
            else if(cmp == 0){
                if(strcmp(eventos[j].getNombre(), eventos[j+1].getNombre()) > 0){
                    intercambiar = true;
                }
            }

            if(intercambiar){
                evento aux = eventos[j];
                eventos[j] = eventos[j+1];
                eventos[j+1] = aux;
            }
        }
    }
}


    // no son necesarios de implementar
    dataBase(const dataBase& other);            
    dataBase& operator=(const dataBase& other); 

    public:

    // constructores

    dataBase() {
        capacidad = 1;
        cantidad = 0;
        eventos= new evento[capacidad];
    }


    // destructor

    ~dataBase() {
        delete[] eventos;
    }
    
    
    // metodos principales (primera fase)
    void agregarEvento(fecha f, char* nom) {
        int indx = encontrarIndice(f,nom);
        if(indx!=-1){
            cout<<"Duplicate event"<<endl;
            return;
        }
        asegurarCapacidad();
        eventos[cantidad].setFecha(f);
        eventos[cantidad].setNombre(nom);
        cantidad++;
        ordenarEventos();
    }

    bool eliminarEvento(fecha f, const char* nom) {
        int indx = encontrarIndice(f,nom);
        if(indx==-1){
        return false;
        }
        eventos[indx] = eventos[cantidad-1];
        cantidad--;
        ajustarCapacidad();
        ordenarEventos();
        return true;
    }

    int eliminarEventosPorFecha(fecha f) {
        int c=0;
        int i=0; 
        while(i<cantidad){
            if(eventos[i].getFecha().fechasIguales(f)){
                eventos[i]= eventos[cantidad-1];
                cantidad--;
                ajustarCapacidad();
                c++;
            }else{
                i++;
            }
        }
        ordenarEventos();
        return c;
    
    }
    int eliminarEventosPorNombre(char* n) { // nuevo formato de comando para eliminar
        int c=0;
        int i=0; 
        while(i<cantidad){
            if(strcmp(n,eventos[i].getNombre())==0){
                eventos[i]= eventos[cantidad-1];
                cantidad--;
                ajustarCapacidad();
                c++;
            }else{
                i++;
            }
        }
        ordenarEventos();
        return c;
    }


    // funciones principales (fase 2)
    //edit
    bool editarEvento(fecha f, char* n, fecha nf, char* nn){
        // existe el evento 
        int indx = encontrarIndice(f,n);
        if(indx == -1){
            return false;
        }
        // evento no duplicado
        int dup = encontrarIndice(nf, nn);
        if(dup != -1 && dup != indx) return false;
        // editar
        if(!(eventos[indx].mismaFecha(nf))) {
            eventos[indx].setFecha(nf);
        }
        if(strcmp(eventos[indx].getNombre(),nn)!=0) {
            eventos[indx].setNombre(nn);
        }
        ordenarEventos();
        return true;
    }

    //find
    void findPorFecha(fecha f){
    bool encontrado = false;
    char sfecha[20];

    for(int i = 0; i < cantidad; i++){
        if(eventos[i].getFecha().fechasIguales(f)){
            eventos[i].getFecha().formatoFecha(sfecha, 20);
            cout << sfecha << " " << eventos[i].getNombre() << endl;
            encontrado = true;
        }
    }

    if(!encontrado){
        cout << "Event not found" << endl;
    }
    }

    void findPorPalabra(char* palabra){
    bool encontrado = false;
    char sfecha[20];

    for(int i = 0; i < cantidad; i++){
        if(strstr(eventos[i].getNombre(), palabra) != nullptr){
            eventos[i].getFecha().formatoFecha(sfecha, 20);
            cout << sfecha << " " << eventos[i].getNombre() << endl;
            encontrado = true;
        }
    }

    if(!encontrado){
        cout << "Event not found" << endl;
    }
}

    //print

    void print(){
    char sfecha[20];

    for(int i = 0; i < cantidad; i++){
        eventos[i].getFecha().formatoFecha(sfecha, 20);
        cout << sfecha << " " << eventos[i].getNombre() << endl;
    }
}

    //Guardar y usar base de datos
    

    void guardarEnArchivo(const char* nombreArchivo){
    // ofstream -> abrir archivo
        ofstream out(nombreArchivo);
        if(!out) return;

        char sfecha[20]; // para el texto de la fecha

        for(int i = 0; i < cantidad; i++){
            eventos[i].getFecha().formatoFecha(sfecha, 20); //crea el texto de fecha en formato
            out << sfecha << " " << eventos[i].getNombre() << endl; // escribela linea como FECHA NOMBRE
        }

        out.close(); // cierra archivo
    }
    

    void cargarDesdeArchivo(const char* nombreArchivo){
        ifstream in(nombreArchivo);
        if(!in) return;

        char linea[300];
        char* palabras[10];
        int cant = 0;
        fecha f;

        while(in.getline(linea, 300)){
            separarLinea(linea, palabras, 10, cant);
            if(cant >= 2){
                if(convertirAFecha(palabras[0], f)){
                    agregarEvento(f, palabras[1]);
                }
            }
        }
    }
};

class comando{
    //atributos
    protected:
    dataBase* bd;

    // metodos
    public:
    comando(){
        bd= nullptr;
    }

    comando(dataBase* bd) {
        this->bd = bd;
    }

    virtual ~comando() {
        
    }

    virtual void ejecutar() = 0;
};

class agregar : public comando{
    // atributos
    private:
    fecha f;
    char* n;
    //metodos
    public:
    agregar(): comando(){
        f= fecha(0,0,0);
        n=nullptr;
    }
    agregar(dataBase* db, fecha fecha, char* nombre) : comando(db){
        f = fecha; 
        n = new char[strlen(nombre)+1];
        strcpy(n, nombre);
    }
    ~agregar(){
        delete[] n;
    }
    void ejecutar() override {
        bd->agregarEvento(f,n);
    }
};

class eliminar : public comando{
    // atributos
    private:
    fecha f;
    char* n;
    bool tieneFecha;
    //metodos
    public:
    eliminar(): comando(){
        f= fecha(0,0,0);
        n=nullptr;
        tieneFecha =0;
    }
    // contructor pa el que tiene todo
    eliminar(dataBase* db, fecha fecha, char* nombre) : comando(db){
        f = fecha; 
        n = new char[strlen(nombre)+1];
        strcpy(n, nombre);
        tieneFecha = 1;
    }
    // constuctor con fecha
    eliminar(dataBase* db, fecha fe) : comando(db){
        f = fe;
        n = nullptr;
        tieneFecha = 1;
    }
    // constructor con nombre
    eliminar(dataBase* db, const char* nombre) : comando(db){
        f = fecha(0,0,0);
        n = new char[strlen(nombre)+1];
        strcpy(n, nombre);
        tieneFecha = 0;
    }
    ~eliminar(){
        delete[] n;
    }
    void ejecutar() override {
        if(n == nullptr){
            int c=bd->eliminarEventosPorFecha(f);
            cout << "Deleted " <<c << " events" << endl;
            return;
        }
        else if(!tieneFecha){
            int c = bd->eliminarEventosPorNombre(n);
            cout << "Deleted " <<c << " events" << endl;
        }else{

            if(bd->eliminarEvento(f,n)){
                cout << "Deleted successfully" << endl;
            }else{
                cout << "Event not found" << endl;
            }
        }
    }
};

class editar : public comando{
    // atributos
    private:
    fecha f;
    char* n;
    fecha nf;
    char* nn;
    //metodos
    public:
    editar(): comando(){
        f= fecha(0,0,0);
        n=nullptr;
        nf= fecha(0,0,0);
        nn=nullptr;
    }
    editar(dataBase* db, fecha fe, char* nombre, fecha nuevaFecha, char* nuevoNombre) : comando(db){
        f = fe; 
        n = new char[strlen(nombre)+1];
        strcpy(n, nombre);
        nf = nuevaFecha; 
        nn = new char[strlen(nuevoNombre)+1];
        strcpy(nn, nuevoNombre);
    }
    ~editar(){
        delete[] nn;
        delete[] n;
    }
    void ejecutar() override {
        if(!bd->editarEvento(f,n,nf,nn)){
            cout<<"Edit Failed"<<endl;
        }else{
            cout<<"Event Edited"<<endl;
        }
    }
};

class encontrar : public comando{
    // atributos
    private:
    fecha f;
    char* n;
    //metodos
    public:
    // constructor find por fecha
    encontrar(dataBase* db, fecha fe) : comando(db) {
        f = fe;
        n = nullptr;
    }
    // constructor find por palabr
    encontrar(dataBase* db, const char* nombre) : comando(db) {
        f = fecha(0,0,0);
        n = new char[strlen(nombre) + 1];
        strcpy(n, nombre);
    }
    ~encontrar(){
        delete[] n;
    }
    void ejecutar() override {
        if(n == nullptr){
            bd->findPorFecha(f);
        }else{
            bd->findPorPalabra(n);
        }
    }
};

class print : public comando{
    // atributos
    private:
    
    //metodos
    public:
    
    print(dataBase* db) : comando(db){}
    
    void ejecutar() override {
        bd->print();
    }
};

/*
==================================
FUNCIONES:
- Leer 
- separar linea
- Identificar digito
- ller entero
- convertir texto a fecha
- identificar comando
==================================
*/

bool leerLinea(char* linea, int max){
    if(!cin.getline(linea,max)) return false;
    return true;
}
void separarLinea(char* linea, char** palabras, int maxPalabras, int& cantPalabras){
    cantPalabras = 0;
    char* p = strtok(linea," ");

    while (p!=nullptr && cantPalabras < maxPalabras){
        palabras[cantPalabras] = p;
        cantPalabras++;
        p= strtok(nullptr, " ");
    }
}
bool esDigito(char c) {
    return (c >= '0' && c <= '9');
}
bool leerEntero( char* texto, int & pos, int & valor){
    valor = 0;            
    int signo = 1;

    if(texto[pos] == '-' || texto[pos] == '+'){ //lee signo
        if(texto[pos] == '-') signo = -1;
        pos++;
    }

    if(!esDigito(texto[pos])) return false;

    while(esDigito(texto[pos])){
        valor = valor*10 + (texto[pos]-'0');
        pos++;
    }

    valor *= signo;
    return true;
}
// COPIAR YEAR , MONTH O DAY
void copiarCampoFecha(const char* texto, int inicio, char fin, char* salida){
    int i = 0;
    while(texto[inicio] != '\0' && texto[inicio] != fin){
        salida[i++] = texto[inicio++];
    }
    salida[i] = '\0';
}
// MOSTRAR ERROR POR CAMPO
bool errorCampoFecha(const char* mensaje, const char* texto, int inicio, char fin){
    char campo[20];
    copiarCampoFecha(texto, inicio, fin, campo);
    cout << mensaje << campo << endl;
    return false;
}
bool convertirAFecha(char* texto, fecha& f){
    int pos = 0;
    int y=0 , m=0, d=0;

    int inicioYear = pos;
    if(!leerEntero(texto,pos, y)) // sino esa entero, muestra error de campo
        return errorCampoFecha("Year value is invalid: ", texto, inicioYear, '-');

    if(texto[pos] != '-'){ // prueba formato
        cout<<"Wrong date format: "<<texto<<endl;
        return false;
    }
    pos++;

    int inicioMes = pos;
    if(!leerEntero(texto,pos, m))
        return errorCampoFecha("Month value is invalid: ", texto, inicioMes, '-');

    if(texto[pos] != '-')
        return errorCampoFecha("Month value is invalid: ", texto, inicioMes, '-');
    pos++;

    int inicioDia = pos;
    if(!leerEntero(texto,pos, d))
        return errorCampoFecha("Day value is invalid: ", texto, inicioDia, '\0');

    if(texto[pos] != '\0')
        return errorCampoFecha("Day value is invalid: ", texto, inicioDia, '\0');

    int ay = (y < 0) ? -y : y;
    if(ay < 1000 || ay > 9999){
        cout<<"Year value is invalid: "<<y<<endl;
        return false;
    }

    if(m < 1 || m > 12){
        cout<<"Month value is invalid: "<<m<<endl;
        return false;
    }

    if(d < 1 || d > 31){
        cout<<"Day value is invalid: "<<d<<endl;
        return false;
    }

    f = fecha(y,m,d);
    return true;
}

int identificarComando(char* linea, fecha& f, char* nombre, bool& tieneNombre, fecha& nf, char* nuevoNombre){
    /*
    0 = línea vacía
    1 = Add
    2 = Del con nombre (borrar uno)
    3 = Del sin nombre (borrar varios por fecha)
    4 = Del sin fecha (borrar varios por nombre)
    5 = Edit
    6 = Find fecha
    7 = Find nombre
    8 = Print
    -1 = comando desconocido (terminar)
    */
   char* palabras[10];
    int cant = 0;

    separarLinea(linea, palabras, 10, cant);

    if (cant == 0) return 0; // línea vacía

    // ADD FECHA NOMBRE
    if (strcmp(palabras[0], "Add") == 0) {
        if (cant == 1) {
            cout << "Unknown command: " << palabras[0] << endl;
            return -1;
        }

        if (cant == 2) {
        
            if (!convertirAFecha(palabras[1], f)) return -1;
            cout << "Unknown event" << endl;
            return -1;
        }
        if (cant != 3) {
            cout << "Unknown command: " << palabras[0] << endl;
            return -1;
        }

        if (!convertirAFecha(palabras[1], f)) {
            return -1; 
        }

        strcpy(nombre, palabras[2]);
        tieneNombre = true;
        return 1;
    }

    // DEL FECHA  o  DEL FECHA NOMBRE o DEL NOMBRE
    if (strcmp(palabras[0], "Del") == 0) {
        if (cant != 2 && cant != 3) {
        cout << "Unknown command: " << palabras[0] << endl;
        return -1;
        }

        if (convertirAFecha(palabras[1], f)) {
            if (cant == 2) {
                tieneNombre = false;
                return 3;
            } else {
                strcpy(nombre, palabras[2]);
                tieneNombre = true;
                return 2;
            }
        } else {
            if (cant != 2) {
                cout << "Unknown command: " << palabras[0] << endl;
                return -1;
            }
            strcpy(nombre, palabras[1]);
            tieneNombre = true;
            return 4;
        }
    }
    // EDITAR
    if(strcmp(palabras[0], "Edit") == 0){
        if(cant != 5){
            cout << "Unknown command: " << palabras[0] << endl;
            return -1;
        }

        if(!convertirAFecha(palabras[1], f)){
            return -1;
        }

        if(!convertirAFecha(palabras[3], nf)){
            return -1;
        }

        strcpy(nombre, palabras[2]);
        strcpy(nuevoNombre, palabras[4]);

        return 5;
    }
    // ENCONTRAR FECHA
    // ENCONTRAR NOMBRE 
    if(strcmp(palabras[0], "Find")==0){
        if(cant != 2){
            cout << "Unknown command: " << palabras[0] << endl;
            return -1;
        }

        if(strchr(palabras[1], '-') != nullptr){
            if(!convertirAFecha(palabras[1], f)) return -1; 
            tieneNombre = false;
            return 6; 
        } else {
            strcpy(nombre, palabras[1]);
            tieneNombre = true;
            return 7; 
        }
    }
    // PRINT TODO
    if(strcmp(palabras[0], "Print")==0){
        if(cant ==1){
            return 8;
        }   
        cout << "Unknown command: " << palabras[0] << endl;
        return -1;
    }

    cout << "Unknown command: " << palabras[0] << endl;
    return -1;
}


int main() {

   dataBase bd;        // base de datos con eventos
    //cargar datos
    bd.cargarDesdeArchivo("eventos.txt");
    char linea[1000];       
    fecha f;                
    char nombre[200];       
    bool tieneNombre = false;
    fecha nf;                
    char nn[200];

    while (leerLinea(linea, 1000)) {          // 1) leer una linea
        if (linea[0] == '\0') continue;       // 2)  ignorar linea vacia

        int tipo = identificarComando(linea, f, nombre, tieneNombre, nf, nn);  // 3) reconocer comando

        if (tipo == 0) continue;   // linea vacia
        if (tipo == -1) continue;  // comando inválido -> ignorar y seguir

        comando* cmd = nullptr;    

        if (tipo == 1) {               // Add FECHA NOMBRE
            cmd = new agregar(&bd, f, nombre);
        }
        else if (tipo == 2) {          // Del FECHA NOMBRE (borra uno)
            cmd = new eliminar(&bd, f, nombre);
        }
        else if (tipo == 3) {          // Del FECHA (borra varios por fecha)
            cmd = new eliminar(&bd, f);
        }
        else if (tipo == 4) {          // Del NOMBTE (borra varios por fecha)
            cmd = new eliminar(&bd, nombre);
        }
        else if (tipo == 5) {          // Del FECHA (borra varios por fecha)
            cmd = new editar(&bd, f,nombre,nf,nn);
        }
        else if (tipo == 6) {          // Del FECHA (borra varios por fecha)
            cmd = new encontrar(&bd, f);
        }
        else if (tipo == 7) {          // Del FECHA (borra varios por fecha)
            cmd = new encontrar(&bd, nombre);
        }
        else if(tipo == 8) {
            cmd = new print(&bd);
        }

        if (cmd != nullptr) {
            cmd->ejecutar();   // 5) ejecutar comando
            delete cmd;        // 6) borrar el comando (no la base de datos)
        }


    }
    bd.guardarEnArchivo("eventos.txt");
    return 0;
}
