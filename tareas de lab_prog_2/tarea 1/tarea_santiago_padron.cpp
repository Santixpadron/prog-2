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
