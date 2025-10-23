#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
using namespace std;

// ===================== ESTRUCTURAS PRINCIPALES =====================
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

// ===================== FUNCIONES AUXILIARES =====================
char* copiarString(const char* origen) {
    if (!origen) return nullptr;
    int longitud = strlen(origen);
    char* copia = new char[longitud + 1];
    strcpy(copia, origen);
    return copia;
}

// ===================== VALIDACIONES =====================
bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    int len = strlen(cedula);
    return (len > 0 && len <= 20);
}

bool validarEmail(const char* email) {
    if (!email) return false;
    return (strchr(email, '@') && strchr(email, '.'));
}

bool validarFecha(const char* fecha) {
    if (strlen(fecha) != 10) return false;
    int y, m, d;
    if (sscanf(fecha, "%d-%d-%d", &y, &m, &d) != 3) return false;
    if (m < 1 || m > 12 || d < 1 || d > 31) return false;
    if ((m == 4 || m == 6 || m == 9 || m == 11) && d > 30) return false;
    if (m == 2) {
        bool bisiesto = (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0));
        if (d > (bisiesto ? 29 : 28)) return false;
    }
    return true;
}

bool validarHora(const char* hora) {
    if (strlen(hora) != 5) return false;
    int h, m;
    if (sscanf(hora, "%d:%d", &h, &m) != 2) return false;
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}

int compararFechas(const char* fecha1, const char* fecha2) {
    return strcmp(fecha1, fecha2);
}

// ===================== REDIMENSIONADORES =====================
void redimensionarArrayPacientes(Hospital* hospital) {
    int nuevaCapacidad = hospital->capacidadPacientes * 2;
    Paciente* nuevoArray = new Paciente[nuevaCapacidad];
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        nuevoArray[i] = hospital->pacientes[i];
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
    delete[] hospital->citas;
    hospital->citas = nuevoArray;
    hospital->capacidadCitas = nuevaCapacidad;
}

// ===================== INICIALIZAR Y DESTRUIR HOSPITAL =====================
Hospital* inicializarHospital(const char* nombre, int capacidadInicial) {
    Hospital* hospital = new Hospital;
    strcpy(hospital->nombre, nombre);
    strcpy(hospital->direccion, "Sin direccion");
    strcpy(hospital->telefono, "0000000000");

    hospital->capacidadPacientes = 10;
    hospital->capacidadDoctores = 10;
    hospital->capacidadCitas = 20;

    hospital->cantidadPacientes = 0;
    hospital->cantidadDoctores = 0;
    hospital->cantidadCitas = 0;

    hospital->pacientes = new Paciente[hospital->capacidadPacientes];
    hospital->doctores = new Doctor[hospital->capacidadDoctores];
    hospital->citas = new Cita[hospital->capacidadCitas];

    hospital->siguienteIdPaciente = 1;
    hospital->siguienteIdDoctor = 1;
    hospital->siguienteIdCita = 1;
    hospital->siguienteIdConsulta = 1;

    cout << "Hospital inicializado correctamente" << endl;
    return hospital;
}

void destruirHospital(Hospital* hospital) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        delete[] hospital->pacientes[i].historial;
        delete[] hospital->pacientes[i].citasAgendadas;
    }
    delete[] hospital->pacientes;

    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        delete[] hospital->doctores[i].pacientesAsignados;
        delete[] hospital->doctores[i].citasAgendadas;
    }
    delete[] hospital->doctores;

    delete[] hospital->citas;
    delete hospital;
    cout << "Hospital destruido y memoria liberada" << endl;
}

// ===================== MAIN DE PRUEBA BASE =====================
int main() {
    Hospital* h = inicializarHospital("Hospital Central", 10);
    destruirHospital(h);
    return 0;
}
#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
using namespace std;

// ============================================================
// ======= ESTRUCTURAS (ya definidas en Commit 1) =============
// ============================================================

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

// ============================================================
// ============ FUNCIONES DE APOYO Y VALIDACIÓN ===============
// ============================================================

bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    int len = strlen(cedula);
    return (len > 0 && len <= 20);
}

// ============================================================
// =================== PACIENTES CRUD ==========================
// ============================================================

Paciente* buscarPacientePorCedula(Hospital* hospital, const char* cedula) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (strcmp(hospital->pacientes[i].cedula, cedula) == 0)
            return &hospital->pacientes[i];
    }
    return nullptr;
}

Paciente* buscarPacientePorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) return &hospital->pacientes[i];
    }
    return nullptr;
}

Paciente* crearPaciente(Hospital* hospital, const char* nombre, const char* apellido,
                        const char* cedula, int edad, char sexo) {
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
    strcpy(p.nombre, nombre);
    strcpy(p.apellido, apellido);
    strcpy(p.cedula, cedula);
    p.edad = edad;
    p.sexo = sexo;
    strcpy(p.tipoSangre, "O+");
    strcpy(p.telefono, "0000000000");
    strcpy(p.direccion, "Sin direccion");
    strcpy(p.email, "noemail@hospital.com");

    p.capacidadHistorial = 5;
    p.cantidadConsultas = 0;
    p.historial = new HistorialMedico[p.capacidadHistorial];

    p.capacidadCitas = 5;
    p.cantidadCitas = 0;
    p.citasAgendadas = new int[p.capacidadCitas];

    strcpy(p.alergias, "");
    strcpy(p.observaciones, "");
    p.activo = true;

    hospital->cantidadPacientes++;
    cout << "Paciente creado correctamente con ID: " << p.id << endl;
    return &p;
}

bool eliminarPaciente(Hospital* hospital, int id) {
    int index = -1;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        if (hospital->pacientes[i].id == id) {
            index = i;
            break;
        }
    }
    if (index == -1) return false;

    delete[] hospital->pacientes[index].historial;
    delete[] hospital->pacientes[index].citasAgendadas;

    for (int i = index; i < hospital->cantidadPacientes - 1; i++) {
        hospital->pacientes[i] = hospital->pacientes[i + 1];
    }
    hospital->cantidadPacientes--;
    cout << "Paciente eliminado correctamente" << endl;
    return true;
}

void listarPacientes(Hospital* hospital) {
    cout << "╔════════════════════════════════════════════════════╗" << endl;
    cout << "║                 LISTA DE PACIENTES                ║" << endl;
    cout << "╠════╦════════════════════╦════════════╦═════╦══════╣" << endl;
    cout << "║ ID ║ NOMBRE COMPLETO    ║ CEDULA     ║ EDAD║CONS.║" << endl;
    cout << "╠════╬════════════════════╬════════════╬═════╬══════╣" << endl;
    for (int i = 0; i < hospital->cantidadPacientes; i++) {
        Paciente& p = hospital->pacientes[i];
        cout << "║ " << setw(2) << p.id << " ║ "
             << left << setw(18) << (string(p.nombre) + " " + p.apellido)
             << "║ " << setw(10) << p.cedula
             << "║ " << setw(4) << p.edad
             << "║ " << setw(5) << p.cantidadConsultas << "║" << endl;
    }
    cout << "╚════╩════════════════════╩════════════╩═════╩══════╝" << endl;
}

// =================== HISTORIAL MÉDICO =====================

void agregarConsultaAlHistorial(Paciente* paciente, HistorialMedico consulta) {
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

void mostrarHistorialMedico(Paciente* paciente) {
    cout << "Historial medico de " << paciente->nombre << " " << paciente->apellido << endl;
    for (int i = 0; i < paciente->cantidadConsultas; i++) {
        HistorialMedico& h = paciente->historial[i];
        cout << "Consulta #" << h.idConsulta << " - Fecha: " << h.fecha
             << " - Diagnostico: " << h.diagnostico << endl;
    }
}

// ============================================================
// =================== DOCTORES CRUD ==========================
// ============================================================

Doctor* buscarDoctorPorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        if (hospital->doctores[i].id == id) return &hospital->doctores[i];
    }
    return nullptr;
}

Doctor* crearDoctor(Hospital* hospital, const char* nombre, const char* apellido,
                    const char* cedula, const char* especialidad,
                    int aniosExperiencia, float costoConsulta) {
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
    strcpy(d.nombre, nombre);
    strcpy(d.apellido, apellido);
    strcpy(d.cedula, cedula);
    strcpy(d.especialidad, especialidad);
    d.aniosExperiencia = aniosExperiencia;
    d.costoConsulta = costoConsulta;
    strcpy(d.horarioAtencion, "Lun-Vie 8:00-16:00");
    strcpy(d.telefono, "0000000000");
    strcpy(d.email, "doctor@hospital.com");

    d.capacidadPacientes = 5;
    d.cantidadPacientes = 0;
    d.pacientesAsignados = new int[d.capacidadPacientes];

    d.capacidadCitas = 10;
    d.cantidadCitas = 0;
    d.citasAgendadas = new int[d.capacidadCitas];

    d.disponible = true;
    hospital->cantidadDoctores++;

    cout << "Doctor creado correctamente con ID: " << d.id << endl;
    return &d;
}

bool asignarPacienteADoctor(Doctor* doctor, int idPaciente) {
    for (int i = 0; i < doctor->cantidadPacientes; i++) {
        if (doctor->pacientesAsignados[i] == idPaciente)
            return false;
    }
    if (doctor->cantidadPacientes >= doctor->capacidadPacientes) {
        int nuevaCap = doctor->capacidadPacientes * 2;
        int* nuevo = new int[nuevaCap];
        for (int i = 0; i < doctor->cantidadPacientes; i++)
            nuevo[i] = doctor->pacientesAsignados[i];
        delete[] doctor->pacientesAsignados;
        doctor->pacientesAsignados = nuevo;
        doctor->capacidadPacientes = nuevaCap;
    }
    doctor->pacientesAsignados[doctor->cantidadPacientes++] = idPaciente;
    return true;
}

void listarDoctores(Hospital* hospital) {
    cout << "╔════════════════════════════════════════════════════╗" << endl;
    cout << "║                  LISTA DE DOCTORES                ║" << endl;
    cout << "╠════╦════════════════════╦════════════╦════════════╣" << endl;
    cout << "║ ID ║ NOMBRE COMPLETO    ║ CEDULA     ║ ESPECIALIDAD║" << endl;
    cout << "╠════╬════════════════════╬════════════╬════════════╣" << endl;
    for (int i = 0; i < hospital->cantidadDoctores; i++) {
        Doctor& d = hospital->doctores[i];
        cout << "║ " << setw(2) << d.id << " ║ "
             << left << setw(18) << (string(d.nombre) + " " + d.apellido)
             << "║ " << setw(10) << d.cedula
             << "║ " << setw(12) << d.especialidad << "║" << endl;
    }
    cout << "╚════╩════════════════════╩════════════╩════════════╝" << endl;
}

// ============================================================
// =================== MAIN DE PRUEBA ==========================
// ============================================================

int main() {
    Hospital* h = inicializarHospital("Hospital Central", 10);
    crearPaciente(h, "Juan", "Perez", "V12345", 30, 'M');
    crearPaciente(h, "Maria", "Lopez", "V67890", 25, 'F');
    listarPacientes(h);

    crearDoctor(h, "Carlos", "Gomez", "DOC001", "Cardiologia", 10, 50.0);
    crearDoctor(h, "Ana", "Torres", "DOC002", "Pediatria", 5, 40.0);
    listarDoctores(h);

    destruirHospital(h);
    return 0;
}


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

// ============================================================
// ======= FUNCIONES BÁSICAS YA EXISTENTES (resumen) ==========
// ============================================================
bool validarFecha(const char* fecha) {
    if (strlen(fecha) != 10) return false;
    int y, m, d;
    if (sscanf(fecha, "%d-%d-%d", &y, &m, &d) != 3) return false;
    if (m < 1 || m > 12 || d < 1 || d > 31) return false;
    return true;
}

bool validarHora(const char* hora) {
    if (strlen(hora) != 5) return false;
    int h, m;
    if (sscanf(hora, "%d:%d", &h, &m) != 2) return false;
    return (h >= 0 && h <= 23 && m >= 0 && m <= 59);
}

// ============================================================
// =================== FUNCIONES DE CITAS =====================
// ============================================================

Cita* buscarCitaPorId(Hospital* hospital, int id) {
    for (int i = 0; i < hospital->cantidadCitas; i++) {
        if (hospital->citas[i].id == id)
            return &hospital->citas[i];
    }
    return nullptr;
}

bool verificarDisponibilidad(Hospital* hospital, int idDoctor, const char* fecha, const char* hora) {
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
    if (!validarFecha(fecha) || !validarHora(hora)) {
        cout << "Fecha u hora invalida" << endl;
        return nullptr;
    }
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
    strcpy(c.fecha, fecha);
    strcpy(c.hora, hora);
    strcpy(c.motivo, motivo);
    strcpy(c.estado, "Agendada");
    strcpy(c.observaciones, "");
    c.atendida = false;

    hospital->cantidadCitas++;
    cout << "Cita agendada correctamente con ID: " << c.id << endl;
    return &c;
}

bool cancelarCita(Hospital* hospital, int idCita) {
    Cita* c = buscarCitaPorId(hospital, idCita);
    if (!c) return false;
    strcpy(c->estado, "Cancelada");
    c->atendida = false;
    cout << "Cita cancelada correctamente" << endl;
    return true;
}

bool atenderCita(Hospital* hospital, int idCita, const char* diagnostico,
                 const char* tratamiento, const char* medicamentos) {
    Cita* cita = buscarCitaPorId(hospital, idCita);
    if (!cita || strcmp(cita->estado, "Agendada") != 0) return false;

    Paciente* p = nullptr;
    Doctor* d = nullptr;

    for (int i = 0; i < hospital->cantidadPacientes; i++)
        if (hospital->pacientes[i].id == cita->idPaciente) p = &hospital->pacientes[i];
    for (int i = 0; i < hospital->cantidadDoctores; i++)
        if (hospital->doctores[i].id == cita->idDoctor) d = &hospital->doctores[i];

    if (!p || !d) return false;

    HistorialMedico h;
    h.idConsulta = hospital->siguienteIdConsulta++;
    strcpy(h.fecha, cita->fecha);
    strcpy(h.hora, cita->hora);
    strcpy(h.diagnostico, diagnostico);
    strcpy(h.tratamiento, tratamiento);
    strcpy(h.medicamentos, medicamentos);
    h.idDoctor = cita->idDoctor;
    h.costo = d->costoConsulta;

    agregarConsultaAlHistorial(p, h);

    strcpy(cita->estado, "Atendida");
    cita->atendida = true;
    cout << "Cita atendida correctamente y agregada al historial" << endl;
    return true;
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
                 << setw(4) << c.idPaciente << " ║ "
                 << setw(4) << c.idDoctor << " ║ "
                 << setw(10) << c.fecha << " ║ "
                 << setw(7) << c.hora << " ║ "
                 << setw(7) << c.estado << "║" << endl;
        }
    }
    cout << "╚════╩══════╩══════╩════════════╩═════════╩══════╝" << endl;
}

// ============================================================
// =================== MENÚS E INTERFAZ =======================
// ============================================================

void menuPacientes(Hospital* h) {
    int op;
    do {
        cout << "╔══════════════════════════════════════╗" << endl;
        cout << "║         GESTION DE PACIENTES         ║" << endl;
        cout << "╚══════════════════════════════════════╝" << endl;
        cout << "1. Registrar nuevo paciente" << endl;
        cout << "2. Listar pacientes" << endl;
        cout << "0. Volver" << endl;
        cout << "Seleccione: ";
        cin >> op; cin.ignore();

        if (op == 1) {
            char n[50], a[50], c[20]; int e; char s;
            cout << "Nombre: "; cin.getline(n,50);
            cout << "Apellido: "; cin.getline(a,50);
            cout << "Cedula: "; cin.getline(c,20);
            cout << "Edad: "; cin >> e;
            cout << "Sexo (M/F): "; cin >> s;
            crearPaciente(h, n, a, c, e, s);
        }
        else if (op == 2) listarPacientes(h);
    } while (op != 0);
}

void menuDoctores(Hospital* h) {
    int op;
    do {
        cout << "╔══════════════════════════════════════╗" << endl;
        cout << "║          GESTION DE DOCTORES         ║" << endl;
        cout << "╚══════════════════════════════════════╝" << endl;
        cout << "1. Registrar nuevo doctor" << endl;
        cout << "2. Listar doctores" << endl;
        cout << "0. Volver" << endl;
        cout << "Seleccione: ";
        cin >> op; cin.ignore();

        if (op == 1) {
            char n[50], a[50], c[20], e[50]; int an; float cost;
            cout << "Nombre: "; cin.getline(n,50);
            cout << "Apellido: "; cin.getline(a,50);
            cout << "Cedula prof.: "; cin.getline(c,20);
            cout << "Especialidad: "; cin.getline(e,50);
            cout << "Anios de experiencia: "; cin >> an;
            cout << "Costo consulta: "; cin >> cost;
            crearDoctor(h, n, a, c, e, an, cost);
        }
        else if (op == 2) listarDoctores(h);
    } while (op != 0);
}

void menuCitas(Hospital* h) {
    int op;
    do {
        cout << "╔══════════════════════════════════════╗" << endl;
        cout << "║           GESTION DE CITAS           ║" << endl;
        cout << "╚══════════════════════════════════════╝" << endl;
        cout << "1. Agendar nueva cita" << endl;
        cout << "2. Atender cita" << endl;
        cout << "3. Cancelar cita" << endl;
        cout << "4. Ver citas pendientes" << endl;
        cout << "0. Volver" << endl;
        cout << "Seleccione: ";
        cin >> op; cin.ignore();

        if (op == 1) {
            int idP, idD;
            char fecha[11], hora[6], motivo[150];
            cout << "ID Paciente: "; cin >> idP;
            cout << "ID Doctor: "; cin >> idD;
            cin.ignore();
            cout << "Fecha (YYYY-MM-DD): "; cin.getline(fecha,11);
            cout << "Hora (HH:MM): "; cin.getline(hora,6);
            cout << "Motivo: "; cin.getline(motivo,150);
            agendarCita(h, idP, idD, fecha, hora, motivo);
        }
        else if (op == 2) {
            int id;
            cout << "ID Cita a atender: "; cin >> id; cin.ignore();
            char diag[200], trat[200], med[150];
            cout << "Diagnostico: "; cin.getline(diag,200);
            cout << "Tratamiento: "; cin.getline(trat,200);
            cout << "Medicamentos: "; cin.getline(med,150);
            atenderCita(h, id, diag, trat, med);
        }
        else if (op == 3) {
            int id;
            cout << "ID Cita a cancelar: "; cin >> id;
            cancelarCita(h, id);
        }
        else if (op == 4) listarCitasPendientes(h);
    } while (op != 0);
}

// ============================================================
// ========================= MAIN =============================
// ============================================================

int main() {
    Hospital* h = inicializarHospital("Hospital Central", 10);
    int opcion;
    do {
        cout << "╔════════════════════════════════════════╗" << endl;
        cout << "║      SISTEMA DE GESTION HOSPITALARIA   ║" << endl;
        cout << "╚════════════════════════════════════════╝" << endl;
        cout << "1. Gestion de Pacientes" << endl;
        cout << "2. Gestion de Doctores" << endl;
        cout << "3. Gestion de Citas" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione: ";
        cin >> opcion; cin.ignore();

        if (opcion == 1) menuPacientes(h);
        else if (opcion == 2) menuDoctores(h);
        else if (opcion == 3) menuCitas(h);
    } while (opcion != 0);

    destruirHospital(h);
    cout << "Saliendo del sistema..." << endl;
    return 0;
}
