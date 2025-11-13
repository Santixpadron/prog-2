#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <fstream>

using namespace std;

// ===================== ESTRUCTURAS MODIFICADAS PARA PERSISTENCIA =====================

struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};

struct HistorialMedico {
    int idConsulta;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costo;
    
    // NUEVO: Para lista enlazada en disco
    int siguienteConsultaID;  // -1 si es la última
    bool eliminado;
};

// Estructuras modificadas SIN punteros dinámicos
struct Paciente {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    int edad;
    char sexo;
    char tipoSangre[5];
    char telefono[15];
    char direccion[100];
    char email[50];

    // REEMPLAZADO: Arrays dinámicos por arrays fijos
    int cantidadConsultas;
    int primerConsultaID;           // Primera consulta en historiales.bin
    
    int cantidadCitas;
    int citasIDs[20];               // Máximo 20 citas

    char alergias[500];
    char observaciones[500];
    bool activo;
    bool eliminado;                 // NUEVO: Borrado lógico
};

struct Doctor {
    int id;
    char nombre[50];
    char apellido[50];
    char cedula[20];
    char especialidad[50];
    int aniosExperiencia;
    float costoConsulta;
    char horarioAtencion[50];
    char telefono[15];
    char email[50];

    // REEMPLAZADO: Arrays dinámicos por arrays fijos
    int cantidadPacientes;
    int pacientesIDs[50];           // Máximo 50 pacientes
    
    int cantidadCitas;
    int citasIDs[30];               // Máximo 30 citas

    bool disponible;
    bool eliminado;                 // NUEVO: Borrado lógico
};

struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20];
    char observaciones[200];
    bool atendida;
    bool eliminado;                 // NUEVO: Borrado lógico
    int consultaID;                 // NUEVO: Referencia a consulta creada
};

// Hospital simplificado - solo datos básicos
struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];

    // SOLO contadores - NO arrays de datos
    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;

    // Estadísticas
    int totalPacientesRegistrados;
    int totalDoctoresRegistrados;
    int totalCitasAgendadas;
    int totalConsultasRealizadas;
};

// ===================== CONSTANTES GLOBALES =====================

const char* ARCHIVO_HOSPITAL = "hospital.bin";
const char* ARCHIVO_PACIENTES = "pacientes.bin";
const char* ARCHIVO_DOCTORES = "doctores.bin";
const char* ARCHIVO_CITAS = "citas.bin";
const char* ARCHIVO_HISTORIAL = "historiales.bin";

const int VERSION_ACTUAL = 1;

// ===================== PROTOTIPOS DE FUNCIONES DE ARCHIVO =====================

// Funciones de inicialización
bool inicializarArchivo(const char* nombreArchivo, int proximoID = 1);
bool verificarArchivo(const char* nombreArchivo);
ArchivoHeader leerHeader(const char* nombreArchivo);
bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& header);

// Funciones de acceso aleatorio
long calcularPosicion(int indice, size_t tamañoRegistro);
Paciente leerPacientePorIndice(int indice);
Doctor leerDoctorPorIndice(int indice);
Cita leerCitaPorIndice(int indice);
HistorialMedico leerHistorialPorIndice(int indice);

// Funciones de búsqueda
int buscarIndicePacientePorID(int id);
int buscarIndiceDoctorPorID(int id);
int buscarIndiceCitaPorID(int id);

// Funciones CRUD para pacientes
bool guardarPaciente(const Paciente& paciente, int indice = -1);
bool agregarPaciente(Paciente& paciente);
Paciente buscarPacientePorID(int id);
Paciente buscarPacientePorCedula(const char* cedula);

// Funciones CRUD para doctores
bool guardarDoctor(const Doctor& doctor, int indice = -1);
bool agregarDoctor(Doctor& doctor);
Doctor buscarDoctorPorID(int id);

// Funciones CRUD para citas
bool guardarCita(const Cita& cita, int indice = -1);
bool agregarCita(Cita& cita);
Cita buscarCitaPorID(int id);

// Funciones de historial médico
bool guardarHistorial(const HistorialMedico& historial, int indice = -1);
bool agregarHistorial(HistorialMedico& historial);
bool agregarConsultaAlHistorial(int idPaciente, const HistorialMedico& consulta);

// Funciones del sistema
Hospital* cargarSistema();
bool guardarSistema(Hospital* hospital);
void limpiarSistema(Hospital* hospital);

// ===================== IMPLEMENTACIONES DE ARCHIVOS =====================

bool inicializarArchivo(const char* nombreArchivo, int proximoID) {
    ofstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;
    
    ArchivoHeader header;
    header.cantidadRegistros = 0;
    header.proximoID = proximoID;
    header.registrosActivos = 0;
    header.version = VERSION_ACTUAL;
    
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    
    return true;
}

ArchivoHeader leerHeader(const char* nombreArchivo) {
    ArchivoHeader header;
    memset(&header, 0, sizeof(ArchivoHeader));
    
    ifstream archivo(nombreArchivo, ios::binary);
    if (archivo.is_open()) {
        archivo.read((char*)&header, sizeof(ArchivoHeader));
        archivo.close();
    }
    return header;
}

bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& header) {
    fstream archivo(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    archivo.seekp(0);
    archivo.write((char*)&header, sizeof(ArchivoHeader));
    archivo.close();
    return true;
}

long calcularPosicion(int indice, size_t tamañoRegistro) {
    return sizeof(ArchivoHeader) + (indice * tamañoRegistro);
}

// ===================== IMPLEMENTACIONES DE ACCESO ALEATORIO =====================

Paciente leerPacientePorIndice(int indice) {
    Paciente paciente;
    memset(&paciente, 0, sizeof(Paciente));
    
    ifstream archivo(ARCHIVO_PACIENTES, ios::binary);
    if (archivo.is_open()) {
        long posicion = calcularPosicion(indice, sizeof(Paciente));
        archivo.seekg(posicion);
        archivo.read((char*)&paciente, sizeof(Paciente));
        archivo.close();
    }
    return paciente;
}

Doctor leerDoctorPorIndice(int indice) {
    Doctor doctor;
    memset(&doctor, 0, sizeof(Doctor));
    
    ifstream archivo(ARCHIVO_DOCTORES, ios::binary);
    if (archivo.is_open()) {
        long posicion = calcularPosicion(indice, sizeof(Doctor));
        archivo.seekg(posicion);
        archivo.read((char*)&doctor, sizeof(Doctor));
        archivo.close();
    }
    return doctor;
}

Cita leerCitaPorIndice(int indice) {
    Cita cita;
    memset(&cita, 0, sizeof(Cita));
    
    ifstream archivo(ARCHIVO_CITAS, ios::binary);
    if (archivo.is_open()) {
        long posicion = calcularPosicion(indice, sizeof(Cita));
        archivo.seekg(posicion);
        archivo.read((char*)&cita, sizeof(Cita));
        archivo.close();
    }
    return cita;
}

// ===================== IMPLEMENTACIONES DE BÚSQUEDA =====================

int buscarIndicePacientePorID(int id) {
    ArchivoHeader header = leerHeader(ARCHIVO_PACIENTES);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente p = leerPacientePorIndice(i);
        if (p.id == id && !p.eliminado) {
            return i;
        }
    }
    return -1;
}

int buscarIndiceDoctorPorID(int id) {
    ArchivoHeader header = leerHeader(ARCHIVO_DOCTORES);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (d.id == id && !d.eliminado) {
            return i;
        }
    }
    return -1;
}

Paciente buscarPacientePorID(int id) {
    int indice = buscarIndicePacientePorID(id);
    if (indice != -1) {
        return leerPacientePorIndice(indice);
    }
    
    Paciente vacio;
    memset(&vacio, 0, sizeof(Paciente));
    return vacio;
}

Paciente buscarPacientePorCedula(const char* cedula) {
    ArchivoHeader header = leerHeader(ARCHIVO_PACIENTES);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente p = leerPacientePorIndice(i);
        if (strcmp(p.cedula, cedula) == 0 && !p.eliminado) {
            return p;
        }
    }
    
    Paciente vacio;
    memset(&vacio, 0, sizeof(Paciente));
    return vacio;
}

Doctor buscarDoctorPorID(int id) {
    int indice = buscarIndiceDoctorPorID(id);
    if (indice != -1) {
        return leerDoctorPorIndice(indice);
    }
    
    Doctor vacio;
    memset(&vacio, 0, sizeof(Doctor));
    return vacio;
}

// ===================== IMPLEMENTACIONES CRUD =====================

bool guardarPaciente(const Paciente& paciente, int indice) {
    if (indice == -1) {
        indice = buscarIndicePacientePorID(paciente.id);
        if (indice == -1) return false;
    }
    
    fstream archivo(ARCHIVO_PACIENTES, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    long posicion = calcularPosicion(indice, sizeof(Paciente));
    archivo.seekp(posicion);
    archivo.write((char*)&paciente, sizeof(Paciente));
    archivo.close();
    return true;
}

bool agregarPaciente(Paciente& paciente) {
    ArchivoHeader header = leerHeader(ARCHIVO_PACIENTES);
    
    // Asignar ID
    paciente.id = header.proximoID;
    
    ofstream archivo(ARCHIVO_PACIENTES, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    
    archivo.write((char*)&paciente, sizeof(Paciente));
    archivo.close();
    
    // Actualizar header
    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(ARCHIVO_PACIENTES, header);
    
    return true;
}

bool agregarDoctor(Doctor& doctor) {
    ArchivoHeader header = leerHeader(ARCHIVO_DOCTORES);
    
    // Asignar ID
    doctor.id = header.proximoID;
    
    ofstream archivo(ARCHIVO_DOCTORES, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    
    archivo.write((char*)&doctor, sizeof(Doctor));
    archivo.close();
    
    // Actualizar header
    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(ARCHIVO_DOCTORES, header);
    
    return true;
}

bool agregarCita(Cita& cita) {
    ArchivoHeader header = leerHeader(ARCHIVO_CITAS);
    
    // Asignar ID
    cita.id = header.proximoID;
    
    ofstream archivo(ARCHIVO_CITAS, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    
    archivo.write((char*)&cita, sizeof(Cita));
    archivo.close();
    
    // Actualizar header
    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(ARCHIVO_CITAS, header);
    
    return true;
}

// ===================== SISTEMA DE ARCHIVOS =====================

Hospital* cargarSistema() {
    // Verificar e inicializar archivos si no existen
    if (!verificarArchivo(ARCHIVO_HOSPITAL)) {
        inicializarArchivo(ARCHIVO_HOSPITAL);
    }
    if (!verificarArchivo(ARCHIVO_PACIENTES)) {
        inicializarArchivo(ARCHIVO_PACIENTES);
    }
    if (!verificarArchivo(ARCHIVO_DOCTORES)) {
        inicializarArchivo(ARCHIVO_DOCTORES);
    }
    if (!verificarArchivo(ARCHIVO_CITAS)) {
        inicializarArchivo(ARCHIVO_CITAS);
    }
    if (!verificarArchivo(ARCHIVO_HISTORIAL)) {
        inicializarArchivo(ARCHIVO_HISTORIAL);
    }
    
    // Cargar datos del hospital
    Hospital* hospital = new Hospital;
    memset(hospital, 0, sizeof(Hospital));
    
    ifstream archivo(ARCHIVO_HOSPITAL, ios::binary);
    if (archivo.is_open()) {
        // Saltar header
        archivo.seekg(sizeof(ArchivoHeader));
        archivo.read((char*)hospital, sizeof(Hospital));
        archivo.close();
    } else {
        // Valores por defecto
        strcpy(hospital->nombre, "Hospital Central");
        strcpy(hospital->direccion, "Dirección no especificada");
        strcpy(hospital->telefono, "0000000000");
        
        // Usar IDs de los headers de cada archivo
        ArchivoHeader headerPac = leerHeader(ARCHIVO_PACIENTES);
        ArchivoHeader headerDoc = leerHeader(ARCHIVO_DOCTORES);
        ArchivoHeader headerCit = leerHeader(ARCHIVO_CITAS);
        ArchivoHeader headerHist = leerHeader(ARCHIVO_HISTORIAL);
        
        hospital->siguienteIdPaciente = headerPac.proximoID;
        hospital->siguienteIdDoctor = headerDoc.proximoID;
        hospital->siguienteIdCita = headerCit.proximoID;
        hospital->siguienteIdConsulta = headerHist.proximoID;
    }
    
    return hospital;
}

bool guardarSistema(Hospital* hospital) {
    ofstream archivo(ARCHIVO_HOSPITAL, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    // Posicionarse después del header
    archivo.seekp(sizeof(ArchivoHeader));
    archivo.write((char*)hospital, sizeof(Hospital));
    archivo.close();
    
    return true;
}

void limpiarSistema(Hospital* hospital) {
    if (hospital) {
        guardarSistema(hospital);
        delete hospital;
    }
}

bool verificarArchivo(const char* nombreArchivo) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) return false;
    
    // Verificar que tenga al menos el header
    archivo.seekg(0, ios::end);
    long tamaño = archivo.tellg();
    archivo.close();
    
    return tamaño >= sizeof(ArchivoHeader);
}

// ===================== FUNCIONES ORIGINALES ADAPTADAS =====================
// [Manteniendo las funciones originales pero adaptándolas al nuevo sistema]

void limpiarBufferEntrada() {
    cin.ignore(256, '\n');
}

void leerLinea(char* buffer, int size) {
    cin.getline(buffer, size);
    if (cin.fail()) {
        cin.clear();
        limpiarBufferEntrada();
    }
    buffer[size-1] = '\0';
}

int leerEntero() {
    int x;
    if (!(cin >> x)) {
        cin.clear();
        limpiarBufferEntrada();
        return -1;
    }
    limpiarBufferEntrada();
    return x;
}

bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    int len = (int)strlen(cedula);
    if (len < 3 || len > 20) return false;
    if (cedula[1] != '-') return false;
    if (cedula[0] != 'V' && cedula[0] != 'v' && cedula[0] != 'E' && cedula[0] != 'e') return false;
    for (int i = 2; i < len; i++) {
        if (!isdigit((unsigned char)cedula[i])) return false;
    }
    return true;
}

// ===================== FUNCIONES DE GESTIÓN ADAPTADAS =====================

Paciente* crearPaciente(Hospital* hospital, const char* nombre, const char* apellido,
                        const char* cedula, int edad, char sexo) {
    if (!hospital || !nombre || !apellido || !cedula) return nullptr;
    if (!validarCedula(cedula)) {
        cout << "Cedula invalida" << endl;
        return nullptr;
    }
    
    // Verificar si ya existe
    Paciente existente = buscarPacientePorCedula(cedula);
    if (existente.id != 0) {
        cout << "Ya existe un paciente con esa cedula" << endl;
        return nullptr;
    }

    Paciente nuevo;
    memset(&nuevo, 0, sizeof(Paciente));
    
    // Usar ID del sistema
    nuevo.id = hospital->siguienteIdPaciente++;
    strncpy(nuevo.nombre, nombre, sizeof(nuevo.nombre)-1);
    strncpy(nuevo.apellido, apellido, sizeof(nuevo.apellido)-1);
    strncpy(nuevo.cedula, cedula, sizeof(nuevo.cedula)-1);
    nuevo.edad = edad;
    nuevo.sexo = sexo;
    strncpy(nuevo.tipoSangre, "O+", sizeof(nuevo.tipoSangre)-1);
    strncpy(nuevo.telefono, "0000000000", sizeof(nuevo.telefono)-1);
    strncpy(nuevo.direccion, "Sin direccion", sizeof(nuevo.direccion)-1);
    strncpy(nuevo.email, "noemail@hospital.com", sizeof(nuevo.email)-1);
    nuevo.activo = true;
    nuevo.eliminado = false;
    nuevo.cantidadConsultas = 0;
    nuevo.primerConsultaID = -1;
    nuevo.cantidadCitas = 0;

    if (agregarPaciente(nuevo)) {
        hospital->totalPacientesRegistrados++;
        cout << "Paciente creado correctamente con ID: " << nuevo.id << endl;
        
        // Crear copia en memoria para retornar
        Paciente* copia = new Paciente;
        *copia = nuevo;
        return copia;
    }
    
    return nullptr;
}

Doctor* crearDoctor(Hospital* hospital, const char* nombre, const char* apellido,
                    const char* cedula, const char* especialidad, int aniosExperiencia,
                    float costoConsulta) {
    if (!hospital || !nombre || !apellido || !cedula || !especialidad) return nullptr;

    Doctor nuevo;
    memset(&nuevo, 0, sizeof(Doctor));
    
    nuevo.id = hospital->siguienteIdDoctor++;
    strncpy(nuevo.nombre, nombre, sizeof(nuevo.nombre)-1);
    strncpy(nuevo.apellido, apellido, sizeof(nuevo.apellido)-1);
    strncpy(nuevo.cedula, cedula, sizeof(nuevo.cedula)-1);
    strncpy(nuevo.especialidad, especialidad, sizeof(nuevo.especialidad)-1);
    nuevo.aniosExperiencia = aniosExperiencia;
    nuevo.costoConsulta = costoConsulta;
    strncpy(nuevo.horarioAtencion, "Lun-Vie 8:00-16:00", sizeof(nuevo.horarioAtencion)-1);
    strncpy(nuevo.telefono, "0000000000", sizeof(nuevo.telefono)-1);
    strncpy(nuevo.email, "doctor@hospital.com", sizeof(nuevo.email)-1);
    nuevo.disponible = true;
    nuevo.eliminado = false;
    nuevo.cantidadPacientes = 0;
    nuevo.cantidadCitas = 0;

    if (agregarDoctor(nuevo)) {
        hospital->totalDoctoresRegistrados++;
        cout << "Doctor creado correctamente con ID: " << nuevo.id << endl;
        
        Doctor* copia = new Doctor;
        *copia = nuevo;
        return copia;
    }
    
    return nullptr;
}

void listarPacientes(Hospital* hospital) {
    ArchivoHeader header = leerHeader(ARCHIVO_PACIENTES);
    
    cout << "================================================================" << endl;
    cout << "                     LISTA DE PACIENTES                         " << endl;
    cout << "=================================================================" << endl;
    cout << "ID  NOMBRE COMPLETO              CEDULA     EDAD CONS." << endl;
    cout << "-----------------------------------------------------------------" << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente p = leerPacientePorIndice(i);
        if (!p.eliminado) {
            char fullname[101];
            snprintf(fullname, sizeof(fullname), "%s %s", p.nombre, p.apellido);
            
            cout << setw(2) << p.id << "  "
                 << left << setw(28) << fullname
                 << " " << setw(10) << p.cedula
                 << " " << setw(4) << p.edad
                 << " " << setw(5) << p.cantidadConsultas << endl;
        }
    }
    cout << "=================================================================" << endl;
}

void listarDoctores(Hospital* hospital) {
    ArchivoHeader header = leerHeader(ARCHIVO_DOCTORES);
    
    cout << "=================================================================" << endl;
    cout << "                     LISTA DE DOCTORES                          " << endl;
    cout << "=================================================================" << endl;
    cout << "ID  NOMBRE COMPLETO              ESPECIALIDAD           EXP. COSTO" << endl;
    cout << "-----------------------------------------------------------------" << endl;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (!d.eliminado) {
            char fullname[101];
            snprintf(fullname, sizeof(fullname), "%s %s", d.nombre, d.apellido);
            
            cout << setw(2) << d.id << "  "
                 << left << setw(28) << fullname
                 << " " << setw(20) << d.especialidad
                 << " " << setw(4) << d.aniosExperiencia
                 << " " << setw(6) << fixed << setprecision(2) << d.costoConsulta << endl;
        }
    }
    cout << "=================================================================" << endl;
}

// ===================== FUNCIONES DE MENU (SIN CAMBIOS) =====================
// [Las funciones de menu permanecen iguales ya que usan las interfaces adaptadas]

void menuPacientes(Hospital* h) {
    int opcion;
    do {
        cout << endl;
        cout << "=== GESTION DE PACIENTES ===" << endl;
        cout << "1. Crear paciente" << endl;
        cout << "2. Listar pacientes" << endl;
        cout << "3. Buscar paciente por cedula" << endl;
        cout << "4. Actualizar paciente" << endl;
        cout << "5. Eliminar paciente" << endl;
        cout << "6. Ver historial medico" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1: {
                char nombre[50], apellido[50], cedula[20];
                int edad;
                char sexo;
                cout << "Nombre: ";
                leerLinea(nombre, 50);
                cout << "Apellido: ";
                leerLinea(apellido, 50);
                cout << "Cedula (V-12345678): ";
                leerLinea(cedula, 20);
                cout << "Edad: ";
                edad = leerEntero();
                cout << "Sexo (M/F): ";
                cin >> sexo;
                limpiarBufferEntrada();
                Paciente* p = crearPaciente(h, nombre, apellido, cedula, edad, sexo);
                if (p) delete p; // Liberar memoria temporal
                break;
            }
            case 2:
                listarPacientes(h);
                break;
            case 3: {
                char cedula[20];
                cout << "Cedula a buscar: ";
                leerLinea(cedula, 20);
                Paciente p = buscarPacientePorCedula(cedula);
                if (p.id != 0) {
                    cout << "Paciente encontrado: " << p.nombre << " " << p.apellido << " (ID: " << p.id << ")" << endl;
                } else {
                    cout << "Paciente no encontrado" << endl;
                }
                break;
            }
            case 4: {
                // Implementar actualización
                cout << "Funcionalidad en desarrollo..." << endl;
                break;
            }
            case 5: {
                // Implementar eliminación lógica
                cout << "Funcionalidad en desarrollo..." << endl;
                break;
            }
            case 6: {
                // Implementar visualización de historial
                cout << "Funcionalidad en desarrollo..." << endl;
                break;
            }
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while (opcion != 0);
}

void menuDoctores(Hospital* h) {
    int opcion;
    do {
        cout << endl;
        cout << "=== GESTION DE DOCTORES ===" << endl;
        cout << "1. Crear doctor" << endl;
        cout << "2. Listar doctores" << endl;
        cout << "3. Buscar doctor por especialidad" << endl;
        cout << "4. Asignar paciente a doctor" << endl;
        cout << "5. Listar pacientes de doctor" << endl;
        cout << "6. Eliminar doctor" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1: {
                char nombre[50], apellido[50], cedula[20], especialidad[50];
                int experiencia;
                float costo;
                cout << "Nombre: ";
                leerLinea(nombre, 50);
                cout << "Apellido: ";
                leerLinea(apellido, 50);
                cout << "Cedula profesional: ";
                leerLinea(cedula, 20);
                cout << "Especialidad: ";
                leerLinea(especialidad, 50);
                cout << "Anios de experiencia: ";
                experiencia = leerEntero();
                cout << "Costo de consulta: ";
                cin >> costo;
                limpiarBufferEntrada();
                Doctor* d = crearDoctor(h, nombre, apellido, cedula, especialidad, experiencia, costo);
                if (d) delete d; // Liberar memoria temporal
                break;
            }
            case 2:
                listarDoctores(h);
                break;
            case 3: {
                // Implementar búsqueda por especialidad
                cout << "Funcionalidad en desarrollo..." << endl;
                break;
            }
            case 4: {
                // Implementar asignación
                cout << "Funcionalidad en desarrollo..." << endl;
                break;
            }
            case 5: {
                // Implementar listado de pacientes
                cout << "Funcionalidad en desarrollo..." << endl;
                break;
            }
            case 6: {
                // Implementar eliminación
                cout << "Funcionalidad en desarrollo..." << endl;
                break;
            }
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while (opcion != 0);
}

void mostrarDatosHospital(Hospital* h) {
    ArchivoHeader headerPac = leerHeader(ARCHIVO_PACIENTES);
    ArchivoHeader headerDoc = leerHeader(ARCHIVO_DOCTORES);
    ArchivoHeader headerCit = leerHeader(ARCHIVO_CITAS);
    
    cout << "=== INFORMACION DEL HOSPITAL ===" << endl;
    cout << "Nombre: " << h->nombre << endl;
    cout << "Direccion: " << h->direccion << endl;
    cout << "Telefono: " << h->telefono << endl;
    cout << "Pacientes registrados: " << headerPac.registrosActivos << endl;
    cout << "Doctores registrados: " << headerDoc.registrosActivos << endl;
    cout << "Citas registradas: " << headerCit.registrosActivos << endl;
    cout << "=================================" << endl;
}

// ===================== FUNCION PRINCIPAL MODIFICADA =====================

int main() {
    cout << "SISTEMA DE GESTIÓN HOSPITALARIA v2 (PERSISTENCIA)" << endl;
    cout << "Cargando sistema desde archivos..." << endl;

    Hospital* hospital = cargarSistema();

    int opcion;
    do {
        cout << endl;
        cout << "=== MENU PRINCIPAL ===" << endl;
        cout << "1. Gestion de Pacientes" << endl;
        cout << "2. Gestion de Doctores" << endl;
        cout << "3. Gestion de Citas" << endl;
        cout << "4. Informacion del Hospital" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1:
                menuPacientes(hospital);
                break;
            case 2:
                menuDoctores(hospital);
                break;
            case 3:
                cout << "Modulo de citas en desarrollo..." << endl;
                break;
            case 4:
                mostrarDatosHospital(hospital);
                break;
            case 0:
                cout << "Guardando y saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while (opcion != 0);

    limpiarSistema(hospital);
    return 0;
}