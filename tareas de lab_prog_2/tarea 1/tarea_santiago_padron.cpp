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
