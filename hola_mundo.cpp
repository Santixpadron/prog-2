// tarea_santiago_padron_completo.cpp
// Version extendida del sistema de gestion hospitalaria
// Se han añadido utilidades y funciones faltantes solicitadas
// Reglas: sin acentos en los comentarios, uso de endl en lugar de '\\n'

#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>

using namespace std;

// -------------------- ESTRUCTURAS --------------------
struct HistorialMedico {
    int idConsulta;
    char fecha[11];
    char hora[6];
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costo;
};

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

    HistorialMedico* historial;
    int cantidadConsultas;
    int capacidadHistorial;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    char alergias[500];
    char observaciones[500];
    bool activo;
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

    int* pacientesAsignados;
    int cantidadPacientes;
    int capacidadPacientes;

    int* citasAgendadas;
    int cantidadCitas;
    int capacidadCitas;

    bool disponible;
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
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];

    Paciente* pacientes;
    int cantidadPacientes;
    int capacidadPacientes;

    Doctor* doctores;
    int cantidadDoctores;
    int capacidadDoctores;

    Cita* citas;
    int cantidadCitas;
    int capacidadCitas;

    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;
};

// -------------------- CONSTANTES --------------------
const int CAP_PACIENTES_INICIAL = 10;
const int CAP_DOCTORES_INICIAL = 10;
const int CAP_CITAS_INICIAL = 20;
const int CAP_HISTORIAL_INICIAL = 5;
const int CAP_CITAS_PACIENTE_INICIAL = 5;
const int CAP_PACIENTOS_DOCTOR_INICIAL = 5;
const int CAP_CITAS_DOCTOR_INICIAL = 10;

// -------------------- PROTOTIPOS --------------------
// utilidades y entrada
void limpiarBufferEntrada();
void leerLinea(char* buffer, int size);
int leerEntero();
char* copiarString(const char* origen);

// validaciones
bool validarCedula(const char* cedula);
bool validarEmail(const char* email);
bool validarFecha(const char* fecha);
bool validarHora(const char* hora);
int compararFechas(const char* fecha1, const char* fecha2);

// redimensionadores / memoria
void redimensionarArrayPacientes(Hospital* hospital);
void redimensionarArrayDoctores(Hospital* hospital);
void redimensionarArrayCitas(Hospital* hospital);
Hospital* inicializarHospital(const char* nombre, int capacidadInicial);
void destruirHospital(Hospital* hospital);

// pacientes
Paciente* crearPaciente(Hospital* hospital, const char* nombre, const char* apellido,
                        const char* cedula, int edad, char sexo);
Paciente* buscarPacientePorCedula(Hospital* hospital, const char* cedula);
Paciente* buscarPacientePorId(Hospital* hospital, int id);
Paciente** buscarPacientesPorNombre(Hospital* hospital, const char* nombre, int* cantidad);
bool actualizarPaciente(Hospital* hospital, int id);
bool eliminarPaciente(Hospital* hospital, int id);
void listarPacientes(Hospital* hospital);

// historial
void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico consulta);
HistorialMedico* obtenerHistorialCompleto(Paciente* paciente, int* cantidad);
void mostrarHistorialMedico(Paciente* paciente);
HistorialMedico* obtenerUltimaConsulta(Paciente* paciente);

// doctores
Doctor* crearDoctor(Hospital* hospital, const char* nombre, const char* apellido,
                    const char* cedula, const char* especialidad, int aniosExperiencia,
                    float costoConsulta);
Doctor* buscarDoctorPorId(Hospital* hospital, int id);
Doctor* buscarDoctorPorCedula(Hospital* hospital, const char* cedula);
Doctor** buscarDoctoresPorEspecialidad(Hospital* hospital, const char* especialidad, int* cantidad);
Doctor** buscarDoctoresPorNombre(Hospital* hospital, const char* nombre, int* cantidad);
bool asignarPacienteADoctor(Doctor* doctor, int idPaciente);
bool removerPacienteDeDoctor(Doctor* doctor, int idPaciente);
void listarPacientesDeDoctor(Hospital* hospital, int idDoctor);
void listarDoctores(Hospital* hospital);
bool eliminarDoctor(Hospital* hospital, int id);

// citas
Cita* agendarCita(Hospital* hospital, int idPaciente, int idDoctor,
                  const char* fecha, const char* hora, const char* motivo);
bool cancelarCita(Hospital* hospital, int idCita);
bool atenderCita(Hospital* hospital, int idCita, const char* diagnostico,
                 const char* tratamiento, const char* medicamentos);
Cita** obtenerCitasDePaciente(Hospital* hospital, int idPaciente, int* cantidad);
Cita** obtenerCitasDeDoctor(Hospital* hospital, int idDoctor, int* cantidad);
Cita** obtenerCitasPorFecha(Hospital* hospital, const char* fecha, int* cantidad);
void listarCitasPendientes(Hospital* hospital);
bool verificarDisponibilidad(Hospital* hospital, int idDoctor, const char* fecha, const char* hora);

// menus
void menuPacientes(Hospital* h);
void menuDoctores(Hospital* h);
void menuCitas(Hospital* h);

// otras utilidades agregadas
void mostrarDatosHospital(Hospital* h);

// -------------------- IMPLEMENTACIONES --------------------

// utilidades de entrada
// limpia hasta 256 caracteres o hasta fin de linea
void limpiarBufferEntrada() {
    cin.ignore(256, '\n');
}

// lee una linea segura en buffer con tamano size
void leerLinea(char* buffer, int size) {
    cin.getline(buffer, size);
    if (cin.fail()) {
        cin.clear();
        limpiarBufferEntrada();
    }
    buffer[size-1] = '\0';
}

// lee un entero y protege el flujo
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

// copia dinamica de string
char* copiarString(const char* origen) {
    if (!origen) return nullptr;
    int longitud = (int)strlen(origen);
    char* copia = new char[longitud + 1];
    strcpy(copia, origen);
    return copia;
}

// validaciones
bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    int len = (int)strlen(cedula);
    if (len < 3 || len > 20) return false;
    // formato esperado: letra '-' y numeros. ej: V-12345678
    if (cedula[1] != '-') return false;
    if (cedula[0] != 'V' && cedula[0] != 'v' && cedula[0] != 'E' && cedula[0] != 'e') return false;
    for (int i = 2; i < len; i++) {
        if (!isdigit((unsigned char)cedula[i])) return false;
    }
    return true;
}

bool validarEmail(const char* email) {
    if (!email) return false;
    int len = (int)strlen(email);
    if (len == 0 || len > 50) return false;
    const char* at = strchr(email, '@');
    if (!at) return false;
    if (at == email) return false;
    if (strchr(at + 1, '@')) return false;
    const char* dot = strchr(at + 1, '.');
    if (!dot) return false;
    if (dot == at + 1) return false;
    const char* lastDot = strrchr(email, '.');
    if (!lastDot || strlen(lastDot) < 3) return false;
    // comprobar caracteres permitidos
    for (const char* p = email; p < at; p++) {
        if (!isalnum((unsigned char)*p) && *p != '.' && *p != '_' && *p != '-' && *p != '+') return false;
    }
    for (const char* p = at + 1; *p; p++) {
        if (!isalnum((unsigned char)*p) && *p != '.' && *p != '-') return false;
    }
    // no permitir doble punto
    for (const char* p = email; *p && *(p+1); p++) {
        if (*p == '.' && *(p+1) == '.') return false;
    }
    return true;
}

bool validarFecha(const char* fecha) {
    if (!fecha) return false;
    if (strlen(fecha) != 10) return false;
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (fecha[i] != '-') return false;
        } else {
            if (!isdigit((unsigned char)fecha[i])) return false;
        }
    }
    int y = (fecha[0]-'0')*1000 + (fecha[1]-'0')*100 + (fecha[2]-'0')*10 + (fecha[3]-'0');
    int m = (fecha[5]-'0')*10 + (fecha[6]-'0');
    int d = (fecha[8]-'0')*10 + (fecha[9]-'0');
    if (m < 1 || m > 12) return false;
    int diasMes = 31;
    if (m == 4 || m == 6 || m == 9 || m == 11) diasMes = 30;
    else if (m == 2) {
        bool bisiesto = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        diasMes = bisiesto ? 29 : 28;
    }
    if (d < 1 || d > diasMes) return false;
    return true;
}

bool validarHora(const char* hora) {
    if (!hora) return false;
    if (strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit((unsigned char)hora[i])) return false;
    }
    int hh = (hora[0]-'0')*10 + (hora[1]-'0');
    int mm = (hora[3]-'0')*10 + (hora[4]-'0');
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    return true;
}

int compararFechas(const char* fecha1, const char* fecha2) {
    int cmp = strcmp(fecha1, fecha2);
    if (cmp < 0) return -1;
    if (cmp > 0) return 1;
    return 0;
}

// redimensionadores
void redimensionarArrayPacientes(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadPacientes * 2;
    Paciente* nuevoArray = new Paciente[nuevaCapacidad];
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        nuevoArray[i] = hospital->pacientes[i];
    }
    for (int i = hospital->cantidadPacientes; i < nuevaCapacidad; i++) {
        nuevoArray[i].historial = nullptr;
        nuevoArray[i].citasAgendadas = nullptr;
        nuevoArray[i].activo = false;
        nuevoArray[i].cantidadConsultas = 0;
        nuevoArray[i].capacidadHistorial = 0;
        nuevoArray[i].cantidadCitas = 0;
        nuevoArray[i].capacidadCitas = 0;
    }
    delete[] hospital->pacientes;
    hospital->pacientes = nuevoArray;
    hospital->capacidadPacientes = nuevaCapacidad;
}

void redimensionarArrayDoctores(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadDoctores * 2;
    Doctor* nuevoArray = new Doctor[nuevaCapacidad];
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        nuevoArray[i] = hospital->doctores[i];
    }
    for (int i = hospital->cantidadDoctores; i < nuevaCapacidad; i++) {
        nuevoArray[i].pacientesAsignados = nullptr;
        nuevoArray[i].citasAgendadas = nullptr;
        nuevoArray[i].disponible = true;
        nuevoArray[i].cantidadPacientes = 0;
        nuevoArray[i].capacidadPacientes = 0;
        nuevoArray[i].cantidadCitas = 0;
        nuevoArray[i].capacidadCitas = 0;
    }
    delete[] hospital->doctores;
    hospital->doctores = nuevoArray;
    hospital->capacidadDoctores = nuevaCapacidad;
}

void redimensionarArrayCitas(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadCitas * 2;
    Cita* nuevoArray = new Cita[nuevaCapacidad];
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        nuevoArray[i] = hospital->citas[i];
    }
    for (int i = hospital->cantidadCitas; i < nuevaCapacidad; i++) {
        nuevoArray[i].id = 0;
        nuevoArray[i].idPaciente = 0;
        nuevoArray[i].idDoctor = 0;
        nuevoArray[i].fecha[0] = '\0';
        nuevoArray[i].hora[0] = '\0';
        nuevoArray[i].motivo[0] = '\0';
        nuevoArray[i].estado[0] = '\0';
        nuevoArray[i].observaciones[0] = '\0';
        nuevoArray[i].atendida = false;
    }
    delete[] hospital->citas;
    hospital->citas = nuevoArray;
    hospital->capacidadCitas = nuevaCapacidad;
}

// inicializar / destruir
Hospital* inicializarHospital(const char* nombre, int capacidadInicial) {
    Hospital* hospital = new Hospital;
    memset(hospital, 0, sizeof(Hospital));
    strncpy(hospital->nombre, nombre, sizeof(hospital->nombre) - 1);
    hospital->nombre[sizeof(hospital->nombre) - 1] = '\0';
    strncpy(hospital->direccion, "Sin direccion", sizeof(hospital->direccion) - 1);
    hospital->direccion[sizeof(hospital->direccion) - 1] = '\0';
    strncpy(hospital->telefono, "0000000000", sizeof(hospital->telefono) - 1);
    hospital->telefono[sizeof(hospital->telefono) - 1] = '\0';

    hospital->capacidadPacientes = CAP_PACIENTES_INICIAL;
    hospital->capacidadDoctores = CAP_DOCTORES_INICIAL;
    hospital->capacidadCitas = CAP_CITAS_INICIAL;

    hospital->cantidadPacientes = 0;
    hospital->cantidadDoctores = 0;
    hospital->cantidadCitas = 0;

    hospital->pacientes = new Paciente[hospital->capacidadPacientes];
    for (int i = 0; i < hospital->capacidadPacientes; i++) {
        hospital->pacientes[i].historial = nullptr;
        hospital->pacientes[i].citasAgendadas = nullptr;
        hospital->pacientes[i].activo = false;
        hospital->pacientes[i].cantidadConsultas = 0;
        hospital->pacientes[i].capacidadHistorial = 0;
        hospital->pacientes[i].cantidadCitas = 0;
        hospital->pacientes[i].capacidadCitas = 0;
    }

    hospital->doctores = new Doctor[hospital->capacidadDoctores];
    for (int i = 0; i < hospital->capacidadDoctores; i++) {
        hospital->doctores[i].pacientesAsignados = nullptr;
        hospital->doctores[i].citasAgendadas = nullptr;
        hospital->doctores[i].disponible = true;
        hospital->doctores[i].cantidadPacientes = 0;
        hospital->doctores[i].capacidadPacientes = 0;
        hospital->doctores[i].cantidadCitas = 0;
        hospital->doctores[i].capacidadCitas = 0;
    }

    hospital->citas = new Cita[hospital->capacidadCitas];
    for (int i = 0; i < hospital->capacidadCitas; i++) {
        hospital->citas[i].id = 0;
        hospital->citas[i].fecha[0] = '\0';
        hospital->citas[i].hora[0] = '\0';
        hospital->citas[i].motivo[0] = '\0';
        hospital->citas[i].estado[0] = '\0';
        hospital->citas[i].observaciones[0] = '\0';
        hospital->citas[i].atendida = false;
    }

    hospital->siguienteIdPaciente = 1;
    hospital->siguienteIdDoctor = 1;
    hospital->siguienteIdCita = 1;
    hospital->siguienteIdConsulta = 1;

    cout << "Hospital inicializado correctamente" << endl;
    return hospital;
}

void destruirHospital(Hospital* hospital) {
    if (!hospital) return;

    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].historial) {
            delete[] hospital->pacientes[i].historial;
            hospital->pacientes[i].historial = nullptr;
        }
        if (hospital->pacientes[i].citasAgendadas) {
            delete[] hospital->pacientes[i].citasAgendadas;
            hospital->pacientes[i].citasAgendadas = nullptr;
        }
    }
    if (hospital->pacientes) {
        delete[] hospital->pacientes;
        hospital->pacientes = nullptr;
    }

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].pacientesAsignados) {
            delete[] hospital->doctores[i].pacientesAsignados;
            hospital->doctores[i].pacientesAsignados = nullptr;
        }
        if (hospital->doctores[i].citasAgendadas) {
            delete[] hospital->doctores[i].citasAgendadas;
            hospital->doctores[i].citasAgendadas = nullptr;
        }
    }
    if (hospital->doctores) {
        delete[] hospital->doctores;
        hospital->doctores = nullptr;
    }

    if (hospital->citas) {
        delete[] hospital->citas;
        hospital->citas = nullptr;
    }

    delete hospital;
    cout << "Hospital destruido y memoria liberada" << endl;
}

// -------------------- PACIENTES --------------------
Paciente* buscarPacientePorCedula(Hospital* hospital, const char* cedula) {
    if (!hospital || !cedula) return nullptr;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strcmp(hospital->pacientes[i].cedula, cedula) == 0)
            return &hospital->pacientes[i];
    }
    return nullptr;
}

Paciente* buscarPacientePorId(Hospital* hospital, int id) {
    if (!hospital) return nullptr;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) return &hospital->pacientes[i];
    }
    return nullptr;
}

Paciente* crearPaciente(Hospital* hospital, const char* nombre, const char* apellido,
                        const char* cedula, int edad, char sexo) {
    if (!hospital || !nombre || !apellido || !cedula) return nullptr;
    if (!validarCedula(cedula)) {
        cout << "Cedula invalida" << endl;
        return nullptr;
    }
    if (buscarPacientePorCedula(hospital, cedula)) {
        cout << "Ya existe un paciente con esa cedula" << endl;
        return nullptr;
    }
    if (hospital->cantidadPacientes >= hospital->capacidadPacientes)
        redimensionarArrayPacientes(hospital);

    Paciente& p = hospital->pacientes[hospital->cantidadPacientes];
    p.id = hospital->siguienteIdPaciente++;
    strncpy(p.nombre, nombre, sizeof(p.nombre)-1);
    p.nombre[sizeof(p.nombre)-1] = '\0';
    strncpy(p.apellido, apellido, sizeof(p.apellido)-1);
    p.apellido[sizeof(p.apellido)-1] = '\0';
    strncpy(p.cedula, cedula, sizeof(p.cedula)-1);
    p.cedula[sizeof(p.cedula)-1] = '\0';
    p.edad = edad;
    p.sexo = sexo;
    strncpy(p.tipoSangre, "O+", sizeof(p.tipoSangre)-1);
    p.tipoSangre[sizeof(p.tipoSangre)-1] = '\0';
    strncpy(p.telefono, "0000000000", sizeof(p.telefono)-1);
    p.telefono[sizeof(p.telefono)-1] = '\0';
    strncpy(p.direccion, "Sin direccion", sizeof(p.direccion)-1);
    p.direccion[sizeof(p.direccion)-1] = '\0';
    strncpy(p.email, "noemail@hospital.com", sizeof(p.email)-1);
    p.email[sizeof(p.email)-1] = '\0';

    p.capacidadHistorial = CAP_HISTORIAL_INICIAL;
    p.cantidadConsultas = 0;
    p.historial = new HistorialMedico[p.capacidadHistorial];

    p.capacidadCitas = CAP_CITAS_PACIENTE_INICIAL;
    p.cantidadCitas = 0;
    p.citasAgendadas = new int[p.capacidadCitas];

    p.alergias[0] = '\0';
    p.observaciones[0] = '\0';
    p.activo = true;

    hospital->cantidadPacientes++;
    cout << "Paciente creado correctamente con ID: " << p.id << endl;
    return &p;
}

Paciente** buscarPacientesPorNombre(Hospital* hospital, const char* nombre, int* cantidad) {
    *cantidad = 0;
    if (!hospital || !nombre) return nullptr;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strstr(hospital->pacientes[i].nombre, nombre) != nullptr) (*cantidad)++;
        else if (strstr(hospital->pacientes[i].apellido, nombre) != nullptr) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Paciente** resultados = new Paciente*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strstr(hospital->pacientes[i].nombre, nombre) != nullptr ||
            strstr(hospital->pacientes[i].apellido, nombre) != nullptr) {
            resultados[idx++] = &hospital->pacientes[i];
        }
    }
    return resultados;
}

bool actualizarPaciente(Hospital* hospital, int id) {
    Paciente* p = buscarPacientePorId(hospital, id);
    if (!p) return false;
    char buffer[200];
    cout << "Actualizar nombre (enter para mantener): " << endl;
    limpiarBufferEntrada();
    leerLinea(buffer,200);
    if (strlen(buffer) > 0) strncpy(p->nombre, buffer, sizeof(p->nombre)-1);
    cout << "Actualizar apellido (enter para mantener): " << endl;
    leerLinea(buffer,200);
    if (strlen(buffer) > 0) strncpy(p->apellido, buffer, sizeof(p->apellido)-1);
    cout << "Actualizar telefono (enter para mantener): " << endl;
    leerLinea(buffer,200);
    if (strlen(buffer) > 0) strncpy(p->telefono, buffer, sizeof(p->telefono)-1);
    cout << "Datos actualizados" << endl;
    return true;
}

bool eliminarPaciente(Hospital* hospital, int id) {
    if (!hospital) return false;
    int index = -1;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) return false;

    if (hospital->pacientes[index].historial) {
        delete[] hospital->pacientes[index].historial;
        hospital->pacientes[index].historial = nullptr;
    }
    if (hospital->pacientes[index].citasAgendadas) {
        delete[] hospital->pacientes[index].citasAgendadas;
        hospital->pacientes[index].citasAgendadas = nullptr;
    }

    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idPaciente == id) {
            strcpy(hospital->citas[i].estado, "Cancelada");
            hospital->citas[i].atendida = false;
        }
    }

    for (int d = 0; d < hospital->cantidadDoctores; d++) {
        removerPacienteDeDoctor(&hospital->doctores[d], id);
    }

    for (int i = index; i < hospital->cantidadPacientes - 1; i++) {
        hospital->pacientes[i] = hospital->pacientes[i + 1];
    }
    hospital->cantidadPacientes--;
    cout << "Paciente eliminado correctamente" << endl;
    return true;
}

void listarPacientes(Hospital* hospital) {
    cout << "╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║                      LISTA DE PACIENTES                    ║" << endl;
    cout << "╠════╦══════════════════════════════╦════════════╦═════╦══════╣" << endl;
    cout << "║ ID ║ NOMBRE COMPLETO              ║ CEDULA     ║ EDAD║CONS.║" << endl;
    cout << "╠════╬══════════════════════════════╬════════════╬═════╬══════╣" << endl;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        char fullname[101];
        fullname[0] = '\0';
        strncat(fullname, p.nombre, sizeof(fullname)-1);
        strncat(fullname, " ", sizeof(fullname)-strlen(fullname)-1);
        strncat(fullname, p.apellido, sizeof(fullname)-strlen(fullname)-1);

        cout << "║ " << setw(2) << p.id << " ║ "
             << left << setw(28) << fullname
             << "║ " << setw(10) << p.cedula
             << "║ " << setw(4) << p.edad
             << "║ " << setw(5) << p.cantidadConsultas << "║" << endl;
    }
    cout << "╚════╩══════════════════════════════╩════════════╩═════╩══════╝" << endl;
}

// -------------------- HISTORIAL --------------------
void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico consulta) {
    if (!paciente) return;
    if (paciente->historial == nullptr) {
        paciente->capacidadHistorial = CAP_HISTORIAL_INICIAL;
        paciente->historial = new HistorialMedico[paciente->capacidadHistorial];
        paciente->cantidadConsultas = 0;
    }
    if (paciente->cantidadConsultas >= paciente->capacidadHistorial) {
        int nuevaCap = paciente->capacidadHistorial * 2;
        HistorialMedico* nuevo = new HistorialMedico[nuevaCap];
        for (int i = 0; i < paciente->cantidadConsultas; i++) {
            nuevo[i] = paciente->historial[i];
        }
        delete[] paciente->historial;
        paciente->historial = nuevo;
        paciente->capacidadHistorial = nuevaCap;
    }
    paciente->historial[paciente->cantidadConsultas++] = consulta;
}

HistorialMedico* obtenerHistorialCompleto(Paciente* paciente, int* cantidad) {
    if (!paciente) { *cantidad = 0; return nullptr; }
    *cantidad = paciente->cantidadConsultas;
    return paciente->historial;
}

void mostrarHistorialMedico(Paciente* paciente) {
    if (!paciente) return;
    cout << "Historial medico de " << paciente->nombre << " " << paciente->apellido << endl;
    for (int i = 0; i < paciente->cantidadConsultas; i++) {
        HistorialMedico& h = paciente->historial[i];
        cout << "Consulta #" << h.idConsulta << " - Fecha: " << h.fecha << " - Hora: " << h.hora << " - Diagnostico: " << h.diagnostico << endl;
    }
}

HistorialMedico* obtenerUltimaConsulta(Paciente* paciente) {
    if (!paciente || paciente->cantidadConsultas == 0) return nullptr;
    return &paciente->historial[paciente->cantidadConsultas - 1];
}

// -------------------- DOCTORES --------------------
Doctor* crearDoctor(Hospital* hospital, const char* nombre, const char* apellido,
                    const char* cedula, const char* especialidad, int aniosExperiencia,
                    float costoConsulta) {
    if (!hospital || !nombre || !apellido || !cedula || !especialidad) return nullptr;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].cedula, cedula) == 0) {
            cout << "Cedula profesional ya registrada" << endl;
            return nullptr;
        }
    }
    if (hospital->cantidadDoctores >= hospital->capacidadDoctores)
        redimensionarArrayDoctores(hospital);

    Doctor& d = hospital->doctores[hospital->cantidadDoctores];
    d.id = hospital->siguienteIdDoctor++;
    strncpy(d.nombre, nombre, sizeof(d.nombre)-1);
    d.nombre[sizeof(d.nombre)-1] = '\0';
    strncpy(d.apellido, apellido, sizeof(d.apellido)-1);
    d.apellido[sizeof(d.apellido)-1] = '\0';
    strncpy(d.cedula, cedula, sizeof(d.cedula)-1);
    d.cedula[sizeof(d.cedula)-1] = '\0';
    strncpy(d.especialidad, especialidad, sizeof(d.especialidad)-1);
    d.especialidad[sizeof(d.especialidad)-1] = '\0';
    d.aniosExperiencia = (aniosExperiencia < 0 ? 0 : aniosExperiencia);
    d.costoConsulta = (costoConsulta <= 0.0f ? 1.0f : costoConsulta);
    strncpy(d.horarioAtencion, "Lun-Vie 8:00-16:00", sizeof(d.horarioAtencion)-1);
    d.horarioAtencion[sizeof(d.horarioAtencion)-1] = '\0';
    strncpy(d.telefono, "0000000000", sizeof(d.telefono)-1);
    d.telefono[sizeof(d.telefono)-1] = '\0';
    strncpy(d.email, "doctor@hospital.com", sizeof(d.email)-1);
    d.email[sizeof(d.email)-1] = '\0';

    d.capacidadPacientes = CAP_PACIENTOS_DOCTOR_INICIAL;
    d.cantidadPacientes = 0;
    d.pacientesAsignados = new int[d.capacidadPacientes];

    d.capacidadCitas = CAP_CITAS_DOCTOR_INICIAL;
    d.cantidadCitas = 0;
    d.citasAgendadas = new int[d.capacidadCitas];

    d.disponible = true;

    hospital->cantidadDoctores++;
    cout << "Doctor creado correctamente con ID: " << d.id << endl;
    return &d;
}

Doctor* buscarDoctorPorId(Hospital* hospital, int id) {
    if (!hospital) return nullptr;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) return &hospital->doctores[i];
    }
    return nullptr;
}

Doctor* buscarDoctorPorCedula(Hospital* hospital, const char* cedula) {
    if (!hospital || !cedula) return nullptr;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].cedula, cedula) == 0) return &hospital->doctores[i];
    }
    return nullptr;
}

Doctor** buscarDoctoresPorEspecialidad(Hospital* hospital, const char* especialidad, int* cantidad) {
    *cantidad = 0;
    if (!hospital || !especialidad) return nullptr;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].especialidad, especialidad) == 0) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Doctor** resultados = new Doctor*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strcmp(hospital->doctores[i].especialidad, especialidad) == 0) {
            resultados[idx++] = &hospital->doctores[i];
        }
    }
    return resultados;
}

Doctor** buscarDoctoresPorNombre(Hospital* hospital, const char* nombre, int* cantidad) {
    *cantidad = 0;
    if (!hospital || !nombre) return nullptr;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        char full[120];
        full[0] = '\0';
        strncat(full, hospital->doctores[i].nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-strlen(full)-1);
        strncat(full, hospital->doctores[i].apellido, sizeof(full)-strlen(full)-1);
        if (strstr(full, nombre) != nullptr) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Doctor** resultados = new Doctor*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        char full[120];
        full[0] = '\0';
        strncat(full, hospital->doctores[i].nombre, sizeof(full)-1);
        strncat(full, " ", sizeof(full)-strlen(full)-1);
        strncat(full, hospital->doctores[i].apellido, sizeof(full)-strlen(full)-1);
        if (strstr(full, nombre) != nullptr) {
            resultados[idx++] = &hospital->doctores[i];
        }
    }
    return resultados;
}

bool asignarPacienteADoctor(Doctor* doctor, int idPaciente) {
    if (!doctor) return false;
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente)
            return false;
    }
    if (doctor->cantidadPacientes >= doctor->capacidadPacientes) {
        int nuevaCap = doctor->capacidadPacientes * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < doctor->cantidadPacientes; i++) nuevo[i] = doctor->pacientesAsignados[i];
        delete[] doctor->pacientesAsignados;
        doctor->pacientesAsignados = nuevo;
        doctor->capacidadPacientes = nuevaCap;
    }
    doctor->pacientesAsignados[doctor->cantidadPacientes++] = idPaciente;
    return true;
}

bool removerPacienteDeDoctor(Doctor* doctor, int idPaciente) {
    if (!doctor || doctor->cantidadPacientes == 0) return false;
    int idx = -1;
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente) { idx = i; break; }
    }
    if (idx == -1) return false;
    for (int i = idx; i < doctor->cantidadPacientes - 1; i++) {
        doctor->pacientesAsignados[i] = doctor->pacientesAsignados[i + 1];
    }
    doctor->cantidadPacientes--;
    return true;
}

void listarPacientesDeDoctor(Hospital* hospital, int idDoctor) {
    Doctor* d = buscarDoctorPorId(hospital, idDoctor);
    if (!d) { cout << "Doctor no encontrado" << endl; return; }
    cout << "Pacientes asignados al Dr. " << d->nombre << " " << d->apellido << endl;
    for (int i = 0; i < d->cantidadPacientes; i++) {
        Paciente* p = buscarPacientePorId(hospital, d->pacientesAsignados[i]);
        if (p) cout << "ID: " << p->id << " - " << p->nombre << " " << p->apellido << endl;
    }
}

void listarDoctores(Hospital* hospital) {
    cout << "╔════════════════════════════════════════════════════╗" << endl;
    cout << "║                  LISTA DE DOCTORES                ║" << endl;
    cout << "╠════╦══════════════════════════════╦════════════╦═══════╣" << endl;
    cout << "║ ID ║ NOMBRE COMPLETO              ║ CEDULA     ║ ESPEC.║" << endl;
    cout << "╠════╬══════════════════════════════╬════════════╬═══════╣" << endl;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        char fullname[101];
        fullname[0] = '\0';
        strncat(fullname, d.nombre, sizeof(fullname)-1);
        strncat(fullname, " ", sizeof(fullname)-strlen(fullname)-1);
        strncat(fullname, d.apellido, sizeof(fullname)-strlen(fullname)-1);

        cout << "║ " << setw(2) << d.id << " ║ "
             << left << setw(28) << fullname
             << "║ " << setw(10) << d.cedula
             << "║ " << setw(6) << d.especialidad << "║" << endl;
    }
    cout << "╚════╩══════════════════════════════╩════════════╩═══════╝" << endl;
}

bool eliminarDoctor(Hospital* hospital, int id) {
    if (!hospital) return false;
    int index = -1;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) { index = i; break; }
    }
    if (index == -1) return false;

    if (hospital->doctores[index].pacientesAsignados) {
        delete[] hospital->doctores[index].pacientesAsignados;
        hospital->doctores[index].pacientesAsignados = nullptr;
    }
    if (hospital->doctores[index].citasAgendadas) {
        delete[] hospital->doctores[index].citasAgendadas;
        hospital->doctores[index].citasAgendadas = nullptr;
    }

    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == id) {
            strcpy(hospital->citas[i].estado, "Cancelada");
            hospital->citas[i].atendida = false;
        }
    }

    for (int i = index; i < hospital->cantidadDoctores - 1; i++) {
        hospital->doctores[i] = hospital->doctores[i + 1];
    }
    hospital->cantidadDoctores--;
    cout << "Doctor eliminado correctamente" << endl;
    return true;
}

// -------------------- CITAS --------------------
bool verificarDisponibilidad(Hospital* hospital, int idDoctor, const char* fecha, const char* hora) {
    if (!hospital) return false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor &&
            strcmp(hospital->citas[i].fecha, fecha) == 0 &&
            strcmp(hospital->citas[i].hora, hora) == 0 &&
            strcmp(hospital->citas[i].estado, "Agendada") == 0) {
            return false;
        }
    }
    return true;
}

Cita* agendarCita(Hospital* hospital, int idPaciente, int idDoctor,
                  const char* fecha, const char* hora, const char* motivo) {
    if (!hospital) return nullptr;
    if (!validarFecha(fecha) || !validarHora(hora)) {
        cout << "Fecha u hora invalida" << endl;
        return nullptr;
    }
    Paciente* p = buscarPacientePorId(hospital, idPaciente);
    Doctor* d = buscarDoctorPorId(hospital, idDoctor);
    if (!p) { cout << "Paciente no encontrado" << endl; return nullptr; }
    if (!d) { cout << "Doctor no encontrado" << endl; return nullptr; }
    if (!verificarDisponibilidad(hospital, idDoctor, fecha, hora)) {
        cout << "El doctor no esta disponible en ese horario" << endl;
        return nullptr;
    }
    if (hospital->cantidadCitas >= hospital->capacidadCitas)
        redimensionarArrayCitas(hospital);

    Cita& c = hospital->citas[hospital->cantidadCitas];
    c.id = hospital->siguienteIdCita++;
    c.idPaciente = idPaciente;
    c.idDoctor = idDoctor;
    strncpy(c.fecha, fecha, sizeof(c.fecha)-1);
    c.fecha[sizeof(c.fecha)-1] = '\0';
    strncpy(c.hora, hora, sizeof(c.hora)-1);
    c.hora[sizeof(c.hora)-1] = '\0';
    strncpy(c.motivo, motivo, sizeof(c.motivo)-1);
    c.motivo[sizeof(c.motivo)-1] = '\0';
    strncpy(c.estado, "Agendada", sizeof(c.estado)-1);
    c.estado[sizeof(c.estado)-1] = '\0';
    c.observaciones[0] = '\0';
    c.atendida = false;

    if (p->cantidadCitas >= p->capacidadCitas) {
        int nuevaCap = p->capacidadCitas * 2;
        if (nuevaCap <= 0) nuevaCap = CAP_CITAS_PACIENTE_INICIAL;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < p->cantidadCitas; i++) nuevo[i] = p->citasAgendadas[i];
        delete[] p->citasAgendadas;
        p->citasAgendadas = nuevo;
        p->capacidadCitas = nuevaCap;
    }
    p->citasAgendadas[p->cantidadCitas++] = c.id;

    if (d->cantidadCitas >= d->capacidadCitas) {
        int nuevaCap = d->capacidadCitas * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < d->cantidadCitas; i++) nuevo[i] = d->citasAgendadas[i];
        delete[] d->citasAgendadas;
        d->citasAgendadas = nuevo;
        d->capacidadCitas = nuevaCap;
    }
    d->citasAgendadas[d->cantidadCitas++] = c.id;

    hospital->cantidadCitas++;
    cout << "Cita agendada correctamente con ID: " << c.id << endl;
    return &c;
}

Cita* buscarCitaPorId(Hospital* hospital, int id) {
    if (!hospital) return nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == id) return &hospital->citas[i];
    }
    return nullptr;
}

bool cancelarCita(Hospital* hospital, int idCita) {
    Cita* c = buscarCitaPorId(hospital, idCita);
    if (!c) return false;
    strncpy(c->estado, "Cancelada", sizeof(c->estado)-1);
    c->estado[sizeof(c->estado)-1] = '\0';
    c->atendida = false;

    Paciente* p = buscarPacientePorId(hospital, c->idPaciente);
    Doctor* d = buscarDoctorPorId(hospital, c->idDoctor);
    if (p) {
        for (int i = 0; i < p->cantidadCitas; i++) {
            if (p->citasAgendadas[i] == idCita) {
                for (int j = i; j < p->cantidadCitas - 1; j++) p->citasAgendadas[j] = p->citasAgendadas[j+1];
                p->cantidadCitas--;
                break;
            }
        }
    }
    if (d) {
        for (int i = 0; i < d->cantidadCitas; i++) {
            if (d->citasAgendadas[i] == idCita) {
                for (int j = i; j < d->cantidadCitas - 1; j++) d->citasAgendadas[j] = d->citasAgendadas[j+1];
                d->cantidadCitas--;
                break;
            }
        }
    }

    cout << "Cita cancelada correctamente" << endl;
    return true;
}

bool atenderCita(Hospital* hospital, int idCita, const char* diagnostico,
                 const char* tratamiento, const char* medicamentos) {
    Cita* cita = buscarCitaPorId(hospital, idCita);
    if (!cita) return false;
    if (strcmp(cita->estado, "Agendada") != 0) return false;

    Paciente* p = buscarPacientePorId(hospital, cita->idPaciente);
    Doctor* d = buscarDoctorPorId(hospital, cita->idDoctor);
    if (!p || !d) return false;

    HistorialMedico h;
    h.idConsulta = hospital->siguienteIdConsulta++;
    strncpy(h.fecha, cita->fecha, sizeof(h.fecha)-1);
    h.fecha[sizeof(h.fecha)-1] = '\0';
    strncpy(h.hora, cita->hora, sizeof(h.hora)-1);
    h.hora[sizeof(h.hora)-1] = '\0';
    strncpy(h.diagnostico, diagnostico, sizeof(h.diagnostico)-1);
    h.diagnostico[sizeof(h.diagnostico)-1] = '\0';
    strncpy(h.tratamiento, tratamiento, sizeof(h.tratamiento)-1);
    h.tratamiento[sizeof(h.tratamiento)-1] = '\0';
    strncpy(h.medicamentos, medicamentos, sizeof(h.medicamentos)-1);
    h.medicamentos[sizeof(h.medicamentos)-1] = '\0';
    h.idDoctor = d->id;
    h.costo = d->costoConsulta;

    agregarConsultaAlHistorial(p, h);

    strncpy(cita->estado, "Atendida", sizeof(cita->estado)-1);
    cita->estado[sizeof(cita->estado)-1] = '\0';
    cita->atendida = true;

    cout << "Cita atendida correctamente y agregada al historial" << endl;
    return true;
}

Cita** obtenerCitasDePaciente(Hospital* hospital, int idPaciente, int* cantidad) {
    *cantidad = 0;
    if (!hospital) return nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idPaciente == idPaciente) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** arr = new Cita*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idPaciente == idPaciente) arr[idx++] = &hospital->citas[i];
    }
    return arr;
}

Cita** obtenerCitasDeDoctor(Hospital* hospital, int idDoctor, int* cantidad) {
    *cantidad = 0;
    if (!hospital) return nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** arr = new Cita*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor) arr[idx++] = &hospital->citas[i];
    }
    return arr;
}

Cita** obtenerCitasPorFecha(Hospital* hospital, const char* fecha, int* cantidad) {
    *cantidad = 0;
    if (!hospital || !fecha) return nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (strcmp(hospital->citas[i].fecha, fecha) == 0) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** arr = new Cita*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (strcmp(hospital->citas[i].fecha, fecha) == 0) arr[idx++] = &hospital->citas[i];
    }
    return arr;
}

void listarCitasPendientes(Hospital* hospital) {
    cout << "╔══════════════════════════════════════════════════╗" << endl;
    cout << "║                CITAS PENDIENTES                 ║" << endl;
    cout << "╠════╦══════╦══════╦════════════╦═════════╦══════╣" << endl;
    cout << "║ ID ║PAC ID║DOC ID║   FECHA    ║   HORA  ║ESTADO║" << endl;
    cout << "╠════╬══════╬══════╬════════════╬═════════╬══════╣" << endl;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        Cita& c = hospital->citas[i];
        if (strcmp(c.estado, "Agendada") == 0) {
            cout << "║ " << setw(2) << c.id << " ║ "
                 << setw(5) << c.idPaciente << " ║ "
                 << setw(5) << c.idDoctor << " ║ "
                 << setw(10) << c.fecha << " ║ "
                 << setw(7) << c.hora << " ║ "
                 << setw(7) << c.estado << "║" << endl;
        }
    }
    cout << "╚════╩══════╩══════╩════════════╩═════════╩══════╝" << endl;
}

// -------------------- MENUS --------------------
void menuPacientes(Hospital* h) {
    int op;
    do {
        cout << "╔══════════════════════════════╗" << endl;
        cout << "║       GESTION PACIENTES      ║" << endl;
        cout << "╚══════════════════════════════╝" << endl;
        cout << "1. Registrar nuevo paciente" << endl;
        cout << "2. Buscar paciente por cedula" << endl;
        cout << "3. Buscar paciente por nombre" << endl;
        cout << "4. Ver historial medico completo" << endl;
        cout << "5. Actualizar datos del paciente" << endl;
        cout << "6. Listar todos los pacientes" << endl;
        cout << "7. Eliminar paciente" << endl;
        cout << "0. Volver" << endl;
        cout << "Seleccione: " << endl;
        op = leerEntero();

        if (op == 1) {
            char n[50], a[50], c[20]; int e; char s;
            cout << "Nombre: " << endl;
            leerLinea(n,50);
            cout << "Apellido: " << endl;
            leerLinea(a,50);
            cout << "Cedula: " << endl;
            leerLinea(c,20);
            cout << "Edad: " << endl;
            e = leerEntero();
            if (e == -1) { cout << "Edad invalida" << endl; continue; }
            cout << "Sexo (M/F): " << endl;
            cin >> s;
            limpiarBufferEntrada();
            crearPaciente(h, n, a, c, e, s);
        } else if (op == 2) {
            char c[20];
            cout << "Cedula: " << endl;
            leerLinea(c,20);
            Paciente* p = buscarPacientePorCedula(h, c);
            if (p) cout << "Encontrado: " << p->nombre << " " << p->apellido << " ID:" << p->id << endl;
            else cout << "Paciente no encontrado" << endl;
        } else if (op == 3) {
            char n[50];
            cout << "Nombre (parcial): " << endl;
            leerLinea(n,50);
            int cantidad = 0;
            Paciente** arr = buscarPacientesPorNombre(h, n, &cantidad);
            if (!arr) cout << "No se encontraron pacientes" << endl;
            else {
                for (int i = 0; i < cantidad; i++) {
                    cout << "ID: " << arr[i]->id << " - " << arr[i]->nombre << " " << arr[i]->apellido << endl;
                }
                delete[] arr;
            }
        } else if (op == 4) {
            int id; cout << "ID Paciente: " << endl;
            id = leerEntero();
            Paciente* p = buscarPacientePorId(h, id);
            if (p) mostrarHistorialMedico(p); else cout << "Paciente no encontrado" << endl;
        } else if (op == 5) {
            int id; cout << "ID Paciente: " << endl;
            id = leerEntero();
            if (!actualizarPaciente(h, id)) cout << "Actualizacion fallida" << endl;
        } else if (op == 6) {
            listarPacientes(h);
        } else if (op == 7) {
            int id; cout << "ID Paciente a eliminar: " << endl;
            id = leerEntero();
            if (!eliminarPaciente(h, id)) cout << "Eliminacion fallida" << endl;
        }
    } while (op != 0);
}

void menuDoctores(Hospital* h) {
    int op;
    do {
        cout << "╔══════════════════════════════╗" << endl;
        cout << "║       GESTION DOCTORES       ║" << endl;
        cout << "╚══════════════════════════════╝" << endl;
        cout << "1. Registrar nuevo doctor" << endl;
        cout << "2. Buscar doctor por ID" << endl;
        cout << "3. Buscar doctores por especialidad" << endl;
        cout << "4. Asignar paciente a doctor" << endl;
        cout << "5. Ver pacientes asignados a doctor" << endl;
        cout << "6. Listar todos los doctores" << endl;
        cout << "7. Eliminar doctor" << endl;
        cout << "0. Volver" << endl;
        cout << "Seleccione: " << endl;
        op = leerEntero();

        if (op == 1) {
            char n[50], a[50], c[20], esp[50]; int an; float cost;
            cout << "Nombre: " << endl;
            leerLinea(n,50);
            cout << "Apellido: " << endl;
            leerLinea(a,50);
            cout << "Cedula prof.: " << endl;
            leerLinea(c,20);
            cout << "Especialidad: " << endl;
            leerLinea(esp,50);
            cout << "Anios experiencia: " << endl;
            an = leerEntero();
            if (an == -1) { cout << "Anios invalidos" << endl; continue; }
            cout << "Costo consulta: " << endl;
            cin >> cost;
            limpiarBufferEntrada();
            crearDoctor(h, n, a, c, esp, an, cost);
        } else if (op == 2) {
            int id; cout << "ID Doctor: " << endl;
            id = leerEntero();
            Doctor* d = buscarDoctorPorId(h, id);
            if (d) cout << "Encontrado: Dr. " << d->nombre << " " << d->apellido << " ID:" << d->id << endl;
            else cout << "Doctor no encontrado" << endl;
        } else if (op == 3) {
            char esp[50];
            cout << "Especialidad: " << endl;
            leerLinea(esp,50);
            int cantidad = 0;
            Doctor** arr = buscarDoctoresPorEspecialidad(h, esp, &cantidad);
            if (!arr) cout << "No se encontraron doctores" << endl;
            else {
                for (int i = 0; i < cantidad; i++) {
                    cout << "ID: " << arr[i]->id << " - Dr. " << arr[i]->nombre << " " << arr[i]->apellido << endl;
                }
                delete[] arr;
            }
        } else if (op == 4) {
            int idD, idP;
            cout << "ID Doctor: " << endl;
            idD = leerEntero();
            cout << "ID Paciente: " << endl;
            idP = leerEntero();
            Doctor* d = buscarDoctorPorId(h, idD);
            if (!d) cout << "Doctor no encontrado" << endl;
            else if (!asignarPacienteADoctor(d, idP)) cout << "Asignacion fallida (duplicado?)" << endl;
            else cout << "Paciente asignado correctamente" << endl;
        } else if (op == 5) {
            int id; cout << "ID Doctor: " << endl;
            id = leerEntero();
            listarPacientesDeDoctor(h, id);
        } else if (op == 6) {
            listarDoctores(h);
        } else if (op == 7) {
            int id; cout << "ID Doctor a eliminar: " << endl;
            id = leerEntero();
            if (!eliminarDoctor(h, id)) cout << "Eliminacion fallida" << endl;
        }
    } while (op != 0);
}

void menuCitas(Hospital* h) {
    int op;
    do {
        cout << "╔══════════════════════════════╗" << endl;
        cout << "║         GESTION CITAS        ║" << endl;
        cout << "╚══════════════════════════════╝" << endl;
        cout << "1. Agendar nueva cita" << endl;
        cout << "2. Cancelar cita" << endl;
        cout << "3. Atender cita" << endl;
        cout << "4. Ver citas de un paciente" << endl;
        cout << "5. Ver citas de un doctor" << endl;
        cout << "6. Ver citas por fecha" << endl;
        cout << "7. Ver citas pendientes" << endl;
        cout << "0. Volver" << endl;
        cout << "Seleccione: " << endl;
        op = leerEntero();

        if (op == 1) {
            int idP, idD;
            char fecha[11], hora[6], motivo[150];
            cout << "ID Paciente: " << endl;
            idP = leerEntero();
            cout << "ID Doctor: " << endl;
            idD = leerEntero();
            cout << "Fecha (YYYY-MM-DD): " << endl;
            leerLinea(fecha,11);
            cout << "Hora (HH:MM): " << endl;
            leerLinea(hora,6);
            cout << "Motivo: " << endl;
            leerLinea(motivo,150);
            agendarCita(h, idP, idD, fecha, hora, motivo);
        } else if (op == 2) {
            int id; cout << "ID Cita a cancelar: " << endl;
            id = leerEntero();
            if (!cancelarCita(h, id)) cout << "Cancelacion fallida" << endl;
        } else if (op == 3) {
            int id; cout << "ID Cita a atender: " << endl;
            id = leerEntero();
            limpiarBufferEntrada();
            char diag[200], trat[200], med[150];
            cout << "Diagnostico: " << endl;
            leerLinea(diag,200);
            cout << "Tratamiento: " << endl;
            leerLinea(trat,200);
            cout << "Medicamentos: " << endl;
            leerLinea(med,150);
            if (!atenderCita(h, id, diag, trat, med)) cout << "Atencion fallida" << endl;
        } else if (op == 4) {
            int id; cout << "ID Paciente: " << endl;
            id = leerEntero();
            int cantidad = 0;
            Cita** arr = obtenerCitasDePaciente(h, id, &cantidad);
            if (!arr) cout << "No hay citas" << endl;
            else {
                for (int i = 0; i < cantidad; i++) {
                    cout << "Cita ID: " << arr[i]->id << " Fecha: " << arr[i]->fecha << " Hora: " << arr[i]->hora << " Estado: " << arr[i]->estado << endl;
                }
                delete[] arr;
            }
        } else if (op == 5) {
            int id; cout << "ID Doctor: " << endl;
            id = leerEntero();
            int cantidad = 0;
            Cita** arr = obtenerCitasDeDoctor(h, id, &cantidad);
            if (!arr) cout << "No hay citas" << endl;
            else {
                for (int i = 0; i < cantidad; i++) {
                    cout << "Cita ID: " << arr[i]->id << " Paciente: " << arr[i]->idPaciente << " Fecha: " << arr[i]->fecha << endl;
                }
                delete[] arr;
            }
        } else if (op == 6) {
            char fecha[11];
            cout << "Fecha (YYYY-MM-DD): " << endl;
            leerLinea(fecha,11);
            int cantidad = 0;
            Cita** arr = obtenerCitasPorFecha(h, fecha, &cantidad);
            if (!arr) cout << "No hay citas en esa fecha" << endl;
            else {
                for (int i = 0; i < cantidad; i++) {
                    cout << "Cita ID: " << arr[i]->id << " Paciente: " << arr[i]->idPaciente << " Doctor: " << arr[i]->idDoctor << endl;
                }
                delete[] arr;
            }
        } else if (op == 7) {
            listarCitasPendientes(h);
        }
    } while (op != 0);
}

// muestra datos basicos del hospital
void mostrarDatosHospital(Hospital* h) {
    if (!h) return;
    cout << "Datos del Hospital" << endl;
    cout << "Nombre: " << h->nombre << endl;
    cout << "Direccion: " << h->direccion << endl;
    cout << "Telefono: " << h->telefono << endl;
    cout << "Cantidad de Pacientes: " << h->cantidadPacientes << endl;
    cout << "Cantidad de Doctores: " << h->cantidadDoctores << endl;
    cout << "Cantidad de Citas: " << h->cantidadCitas << endl;
}

// -------------------- MAIN --------------------
int main() {
    Hospital* h = inicializarHospital("Hospital Central", 10);

    // Datos minimos para pruebas
    crearPaciente(h, "Juan", "Perez", "V-12345678", 30, 'M');
    crearPaciente(h, "Maria", "Lopez", "V-87654321", 25, 'F');

    crearDoctor(h, "Carlos", "Gomez", "D-001", "Cardiologia", 10, 50.0f);
    crearDoctor(h, "Ana", "Torres", "D-002", "Pediatria", 5, 40.0f);

    int opcion;
    do {
        cout << "╔════════════════════════════════════════╗" << endl;
        cout << "║      SISTEMA DE GESTION HOSPITALARIA   ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "1. Gestion de Pacientes" << endl;
        cout << "2. Gestion de Doctores" << endl;
        cout << "3. Gestion de Citas" << endl;
        cout << "4. Mostrar Datos del Hospital" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione: " << endl;
        opcion = leerEntero();
        if (opcion == 1) menuPacientes(h);
        else if (opcion == 2) menuDoctores(h);
        else if (opcion == 3) menuCitas(h);
        else if (opcion == 4) mostrarDatosHospital(h);
    } while (opcion != 0);

    destruirHospital(h);
    cout << "Saliendo del sistema..." << endl;
    return 0;
}
