// tarea_santiago_padron_completo.cpp
// Version extendida del sistema de gestion hospitalaria
// Se han añadido utilidades y funciones faltantes solicitadas
// Reglas: sin acentos en los comentarios, uso de endl en lugar de '\n'

#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>

using namespace std;

// ===================== ESTRUCTURAS DE DATOS =====================

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

// ===================== CONSTANTES GLOBALES =====================

const int CAP_PACIENTES_INICIAL = 10;
const int CAP_DOCTORES_INICIAL = 10;
const int CAP_CITAS_INICIAL = 20;
const int CAP_HISTORIAL_INICIAL = 5;
const int CAP_CITAS_PACIENTE_INICIAL = 5;
const int CAP_PACIENTOS_DOCTOR_INICIAL = 5;
const int CAP_CITAS_DOCTOR_INICIAL = 10;

// ===================== PROTOTIPOS DE FUNCIONES =====================

// ========== FUNCIONES DE UTILIDAD Y ENTRADA DE DATOS ==========
void limpiarBufferEntrada();
void leerLinea(char* buffer, int size);
int leerEntero();
char* copiarString(const char* origen);

// ========== FUNCIONES DE VALIDACION ==========
bool validarCedula(const char* cedula);
bool validarEmail(const char* email);
bool validarFecha(const char* fecha);
bool validarHora(const char* hora);
int compararFechas(const char* fecha1, const char* fecha2);

// ========== FUNCIONES DE GESTION DE MEMORIA ==========
void redimensionarArrayPacientes(Hospital* hospital);
void redimensionarArrayDoctores(Hospital* hospital);
void redimensionarArrayCitas(Hospital* hospital);
Hospital* inicializarHospital(const char* nombre, int capacidadInicial);
void destruirHospital(Hospital* hospital);

// ========== FUNCIONES DE GESTION DE PACIENTES ==========
Paciente* crearPaciente(Hospital* hospital, const char* nombre, const char* apellido,
                        const char* cedula, int edad, char sexo);
Paciente* buscarPacientePorCedula(Hospital* hospital, const char* cedula);
Paciente* buscarPacientePorId(Hospital* hospital, int id);
Paciente** buscarPacientesPorNombre(Hospital* hospital, const char* nombre, int* cantidad);
bool actualizarPaciente(Hospital* hospital, int id);
bool eliminarPaciente(Hospital* hospital, int id);
void listarPacientes(Hospital* hospital);

// ========== FUNCIONES DE GESTION DE HISTORIAL MEDICO ==========
void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico consulta);
HistorialMedico* obtenerHistorialCompleto(Paciente* paciente, int* cantidad);
void mostrarHistorialMedico(Paciente* paciente);
HistorialMedico* obtenerUltimaConsulta(Paciente* paciente);

// ========== FUNCIONES DE GESTION DE DOCTORES ==========
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

// ========== FUNCIONES DE GESTION DE CITAS ==========
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

// ========== FUNCIONES DE MENU ==========
void menuPacientes(Hospital* h);
void menuDoctores(Hospital* h);
void menuCitas(Hospital* h);

// ========== FUNCIONES DE UTILIDAD ADICIONALES ==========
void mostrarDatosHospital(Hospital* h);

// ===================== IMPLEMENTACIONES =====================

// ========== IMPLEMENTACIONES DE UTILIDAD Y ENTRADA ==========

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

char* copiarString(const char* origen) {
    if (!origen) return nullptr;
    int longitud = (int)strlen(origen);
    char* copia = new char[longitud + 1];
    strcpy(copia, origen);
    return copia;
}

// ========== IMPLEMENTACIONES DE VALIDACION ==========

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
    for (const char* p = email; p < at; p++) {
        if (!isalnum((unsigned char)*p) && *p != '.' && *p != '_' && *p != '-' && *p != '+') return false;
    }
    for (const char* p = at + 1; *p; p++) {
        if (!isalnum((unsigned char)*p) && *p != '.' && *p != '-') return false;
    }
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

// ========== IMPLEMENTACIONES DE GESTION DE MEMORIA ==========

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

// ========== IMPLEMENTACIONES DE GESTION DE PACIENTES ==========

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
    cout << "================================================================" << endl;
    cout << "                     LISTA DE PACIENTES                         " << endl;
    cout << "=================================================================" << endl;
    cout << "ID  NOMBRE COMPLETO              CEDULA     EDAD CONS." << endl;
    cout << "-----------------------------------------------------------------" << endl;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        char fullname[101];
        fullname[0] = '\0';
        strncat(fullname, p.nombre, sizeof(fullname)-1);
        strncat(fullname, " ", sizeof(fullname)-strlen(fullname)-1);
        strncat(fullname, p.apellido, sizeof(fullname)-strlen(fullname)-1);

        cout << setw(2) << p.id << "  "
             << left << setw(28) << fullname
             << " " << setw(10) << p.cedula
             << " " << setw(4) << p.edad
             << " " << setw(5) << p.cantidadConsultas << endl;
    }
    cout << "=================================================================" << endl;
}

// ========== IMPLEMENTACIONES DE GESTION DE HISTORIAL MEDICO ==========

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

// ========== IMPLEMENTACIONES DE GESTION DE DOCTORES ==========

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
        if (strstr(hospital->doctores[i].especialidad, especialidad) != nullptr) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Doctor** resultados = new Doctor*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strstr(hospital->doctores[i].especialidad, especialidad) != nullptr) {
            resultados[idx++] = &hospital->doctores[i];
        }
    }
    return resultados;
}

Doctor** buscarDoctoresPorNombre(Hospital* hospital, const char* nombre, int* cantidad) {
    *cantidad = 0;
    if (!hospital || !nombre) return nullptr;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strstr(hospital->doctores[i].nombre, nombre) != nullptr) (*cantidad)++;
        else if (strstr(hospital->doctores[i].apellido, nombre) != nullptr) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Doctor** resultados = new Doctor*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (strstr(hospital->doctores[i].nombre, nombre) != nullptr ||
            strstr(hospital->doctores[i].apellido, nombre) != nullptr) {
            resultados[idx++] = &hospital->doctores[i];
        }
    }
    return resultados;
}

bool asignarPacienteADoctor(Doctor* doctor, int idPaciente) {
    if (!doctor) return false;
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente) return false;
    }
    if (doctor->cantidadPacientes >= doctor->capacidadPacientes) {
        int nuevaCap = doctor->capacidadPacientes * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < doctor->cantidadPacientes; i++) {
            nuevo[i] = doctor->pacientesAsignados[i];
        }
        delete[] doctor->pacientesAsignados;
        doctor->pacientesAsignados = nuevo;
        doctor->capacidadPacientes = nuevaCap;
    }
    doctor->pacientesAsignados[doctor->cantidadPacientes++] = idPaciente;
    return true;
}

bool removerPacienteDeDoctor(Doctor* doctor, int idPaciente) {
    if (!doctor) return false;
    int index = -1;
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente) {
            index = i;
            break;
        }
    }
    if (index == -1) return false;
    for (int i = index; i < doctor->cantidadPacientes - 1; i++) {
        doctor->pacientesAsignados[i] = doctor->pacientesAsignados[i + 1];
    }
    doctor->cantidadPacientes--;
    return true;
}

void listarPacientesDeDoctor(Hospital* hospital, int idDoctor) {
    Doctor* d = buscarDoctorPorId(hospital, idDoctor);
    if (!d) return;
    cout << "Pacientes asignados al Dr. " << d->nombre << " " << d->apellido << ":" << endl;
    for (int i = 0; i < d->cantidadPacientes; i++) {
        Paciente* p = buscarPacientePorId(hospital, d->pacientesAsignados[i]);
        if (p) cout << p->id << ": " << p->nombre << " " << p->apellido << endl;
    }
}

void listarDoctores(Hospital* hospital) {
    cout << "=================================================================" << endl;
    cout << "                     LISTA DE DOCTORES                          " << endl;
    cout << "=================================================================" << endl;
    cout << "ID  NOMBRE COMPLETO              ESPECIALIDAD           EXP. COSTO" << endl;
    cout << "-----------------------------------------------------------------" << endl;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        char fullname[101];
        fullname[0] = '\0';
        strncat(fullname, d.nombre, sizeof(fullname)-1);
        strncat(fullname, " ", sizeof(fullname)-strlen(fullname)-1);
        strncat(fullname, d.apellido, sizeof(fullname)-strlen(fullname)-1);

        cout << setw(2) << d.id << "  "
             << left << setw(28) << fullname
             << " " << setw(20) << d.especialidad
             << " " << setw(4) << d.aniosExperiencia
             << " " << setw(6) << fixed << setprecision(2) << d.costoConsulta << endl;
    }
    cout << "=================================================================" << endl;
}

bool eliminarDoctor(Hospital* hospital, int id) {
    if (!hospital) return false;
    int index = -1;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) {
            index = i;
            break;
        }
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

// ========== IMPLEMENTACIONES DE GESTION DE CITAS ==========

Cita* agendarCita(Hospital* hospital, int idPaciente, int idDoctor,
                  const char* fecha, const char* hora, const char* motivo) {
    if (!hospital) return nullptr;
    Paciente* p = buscarPacientePorId(hospital, idPaciente);
    Doctor* d = buscarDoctorPorId(hospital, idDoctor);
    if (!p || !d) return nullptr;
    if (!validarFecha(fecha) || !validarHora(hora)) return nullptr;
    if (!verificarDisponibilidad(hospital, idDoctor, fecha, hora)) return nullptr;

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
    strncpy(c.estado, "Programada", sizeof(c.estado)-1);
    c.estado[sizeof(c.estado)-1] = '\0';
    c.observaciones[0] = '\0';
    c.atendida = false;

    if (p->cantidadCitas >= p->capacidadCitas) {
        int nuevaCap = p->capacidadCitas * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < p->cantidadCitas; i++) {
            nuevo[i] = p->citasAgendadas[i];
        }
        delete[] p->citasAgendadas;
        p->citasAgendadas = nuevo;
        p->capacidadCitas = nuevaCap;
    }
    p->citasAgendadas[p->cantidadCitas++] = c.id;

    if (d->cantidadCitas >= d->capacidadCitas) {
        int nuevaCap = d->capacidadCitas * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < d->cantidadCitas; i++) {
            nuevo[i] = d->citasAgendadas[i];
        }
        delete[] d->citasAgendadas;
        d->citasAgendadas = nuevo;
        d->capacidadCitas = nuevaCap;
    }
    d->citasAgendadas[d->cantidadCitas++] = c.id;

    hospital->cantidadCitas++;
    cout << "Cita agendada correctamente con ID: " << c.id << endl;
    return &c;
}

bool cancelarCita(Hospital* hospital, int idCita) {
    if (!hospital) return false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == idCita) {
            strcpy(hospital->citas[i].estado, "Cancelada");
            hospital->citas[i].atendida = false;
            cout << "Cita cancelada correctamente" << endl;
            return true;
        }
    }
    return false;
}

bool atenderCita(Hospital* hospital, int idCita, const char* diagnostico,
                 const char* tratamiento, const char* medicamentos) {
    if (!hospital) return false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == idCita && !hospital->citas[i].atendida) {
            Cita& c = hospital->citas[i];
            c.atendida = true;
            strcpy(c.estado, "Atendida");
            strncpy(c.observaciones, diagnostico, sizeof(c.observaciones)-1);
            c.observaciones[sizeof(c.observaciones)-1] = '\0';

            Paciente* p = buscarPacientePorId(hospital, c.idPaciente);
            if (p) {
                HistorialMedico hm;
                hm.idConsulta = hospital->siguienteIdConsulta++;
                strncpy(hm.fecha, c.fecha, sizeof(hm.fecha)-1);
                hm.fecha[sizeof(hm.fecha)-1] = '\0';
                strncpy(hm.hora, c.hora, sizeof(hm.hora)-1);
                hm.hora[sizeof(hm.hora)-1] = '\0';
                strncpy(hm.diagnostico, diagnostico, sizeof(hm.diagnostico)-1);
                hm.diagnostico[sizeof(hm.diagnostico)-1] = '\0';
                strncpy(hm.tratamiento, tratamiento, sizeof(hm.tratamiento)-1);
                hm.tratamiento[sizeof(hm.tratamiento)-1] = '\0';
                strncpy(hm.medicamentos, medicamentos, sizeof(hm.medicamentos)-1);
                hm.medicamentos[sizeof(hm.medicamentos)-1] = '\0';
                hm.idDoctor = c.idDoctor;
                Doctor* doc = buscarDoctorPorId(hospital, c.idDoctor);
                hm.costo = doc ? doc->costoConsulta : 0.0f;
                agregarConsultaAlHistorial(p, hm);
            }
            cout << "Cita atendida y registrada en historial" << endl;
            return true;
        }
    }
    return false;
}

Cita** obtenerCitasDePaciente(Hospital* hospital, int idPaciente, int* cantidad) {
    *cantidad = 0;
    if (!hospital) return nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idPaciente == idPaciente) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** resultados = new Cita*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idPaciente == idPaciente) {
            resultados[idx++] = &hospital->citas[i];
        }
    }
    return resultados;
}

Cita** obtenerCitasDeDoctor(Hospital* hospital, int idDoctor, int* cantidad) {
    *cantidad = 0;
    if (!hospital) return nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** resultados = new Cita*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor) {
            resultados[idx++] = &hospital->citas[i];
        }
    }
    return resultados;
}

Cita** obtenerCitasPorFecha(Hospital* hospital, const char* fecha, int* cantidad) {
    *cantidad = 0;
    if (!hospital || !fecha) return nullptr;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (strcmp(hospital->citas[i].fecha, fecha) == 0) (*cantidad)++;
    }
    if (*cantidad == 0) return nullptr;
    Cita** resultados = new Cita*[*cantidad];
    int idx = 0;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (strcmp(hospital->citas[i].fecha, fecha) == 0) {
            resultados[idx++] = &hospital->citas[i];
        }
    }
    return resultados;
}

void listarCitasPendientes(Hospital* hospital) {
    cout << "Citas pendientes:" << endl;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (!hospital->citas[i].atendida && strcmp(hospital->citas[i].estado, "Cancelada") != 0) {
            Cita& c = hospital->citas[i];
            Paciente* p = buscarPacientePorId(hospital, c.idPaciente);
            Doctor* d = buscarDoctorPorId(hospital, c.idDoctor);
            cout << "Cita #" << c.id << " - Paciente: " << (p ? p->nombre : "N/A") << " - Doctor: " << (d ? d->nombre : "N/A") << " - Fecha: " << c.fecha << " " << c.hora << " - Motivo: " << c.motivo << endl;
        }
    }
}

bool verificarDisponibilidad(Hospital* hospital, int idDoctor, const char* fecha, const char* hora) {
    if (!hospital) return false;
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].idDoctor == idDoctor &&
            strcmp(hospital->citas[i].fecha, fecha) == 0 &&
            strcmp(hospital->citas[i].hora, hora) == 0 &&
            !hospital->citas[i].atendida &&
            strcmp(hospital->citas[i].estado, "Cancelada") != 0) {
            return false;
        }
    }
    return true;
}

// ========== IMPLEMENTACIONES DE MENU ==========

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
                crearPaciente(h, nombre, apellido, cedula, edad, sexo);
                break;
            }
            case 2:
                listarPacientes(h);
                break;
            case 3: {
                char cedula[20];
                cout << "Cedula a buscar: ";
                leerLinea(cedula, 20);
                Paciente* p = buscarPacientePorCedula(h, cedula);
                if (p) {
                    cout << "Paciente encontrado: " << p->nombre << " " << p->apellido << " (ID: " << p->id << ")" << endl;
                } else {
                    cout << "Paciente no encontrado" << endl;
                }
                break;
            }
            case 4: {
                int id;
                cout << "ID del paciente a actualizar: ";
                id = leerEntero();
                actualizarPaciente(h, id);
                break;
            }
            case 5: {
                int id;
                cout << "ID del paciente a eliminar: ";
                id = leerEntero();
                eliminarPaciente(h, id);
                break;
            }
            case 6: {
                int id;
                cout << "ID del paciente para ver historial: ";
                id = leerEntero();
                Paciente* p = buscarPacientePorId(h, id);
                if (p) mostrarHistorialMedico(p);
                else cout << "Paciente no encontrado" << endl;
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
                crearDoctor(h, nombre, apellido, cedula, especialidad, experiencia, costo);
                break;
            }
            case 2:
                listarDoctores(h);
                break;
            case 3: {
                char especialidad[50];
                cout << "Especialidad a buscar: ";
                leerLinea(especialidad, 50);
                int cantidad;
                Doctor** resultados = buscarDoctoresPorEspecialidad(h, especialidad, &cantidad);
                if (resultados) {
                    cout << "Doctores encontrados en " << especialidad << ":" << endl;
                    for (int i = 0; i < cantidad; i++) {
                        cout << resultados[i]->id << ": " << resultados[i]->nombre << " " << resultados[i]->apellido << endl;
                    }
                    delete[] resultados;
                } else {
                    cout << "No se encontraron doctores con esa especialidad" << endl;
                }
                break;
            }
            case 4: {
                int idDoctor, idPaciente;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                Doctor* d = buscarDoctorPorId(h, idDoctor);
                Paciente* p = buscarPacientePorId(h, idPaciente);
                if (d && p) {
                    if (asignarPacienteADoctor(d, idPaciente)) {
                        cout << "Paciente asignado correctamente" << endl;
                    } else {
                        cout << "El paciente ya estaba asignado" << endl;
                    }
                } else {
                    cout << "Doctor o paciente no encontrado" << endl;
                }
                break;
            }
            case 5: {
                int idDoctor;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                listarPacientesDeDoctor(h, idDoctor);
                break;
            }
            case 6: {
                int id;
                cout << "ID del doctor a eliminar: ";
                id = leerEntero();
                eliminarDoctor(h, id);
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

void menuCitas(Hospital* h) {
    int opcion;
    do {
        cout << endl;
        cout << "=== GESTION DE CITAS ===" << endl;
        cout << "1. Agendar cita" << endl;
        cout << "2. Listar citas pendientes" << endl;
        cout << "3. Cancelar cita" << endl;
        cout << "4. Atender cita" << endl;
        cout << "5. Ver citas de paciente" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1: {
                int idPaciente, idDoctor;
                char fecha[11], hora[6], motivo[150];
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                cout << "Fecha (YYYY-MM-DD): ";
                leerLinea(fecha, 11);
                cout << "Hora (HH:MM): ";
                leerLinea(hora, 6);
                cout << "Motivo de la consulta: ";
                leerLinea(motivo, 150);
                agendarCita(h, idPaciente, idDoctor, fecha, hora, motivo);
                break;
            }
            case 2:
                listarCitasPendientes(h);
                break;
            case 3: {
                int idCita;
                cout << "ID de la cita a cancelar: ";
                idCita = leerEntero();
                cancelarCita(h, idCita);
                break;
            }
            case 4: {
                int idCita;
                char diagnostico[200], tratamiento[200], medicamentos[150];
                cout << "ID de la cita a atender: ";
                idCita = leerEntero();
                cout << "Diagnostico: ";
                leerLinea(diagnostico, 200);
                cout << "Tratamiento: ";
                leerLinea(tratamiento, 200);
                cout << "Medicamentos: ";
                leerLinea(medicamentos, 150);
                atenderCita(h, idCita, diagnostico, tratamiento, medicamentos);
                break;
            }
            case 5: {
                int idPaciente;
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                int cantidad;
                Cita** citas = obtenerCitasDePaciente(h, idPaciente, &cantidad);
                if (citas) {
                    cout << "Citas del paciente:" << endl;
                    for (int i = 0; i < cantidad; i++) {
                        cout << "Cita #" << citas[i]->id << " - Fecha: " << citas[i]->fecha << " " << citas[i]->hora << " - Estado: " << citas[i]->estado << endl;
                    }
                    delete[] citas;
                } else {
                    cout << "No se encontraron citas para este paciente" << endl;
                }
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

// ========== IMPLEMENTACIONES DE UTILIDAD ADICIONALES ==========

void mostrarDatosHospital(Hospital* h) {
    cout << "=== INFORMACION DEL HOSPITAL ===" << endl;
    cout << "Nombre: " << h->nombre << endl;
    cout << "Direccion: " << h->direccion << endl;
    cout << "Telefono: " << h->telefono << endl;
    cout << "Pacientes registrados: " << h->cantidadPacientes << endl;
    cout << "Doctores registrados: " << h->cantidadDoctores << endl;
    cout << "Citas registradas: " << h->cantidadCitas << endl;
    cout << "=================================" << endl;
}

// ===================== FUNCION PRINCIPAL =====================

int main() {
    cout << "SISTEMA DE GESTION HOSPITALARIA" << endl;
    cout << "Inicializando hospital..." << endl;

    Hospital* hospital = inicializarHospital("Hospital Central", 10);

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
                menuCitas(hospital);
                break;
            case 4:
                mostrarDatosHospital(hospital);
                break;
            case 0:
                cout << "Saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
        }
    } while (opcion != 0);

    destruirHospital(hospital);
    return 0;
}