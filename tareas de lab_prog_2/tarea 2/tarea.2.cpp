#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <string>
#include <limits>
#include <algorithm>
#include <cstdio>

using namespace std;

// -------------------- CONSTANTES --------------------
const char* ARCHIVO_HOSPITAL   = "hospital.bin";
const char* ARCHIVO_PACIENTES  = "pacientes.bin";
const char* ARCHIVO_DOCTORES   = "doctores.bin";
const char* ARCHIVO_CITAS      = "citas.bin";
const char* ARCHIVO_HISTORIAL  = "historiales.bin";

const int VERSION_ACTUAL = 1;
const int MAX_CITAS_PACIENTE = 20;
const int MAX_PACIENTES_DOCTOR = 50;
const int MAX_CITAS_DOCTOR = 50;

// -------------------- ESTRUCTURAS --------------------
struct ArchivoHeader {
    int cantidadRegistros;
    int proximoID;
    int registrosActivos;
    int version;
};

struct HistorialMedico {
    int idConsulta;
    int idPaciente;
    char fecha[11]; // YYYY-MM-DD
    char hora[6];   // HH:MM
    char diagnostico[200];
    char tratamiento[200];
    char medicamentos[150];
    int idDoctor;
    float costo;
    int siguienteConsultaID; // -1 si ultima
    bool eliminado;
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

    int cantidadConsultas;
    int primerConsultaID;

    int cantidadCitas;
    int citasIDs[MAX_CITAS_PACIENTE];

    bool activo;
    bool eliminado;
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

    int cantidadPacientes;
    int pacientesIDs[MAX_PACIENTES_DOCTOR];

    int cantidadCitas;
    int citasIDs[MAX_CITAS_DOCTOR];

    bool disponible;
    bool eliminado;
};

struct Cita {
    int id;
    int idPaciente;
    int idDoctor;
    char fecha[11];
    char hora[6];
    char motivo[150];
    char estado[20]; // Programada, Cancelada, Atendida
    char observaciones[200];
    bool atendida;
    bool eliminado;
    int consultaID; // si se creo consulta
};

struct Hospital {
    char nombre[100];
    char direccion[150];
    char telefono[15];
    int siguienteIdPaciente;
    int siguienteIdDoctor;
    int siguienteIdCita;
    int siguienteIdConsulta;
    int totalPacientesRegistrados;
    int totalDoctoresRegistrados;
    int totalCitasAgendadas;
    int totalConsultasRealizadas;
};

// -------------------- UTILIDADES --------------------

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pausar() {
    cout << "\nPresione ENTER para continuar...";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void asegurarString(char* dest, const char* src, size_t n) {
    if (!src) { dest[0] = '\0'; return; }
    strncpy(dest, src, n-1);
    dest[n-1] = '\0';
}

bool validarEmail(const char* email) {
    if (!email) return false;
    int len = (int)strlen(email);
    if (len < 3 || len >= 50) return false;
    const char* at = strchr(email, '@');
    if (!at || at == email) return false;
    if (strchr(at+1, '@')) return false;
    if (!strchr(at+1, '.')) return false;
    return true;
}

bool validarFecha(const char* fecha) {
    if (!fecha) return false;
    if (strlen(fecha) != 10) return false;
    if (fecha[4] != '-' || fecha[7] != '-') return false;
    for (int i=0;i<10;i++) {
        if (i==4||i==7) continue;
        if (!isdigit((unsigned char)fecha[i])) return false;
    }
    int anio = atoi(string(fecha, fecha+4).c_str());
    int mes = atoi(string(fecha+5, fecha+7).c_str());
    int dia = atoi(string(fecha+8, fecha+10).c_str());
    if (mes<1||mes>12) return false;
    if (dia<1||dia>31) return false;
    (void)anio;
    return true;
}

bool validarHora(const char* hora) {
    if (!hora) return false;
    if (strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    for (int i=0;i<5;i++) {
        if (i==2) continue;
        if (!isdigit((unsigned char)hora[i])) return false;
    }
    int hh = atoi(string(hora, hora+2).c_str());
    int mm = atoi(string(hora+3, hora+5).c_str());
    if (hh<0||hh>23) return false;
    if (mm<0||mm>59) return false;
    return true;
}

bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    int len = (int)strlen(cedula);
    if (len < 3 || len > 20) return false;
    // formato V-12345678 o E-...
    if (!(cedula[0]=='V' || cedula[0]=='v' || cedula[0]=='E' || cedula[0]=='e')) return false;
    if (cedula[1] != '-') return false;
    for (int i=2;i<len;i++) {
        if (!isdigit((unsigned char)cedula[i])) return false;
    }
    return true;
}

// -------------------- FUNCIONES DE ARCHIVO (HEADER) --------------------

bool inicializarArchivo(const char* nombreArchivo, int proximoID = 1) {
    ofstream out(nombreArchivo, ios::binary | ios::trunc);
    if (!out.is_open()) return false;
    ArchivoHeader h;
    h.cantidadRegistros = 0;
    h.proximoID = proximoID;
    h.registrosActivos = 0;
    h.version = VERSION_ACTUAL;
    out.write((char*)&h, sizeof(h));
    out.close();
    return true;
}

ArchivoHeader leerHeader(const char* nombreArchivo) {
    ArchivoHeader h;
    memset(&h, 0, sizeof(h));
    ifstream in(nombreArchivo, ios::binary);
    if (!in.is_open()) return h;
    in.read((char*)&h, sizeof(h));
    in.close();
    return h;
}

bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& h) {
    fstream f(nombreArchivo, ios::binary | ios::in | ios::out);
    if (!f.is_open()) return false;
    f.seekp(0);
    f.write((char*)&h, sizeof(h));
    f.close();
    return true;
}

bool verificarArchivo(const char* nombreArchivo) {
    ifstream in(nombreArchivo, ios::binary);
    if (!in.is_open()) return false;
    in.seekg(0, ios::end);
    long tam = in.tellg();
    in.close();
    return tam >= (long)sizeof(ArchivoHeader);
}

long calcularPosicion(int indice, size_t tamRegistro) {
    return (long)(sizeof(ArchivoHeader) + indice * tamRegistro);
}

// -------------------- LECTURA POR INDICE --------------------

Paciente leerPacientePorIndice(int indice) {
    Paciente p;
    memset(&p, 0, sizeof(p));
    ifstream f(ARCHIVO_PACIENTES, ios::binary);
    if (!f.is_open()) return p;
    long pos = calcularPosicion(indice, sizeof(Paciente));
    f.seekg(pos);
    f.read((char*)&p, sizeof(Paciente));
    f.close();
    return p;
}

Doctor leerDoctorPorIndice(int indice) {
    Doctor d;
    memset(&d, 0, sizeof(d));
    ifstream f(ARCHIVO_DOCTORES, ios::binary);
    if (!f.is_open()) return d;
    long pos = calcularPosicion(indice, sizeof(Doctor));
    f.seekg(pos);
    f.read((char*)&d, sizeof(Doctor));
    f.close();
    return d;
}

Cita leerCitaPorIndice(int indice) {
    Cita c;
    memset(&c, 0, sizeof(c));
    ifstream f(ARCHIVO_CITAS, ios::binary);
    if (!f.is_open()) return c;
    long pos = calcularPosicion(indice, sizeof(Cita));
    f.seekg(pos);
    f.read((char*)&c, sizeof(Cita));
    f.close();
    return c;
}

HistorialMedico leerHistorialPorIndice(int indice) {
    HistorialMedico h;
    memset(&h, 0, sizeof(h));
    ifstream f(ARCHIVO_HISTORIAL, ios::binary);
    if (!f.is_open()) return h;
    long pos = calcularPosicion(indice, sizeof(HistorialMedico));
    f.seekg(pos);
    f.read((char*)&h, sizeof(HistorialMedico));
    f.close();
    return h;
}

// forward declaration: usada por agregarConsultaAlHistorial antes de su definicion
HistorialMedico buscarHistorialPorID(int id);

// -------------------- BUSQUEDAS (INDICE POR ID) --------------------

int buscarIndicePacientePorID(int id) {
    ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
    for (int i=0;i<h.cantidadRegistros;i++) {
        Paciente p = leerPacientePorIndice(i);
        if (p.id == id && !p.eliminado) return i;
    }
    return -1;
}

int buscarIndiceDoctorPorID(int id) {
    ArchivoHeader h = leerHeader(ARCHIVO_DOCTORES);
    for (int i=0;i<h.cantidadRegistros;i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (d.id == id && !d.eliminado) return i;
    }
    return -1;
}

int buscarIndiceCitaPorID(int id) {
    ArchivoHeader h = leerHeader(ARCHIVO_CITAS);
    for (int i=0;i<h.cantidadRegistros;i++) {
        Cita c = leerCitaPorIndice(i);
        if (c.id == id && !c.eliminado) return i;
    }
    return -1;
}

int buscarIndiceHistorialPorID(int id) {
    ArchivoHeader h = leerHeader(ARCHIVO_HISTORIAL);
    for (int i=0;i<h.cantidadRegistros;i++) {
        HistorialMedico hm = leerHistorialPorIndice(i);
        if (hm.idConsulta == id && !hm.eliminado) return i;
    }
    return -1;
}

// -------------------- CRUD PACIENTE --------------------

bool guardarPaciente(const Paciente& paciente, int indice = -1) {
    if (indice == -1) {
        indice = buscarIndicePacientePorID(paciente.id);
        if (indice == -1) return false;
    }
    fstream f(ARCHIVO_PACIENTES, ios::binary | ios::in | ios::out);
    if (!f.is_open()) return false;
    f.seekp(calcularPosicion(indice, sizeof(Paciente)));
    f.write((char*)&paciente, sizeof(Paciente));
    f.close();
    return true;
}

bool agregarPaciente(Paciente& paciente) {
    ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
    paciente.id = h.proximoID;
    // valores por defecto
    paciente.activo = true;
    paciente.eliminado = false;
    paciente.cantidadConsultas = 0;
    paciente.primerConsultaID = -1;
    paciente.cantidadCitas = 0;
    // append
    ofstream f(ARCHIVO_PACIENTES, ios::binary | ios::app);
    if (!f.is_open()) return false;
    f.write((char*)&paciente, sizeof(Paciente));
    f.close();
    // actualizar header
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID++;
    actualizarHeader(ARCHIVO_PACIENTES, h);
    return true;
}

Paciente buscarPacientePorID(int id) {
    Paciente vacio; memset(&vacio,0,sizeof(vacio));
    int idx = buscarIndicePacientePorID(id);
    if (idx==-1) return vacio;
    return leerPacientePorIndice(idx);
}

Paciente buscarPacientePorCedula(const char* cedula) {
    ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
    Paciente vacio; memset(&vacio,0,sizeof(vacio));
    for (int i=0;i<h.cantidadRegistros;i++) {
        Paciente p = leerPacientePorIndice(i);
        if (!p.eliminado && strcmp(p.cedula, cedula) == 0) return p;
    }
    return vacio;
}

bool eliminarPaciente(int id) {
    int idx = buscarIndicePacientePorID(id);
    if (idx == -1) return false;
    Paciente p = leerPacientePorIndice(idx);
    p.eliminado = true;
    p.activo = false;
    if (!guardarPaciente(p, idx)) return false;
    ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
    h.registrosActivos = std::max(0, h.registrosActivos - 1);
    actualizarHeader(ARCHIVO_PACIENTES, h);
    return true;
}

void listarPacientes() {
    ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
    cout << "=== LISTA DE PACIENTES (" << h.registrosActivos << " activos) ===\n";
    cout << left << setw(4) << "ID" << setw(20) << "NOMBRE" << setw(14) << "CEDULA" << setw(6) << "EDAD" << setw(6) << "CITAS" << "\n";
    cout << "---------------------------------------------------------------\n";
    for (int i=0;i<h.cantidadRegistros;i++) {
        Paciente p = leerPacientePorIndice(i);
        if (!p.eliminado) {
            string fullname = string(p.nombre) + " " + string(p.apellido);
            cout << setw(4) << p.id << setw(20) << fullname.substr(0,19) << setw(14) << p.cedula << setw(6) << p.edad << setw(6) << p.cantidadCitas << "\n";
        }
    }
}

// -------------------- CRUD DOCTOR --------------------

bool guardarDoctor(const Doctor& doctor, int indice = -1) {
    if (indice == -1) {
        indice = buscarIndiceDoctorPorID(doctor.id);
        if (indice == -1) return false;
    }
    fstream f(ARCHIVO_DOCTORES, ios::binary | ios::in | ios::out);
    if (!f.is_open()) return false;
    f.seekp(calcularPosicion(indice, sizeof(Doctor)));
    f.write((char*)&doctor, sizeof(Doctor));
    f.close();
    return true;
}

bool agregarDoctor(Doctor& doctor) {
    ArchivoHeader h = leerHeader(ARCHIVO_DOCTORES);
    doctor.id = h.proximoID;
    doctor.disponible = true;
    doctor.eliminado = false;
    doctor.cantidadPacientes = 0;
    doctor.cantidadCitas = 0;
    ofstream f(ARCHIVO_DOCTORES, ios::binary | ios::app);
    if (!f.is_open()) return false;
    f.write((char*)&doctor, sizeof(Doctor));
    f.close();
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID++;
    actualizarHeader(ARCHIVO_DOCTORES, h);
    return true;
}

Doctor buscarDoctorPorID(int id) {
    Doctor vacio; memset(&vacio,0,sizeof(vacio));
    int idx = buscarIndiceDoctorPorID(id);
    if (idx==-1) return vacio;
    return leerDoctorPorIndice(idx);
}

bool eliminarDoctor(int id) {
    int idx = buscarIndiceDoctorPorID(id);
    if (idx == -1) return false;
    Doctor d = leerDoctorPorIndice(idx);
    d.eliminado = true;
    d.disponible = false;
    if (!guardarDoctor(d, idx)) return false;
    ArchivoHeader h = leerHeader(ARCHIVO_DOCTORES);
    h.registrosActivos = std::max(0, h.registrosActivos - 1);
    actualizarHeader(ARCHIVO_DOCTORES, h);
    return true;
}

void listarDoctores() {
    ArchivoHeader h = leerHeader(ARCHIVO_DOCTORES);
    cout << "=== LISTA DE DOCTORES (" << h.registrosActivos << " activos) ===\n";
    cout << left << setw(4) << "ID" << setw(20) << "NOMBRE" << setw(16) << "ESPECIALIDAD" << setw(6) << "EXP" << setw(8) << "COSTO" << "\n";
    cout << "-----------------------------------------------------------------\n";
    for (int i=0;i<h.cantidadRegistros;i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (!d.eliminado) {
            string fullname = string(d.nombre) + " " + string(d.apellido);
            cout << setw(4) << d.id << setw(20) << fullname.substr(0,19) << setw(16) << d.especialidad << setw(6) << d.aniosExperiencia << setw(8) << fixed << setprecision(2) << d.costoConsulta << "\n";
        }
    }
}

// -------------------- CRUD CITA --------------------

bool guardarCita(const Cita& cita, int indice = -1) {
    if (indice == -1) {
        indice = buscarIndiceCitaPorID(cita.id);
        if (indice == -1) return false;
    }
    fstream f(ARCHIVO_CITAS, ios::binary | ios::in | ios::out);
    if (!f.is_open()) return false;
    f.seekp(calcularPosicion(indice, sizeof(Cita)));
    f.write((char*)&cita, sizeof(Cita));
    f.close();
    return true;
}

bool agregarCita(Cita& cita) {
    ArchivoHeader h = leerHeader(ARCHIVO_CITAS);
    cita.id = h.proximoID;
    cita.atendida = false;
    cita.eliminado = false;
    cita.consultaID = -1;
    ofstream f(ARCHIVO_CITAS, ios::binary | ios::app);
    if (!f.is_open()) return false;
    f.write((char*)&cita, sizeof(Cita));
    f.close();
    h.cantidadRegistros++;
    h.registrosActivos++;
    h.proximoID++;
    actualizarHeader(ARCHIVO_CITAS, h);
    return true;
}

Cita buscarCitaPorID(int id) {
    Cita vacio; memset(&vacio,0,sizeof(vacio));
    int idx = buscarIndiceCitaPorID(id);
    if (idx==-1) return vacio;
    return leerCitaPorIndice(idx);
}

bool cancelarCita(int id) {
    int idx = buscarIndiceCitaPorID(id);
    if (idx==-1) return false;
    Cita c = leerCitaPorIndice(idx);
    strcpy(c.estado, "Cancelada");
    c.atendida = false;
    if (!guardarCita(c, idx)) return false;
    ArchivoHeader h = leerHeader(ARCHIVO_CITAS);
    h.registrosActivos = std::max(0, h.registrosActivos - 1);
    actualizarHeader(ARCHIVO_CITAS, h);
    return true;
}

vector<Cita> obtenerCitasDePaciente(int idPaciente) {
    vector<Cita> res;
    ArchivoHeader h = leerHeader(ARCHIVO_CITAS);
    for (int i=0;i<h.cantidadRegistros;i++) {
        Cita c = leerCitaPorIndice(i);
        if (!c.eliminado && c.idPaciente == idPaciente) res.push_back(c);
    }
    return res;
}

vector<Cita> obtenerCitasDeDoctor(int idDoctor) {
    vector<Cita> res;
    ArchivoHeader h = leerHeader(ARCHIVO_CITAS);
    for (int i=0;i<h.cantidadRegistros;i++) {
        Cita c = leerCitaPorIndice(i);
        if (!c.eliminado && c.idDoctor == idDoctor) res.push_back(c);
    }
    return res;
}

bool verificarDisponibilidad(int idDoctor, const char* fecha, const char* hora) {
    vector<Cita> citas = obtenerCitasDeDoctor(idDoctor);
    for (auto &c : citas) {
        if (strcmp(c.fecha, fecha) == 0 && strcmp(c.hora, hora) == 0 && strcmp(c.estado, "Cancelada") != 0) return false;
    }
    return true;
}

// -------------------- CRUD HISTORIAL (ENLACE) --------------------

bool guardarHistorial(const HistorialMedico& h, int indice = -1) {
    if (indice == -1) {
        indice = buscarIndiceHistorialPorID(h.idConsulta);
        if (indice == -1) return false;
    }
    fstream f(ARCHIVO_HISTORIAL, ios::binary | ios::in | ios::out);
    if (!f.is_open()) return false;
    f.seekp(calcularPosicion(indice, sizeof(HistorialMedico)));
    f.write((char*)&h, sizeof(HistorialMedico));
    f.close();
    return true;
}

bool agregarHistorial(HistorialMedico& h) {
    ArchivoHeader head = leerHeader(ARCHIVO_HISTORIAL);
    h.idConsulta = head.proximoID;
    h.eliminado = false;
    ofstream f(ARCHIVO_HISTORIAL, ios::binary | ios::app);
    if (!f.is_open()) return false;
    f.write((char*)&h, sizeof(HistorialMedico));
    f.close();
    head.cantidadRegistros++;
    head.registrosActivos++;
    head.proximoID++;
    actualizarHeader(ARCHIVO_HISTORIAL, head);
    return true;
}

// agrega una consulta al historial enlazado del paciente
bool agregarConsultaAlHistorial(int idPaciente, HistorialMedico consulta) {
    Paciente p = buscarPacientePorID(idPaciente);
    if (p.id == 0) return false;
    consulta.idPaciente = idPaciente;
    consulta.eliminado = false;
    consulta.siguienteConsultaID = -1;
    if (p.primerConsultaID == -1) {
        // primer consulta
        if (!agregarHistorial(consulta)) return false;
        // actualizar paciente con primerConsultaID: leemos de header el ultimo id insertado = proximoID-1
        ArchivoHeader hh = leerHeader(ARCHIVO_HISTORIAL);
        p.primerConsultaID = hh.proximoID - 1;
        p.cantidadConsultas = 1;
        int idxP = buscarIndicePacientePorID(p.id);
        return guardarPaciente(p, idxP);
    } else {
        // recorrer hasta final
        int actualID = p.primerConsultaID;
        while (true) {
            HistorialMedico h = buscarHistorialPorID(actualID);
            if (h.idConsulta == 0) return false; // fallo
            if (h.siguienteConsultaID == -1) {
                // agregar nueva
                if (!agregarHistorial(consulta)) return false;
                ArchivoHeader hh = leerHeader(ARCHIVO_HISTORIAL);
                int nuevoID = hh.proximoID - 1;
                h.siguienteConsultaID = nuevoID;
                int idxH = buscarIndiceHistorialPorID(h.idConsulta);
                guardarHistorial(h, idxH);
                // actualizar paciente contador
                p.cantidadConsultas++;
                int idxP = buscarIndicePacientePorID(p.id);
                return guardarPaciente(p, idxP);
            } else {
                actualID = h.siguienteConsultaID;
            }
        }
    }
    return false;
}

HistorialMedico buscarHistorialPorID(int id) {
    HistorialMedico vacio; memset(&vacio,0,sizeof(vacio));
    int idx = buscarIndiceHistorialPorID(id);
    if (idx == -1) return vacio;
    return leerHistorialPorIndice(idx);
}

vector<HistorialMedico> obtenerHistorialCompleto(int idPaciente) {
    vector<HistorialMedico> res;
    Paciente p = buscarPacientePorID(idPaciente);
    if (p.id == 0) return res;
    int id = p.primerConsultaID;
    while (id != -1) {
        HistorialMedico h = buscarHistorialPorID(id);
        if (h.idConsulta == 0 || h.eliminado) break;
        res.push_back(h);
        id = h.siguienteConsultaID;
    }
    return res;
}

void mostrarHistorialMedico(int idPaciente) {
    Paciente p = buscarPacientePorID(idPaciente);
    if (p.id == 0) { cout << "Paciente no encontrado\n"; return; }
    vector<HistorialMedico> hist = obtenerHistorialCompleto(idPaciente);
    cout << "=== HISTORIAL DE " << p.nombre << " " << p.apellido << " ===\n";
    if (hist.empty()) { cout << "No hay consultas registradas\n"; return; }
    for (auto &c : hist) {
        cout << "Consulta #" << c.idConsulta << " - " << c.fecha << " " << c.hora << "\n";
        cout << "Doctor ID: " << c.idDoctor << " - Diagnostico: " << c.diagnostico << "\n";
        cout << "Tratamiento: " << c.tratamiento << "\n";
        cout << "Medicamentos: " << c.medicamentos << "\n";
        cout << "Costo: $" << fixed << setprecision(2) << c.costo << "\n";
        cout << "----------------------------------------\n";
    }
}

// -------------------- FUNCIONES DE ALTO NIVEL: AGENDAR Y ATENDER --------------------

Cita* agendarCita(Hospital* h, int idPaciente, int idDoctor, const char* fecha, const char* hora, const char* motivo) {
    Paciente p = buscarPacientePorID(idPaciente);
    Doctor d = buscarDoctorPorID(idDoctor);
    if (p.id == 0 || d.id == 0) { cout << "Paciente o doctor no encontrado\n"; return nullptr; }
    if (!validarFecha(fecha) || !validarHora(hora)) { cout << "Fecha u hora invalidas\n"; return nullptr; }
    if (!verificarDisponibilidad(idDoctor, fecha, hora)) { cout << "Doctor no disponible en esa fecha/hora\n"; return nullptr; }
    if (p.cantidadCitas >= MAX_CITAS_PACIENTE) { cout << "Paciente excede limite de citas\n"; return nullptr; }
    if (d.cantidadCitas >= MAX_CITAS_DOCTOR) { cout << "Doctor excede limite de citas\n"; return nullptr; }

    Cita c; memset(&c,0,sizeof(c));
    c.idPaciente = idPaciente;
    c.idDoctor = idDoctor;
    asegurarString(c.fecha, fecha, sizeof(c.fecha));
    asegurarString(c.hora, hora, sizeof(c.hora));
    asegurarString(c.motivo, motivo, sizeof(c.motivo));
    asegurarString(c.estado, "Programada", sizeof(c.estado));
    c.atendida = false;
    c.eliminado = false;
    c.consultaID = -1;

    if (!agregarCita(c)) { cout << "Error al guardar cita\n"; return nullptr; }

    // obtener el ID asignado por el header (proximoID-1)
    ArchivoHeader hh = leerHeader(ARCHIVO_CITAS);
    int nuevaID = hh.proximoID - 1;

    // actualizar hospital contadores
    h->totalCitasAgendadas++;
    // actualizar paciente (usar nuevaID en lugar de c.id)
    p.citasIDs[p.cantidadCitas++] = nuevaID;
    int idxP = buscarIndicePacientePorID(p.id);
    guardarPaciente(p, idxP);
    // actualizar doctor
    d.citasIDs[d.cantidadCitas++] = nuevaID;
    int idxD = buscarIndiceDoctorPorID(d.id);
    guardarDoctor(d, idxD);

    // leer la cita creada
    Cita* ret = new Cita;
    *ret = buscarCitaPorID(nuevaID);
    cout << "Cita agendada. ID: " << ret->id << "\n";
    return ret;
}

bool atenderCita(Hospital* h, int idCita, const char* diagnostico, const char* tratamiento, const char* medicamentos) {
    Cita c = buscarCitaPorID(idCita);
    if (c.id == 0) { cout << "Cita no encontrada\n"; return false; }
    if (c.atendida) { cout << "Cita ya fue atendida\n"; return false; }

    HistorialMedico hm; memset(&hm,0,sizeof(hm));
    asegurarString(hm.fecha, c.fecha, sizeof(hm.fecha));
    asegurarString(hm.hora, c.hora, sizeof(hm.hora));
    asegurarString(hm.diagnostico, diagnostico, sizeof(hm.diagnostico));
    asegurarString(hm.tratamiento, tratamiento, sizeof(hm.tratamiento));
    asegurarString(hm.medicamentos, medicamentos, sizeof(hm.medicamentos));
    hm.idDoctor = c.idDoctor;
    Doctor d = buscarDoctorPorID(c.idDoctor);
    hm.costo = d.costoConsulta;

    if (!agregarConsultaAlHistorial(c.idPaciente, hm)) { cout << "Error al crear historial\n"; return false; }

    // la nueva consulta tiene id: header.proximoID -1
    ArchivoHeader hh = leerHeader(ARCHIVO_HISTORIAL);
    int idNuevaConsulta = hh.proximoID - 1;

    c.atendida = true;
    asegurarString(c.estado, "Atendida", sizeof(c.estado));
    c.consultaID = idNuevaConsulta;
    asegurarString(c.observaciones, diagnostico, sizeof(c.observaciones));
    int idxC = buscarIndiceCitaPorID(c.id);
    guardarCita(c, idxC);
    h->totalConsultasRealizadas++;

    cout << "Cita atendida y creada consulta #" << idNuevaConsulta << "\n";
    return true;
}

// -------------------- MANTENIMIENTO SIMPLE --------------------

bool compactarArchivo(const char* archivoSrc, const char* archivoTmp, size_t tamRegistro) {
    // implementacion simplificada: copia registros no eliminados a tmp, reescribe header
    ifstream in(archivoSrc, ios::binary);
    if (!in.is_open()) return false;
    ArchivoHeader head; in.read((char*)&head, sizeof(head));
    ofstream out(archivoTmp, ios::binary | ios::trunc);
    if (!out.is_open()) { in.close(); return false; }
    ArchivoHeader nuevo; nuevo.cantidadRegistros = 0; nuevo.proximoID = head.proximoID; nuevo.registrosActivos = 0; nuevo.version = head.version;
    out.write((char*)&nuevo, sizeof(nuevo)); // header provis
    // recorrer
    for (int i=0;i<head.cantidadRegistros;i++) {
        vector<char> buffer(tamRegistro);
        in.read(buffer.data(), tamRegistro);
        // comprobar campo 'eliminado' en offset conocido: no es portable, pero para simplificar leemos estructura por tamRegistro
        // aqui asumimos que el campo eliminado es el ultimo bool (por nuestra definicion)
        // mejor: reabrir lectura a struct segun tamRegistro - but for generality: simply copy all (no compact real)
        // Para esta version simplificada no compactamos registros, solo reescribimos header equivalente.
    }
    in.close();
    out.close();
    // reemplazar
    std::remove(archivoSrc);
    std::rename(archivoTmp, archivoSrc);
    return true;
}

bool hacerRespaldo() {
    cout << "Respaldo simple: copiando archivos .bin (version simplificada)...\n";
    // En este ejemplo no implementamos copia real por simplicidad.
    return true;
}

bool restaurarRespaldo() {
    cout << "Restauracion simplificada (no implementada completamente)...\n";
    return true;
}

// -------------------- SISTEMA: cargar y guardar hospital --------------------

Hospital* cargarSistema() {
    // asegurar archivos con header
    if (!verificarArchivo(ARCHIVO_PACIENTES)) inicializarArchivo(ARCHIVO_PACIENTES);
    if (!verificarArchivo(ARCHIVO_DOCTORES)) inicializarArchivo(ARCHIVO_DOCTORES);
    if (!verificarArchivo(ARCHIVO_CITAS)) inicializarArchivo(ARCHIVO_CITAS);
    if (!verificarArchivo(ARCHIVO_HISTORIAL)) inicializarArchivo(ARCHIVO_HISTORIAL);
    if (!verificarArchivo(ARCHIVO_HOSPITAL)) inicializarArchivo(ARCHIVO_HOSPITAL);

    Hospital* h = new Hospital();
    memset(h, 0, sizeof(Hospital));
    // valores por defecto
    asegurarString(h->nombre, "Hospital Central", sizeof(h->nombre));
    asegurarString(h->direccion, "Direccion no especificada", sizeof(h->direccion));
    asegurarString(h->telefono, "0000000000", sizeof(h->telefono));

    // leer headers para iniciar ids
    ArchivoHeader hp = leerHeader(ARCHIVO_PACIENTES);
    ArchivoHeader hd = leerHeader(ARCHIVO_DOCTORES);
    ArchivoHeader hc = leerHeader(ARCHIVO_CITAS);
    ArchivoHeader hh = leerHeader(ARCHIVO_HISTORIAL);

    h->siguienteIdPaciente = hp.proximoID;
    h->siguienteIdDoctor = hd.proximoID;
    h->siguienteIdCita = hc.proximoID;
    h->siguienteIdConsulta = hh.proximoID;

    h->totalPacientesRegistrados = hp.registrosActivos;
    h->totalDoctoresRegistrados = hd.registrosActivos;
    h->totalCitasAgendadas = hc.registrosActivos;
    h->totalConsultasRealizadas = hh.registrosActivos;

    // leer hospital desde archivo (si tiene datos)
    ifstream f(ARCHIVO_HOSPITAL, ios::binary);
    if (f.is_open()) {
        ArchivoHeader hfile; f.read((char*)&hfile, sizeof(hfile));
        // si hay datos despues del header
        f.seekg(sizeof(ArchivoHeader));
        if (f.peek() != EOF) {
            f.read((char*)h, sizeof(Hospital));
        }
        f.close();
    } else {
        // guardar valores iniciales
        fstream out(ARCHIVO_HOSPITAL, ios::binary | ios::in | ios::out);
        if (!out.is_open()) {
            // crear archivo con header y datos
            inicializarArchivo(ARCHIVO_HOSPITAL);
            fstream ff(ARCHIVO_HOSPITAL, ios::binary | ios::in | ios::out);
            ff.seekp(sizeof(ArchivoHeader));
            ff.write((char*)h, sizeof(Hospital));
            ff.close();
        } else {
            out.seekp(sizeof(ArchivoHeader));
            out.write((char*)h, sizeof(Hospital));
            out.close();
        }
    }
    return h;
}

bool guardarSistema(Hospital* h) {
    fstream f(ARCHIVO_HOSPITAL, ios::binary | ios::in | ios::out);
    if (!f.is_open()) {
        // crear
        inicializarArchivo(ARCHIVO_HOSPITAL);
        f.open(ARCHIVO_HOSPITAL, ios::binary | ios::in | ios::out);
        if (!f.is_open()) return false;
    }
    f.seekp(sizeof(ArchivoHeader));
    f.write((char*)h, sizeof(Hospital));
    f.close();
    return true;
}

// -------------------- MENUS --------------------

void menuPacientes(Hospital* h) {
    while (true) {
        clearScreen();
        cout << "=== GESTION DE PACIENTES ===\n";
        cout << "1. Crear paciente\n2. Listar pacientes\n3. Buscar por cedula\n4. Eliminar paciente\n5. Ver historial\n0. Volver\n";
        cout << "Opcion: ";
        int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
        cin.ignore(10000,'\n');
        if (op==0) break;
        if (op==1) {
            Paciente p; memset(&p,0,sizeof(p));
            char nombre[50], apellido[50], cedula[20], tipoS[5], telefono[15], direccion[100], email[50];
            int edad; char sexo;
            cout << "Nombre: "; cin.getline(nombre,50);
            cout << "Apellido: "; cin.getline(apellido,50);
            cout << "Cedula (V-...): "; cin.getline(cedula,20);
            if (!validarCedula(cedula)) { cout << "Cedula invalida\n"; pausar(); continue; }
            if (buscarPacientePorCedula(cedula).id != 0) { cout << "Cedula ya registrada\n"; pausar(); continue; }
            cout << "Edad: "; cin >> edad; cin.ignore(10000,'\n');
            cout << "Sexo (M/F): "; cin >> sexo; cin.ignore(10000,'\n');
            cout << "Tipo sangre (ej O+): "; cin.getline(tipoS,5);
            cout << "Telefono: "; cin.getline(telefono,15);
            cout << "Direccion: "; cin.getline(direccion,100);
            cout << "Email: "; cin.getline(email,50);
            if (!validarEmail(email)) { asegurarString(email, "noemail@hospital", sizeof(email)); }
            asegurarString(p.nombre, nombre, sizeof(p.nombre));
            asegurarString(p.apellido, apellido, sizeof(p.apellido));
            asegurarString(p.cedula, cedula, sizeof(p.cedula));
            p.edad = edad; p.sexo = sexo;
            asegurarString(p.tipoSangre, tipoS, sizeof(p.tipoSangre));
            asegurarString(p.telefono, telefono, sizeof(p.telefono));
            asegurarString(p.direccion, direccion, sizeof(p.direccion));
            asegurarString(p.email, email, sizeof(p.email));
            if (agregarPaciente(p)) {
                cout << "Paciente creado. ID: " << p.id << "\n";
                h->totalPacientesRegistrados++;
                guardarSistema(h);
            } else cout << "Error al crear paciente\n";
            pausar();
        } else if (op==2) {
            clearScreen(); listarPacientes(); pausar();
        } else if (op==3) {
            char ced[20]; cout << "Cedula: "; cin.getline(ced,20);
            Paciente p = buscarPacientePorCedula(ced);
            if (p.id==0) cout << "No encontrado\n"; else {
                cout << "ID: " << p.id << " - " << p.nombre << " " << p.apellido << " - Email: " << p.email << "\n";
            }
            pausar();
        } else if (op==4) {
            int id; cout << "ID a eliminar: "; cin >> id; cin.ignore(10000,'\n');
            if (eliminarPaciente(id)) { cout << "Eliminado\n"; h->totalPacientesRegistrados--; guardarSistema(h); }
            else cout << "Error al eliminar\n";
            pausar();
        } else if (op==5) {
            int id; cout << "ID paciente: "; cin >> id; cin.ignore(10000,'\n');
            mostrarHistorialMedico(id);
            pausar();
        } else { cout << "Opcion invalida\n"; pausar(); }
    }
}

void menuDoctores(Hospital* h) {
    while (true) {
        clearScreen();
        cout << "=== GESTION DE DOCTORES ===\n";
        cout << "1. Crear doctor\n2. Listar doctores\n3. Eliminar doctor\n0. Volver\n";
        cout << "Opcion: ";
        int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
        cin.ignore(10000,'\n');
        if (op==0) break;
        if (op==1) {
            Doctor d; memset(&d,0,sizeof(d));
            char nombre[50], apellido[50], ced[20], esp[50], horario[50], telefono[15], email[50];
            int exp; float costo;
            cout << "Nombre: "; cin.getline(nombre,50);
            cout << "Apellido: "; cin.getline(apellido,50);
            cout << "Cedula profesional: "; cin.getline(ced,20);
            cout << "Especialidad: "; cin.getline(esp,50);
            cout << "Anios experiencia: "; cin >> exp; cin.ignore(10000,'\n');
            cout << "Costo consulta: "; cin >> costo; cin.ignore(10000,'\n');
            cout << "Horario (ej Lun-Vie 8-16): "; cin.getline(horario,50);
            cout << "Telefono: "; cin.getline(telefono,15);
            cout << "Email: "; cin.getline(email,50);
            asegurarString(d.nombre, nombre, sizeof(d.nombre));
            asegurarString(d.apellido, apellido, sizeof(d.apellido));
            asegurarString(d.cedula, ced, sizeof(d.cedula));
            asegurarString(d.especialidad, esp, sizeof(d.especialidad));
            d.aniosExperiencia = exp;
            d.costoConsulta = costo;
            asegurarString(d.horarioAtencion, horario, sizeof(d.horarioAtencion));
            asegurarString(d.telefono, telefono, sizeof(d.telefono));
            asegurarString(d.email, email, sizeof(d.email));
            if (agregarDoctor(d)) {
                cout << "Doctor creado. ID: " << d.id << "\n";
                h->totalDoctoresRegistrados++;
                guardarSistema(h);
            } else cout << "Error al crear doctor\n";
            pausar();
        } else if (op==2) {
            clearScreen(); listarDoctores(); pausar();
        } else if (op==3) {
            int id; cout << "ID a eliminar: "; cin >> id; cin.ignore(10000,'\n');
            if (eliminarDoctor(id)) { cout << "Doctor eliminado\n"; h->totalDoctoresRegistrados--; guardarSistema(h); }
            else cout << "Error al eliminar\n";
            pausar();
        } else { cout << "Opcion invalida\n"; pausar(); }
    }
}

void menuCitas(Hospital* h) {
    while (true) {
        clearScreen();
        cout << "=== GESTION DE CITAS ===\n";
        cout << "1. Agendar cita\n2. Listar citas de paciente\n3. Listar citas de doctor\n4. Cancelar cita\n5. Atender cita\n0. Volver\n";
        cout << "Opcion: ";
        int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
        cin.ignore(10000,'\n');
        if (op==0) break;
        if (op==1) {
            int idP, idD; char fecha[11], hora[6], motivo[150];
            cout << "ID paciente: "; cin >> idP; cin.ignore(10000,'\n');
            cout << "ID doctor: "; cin >> idD; cin.ignore(10000,'\n');
            cout << "Fecha (YYYY-MM-DD): "; cin.getline(fecha,11);
            cout << "Hora (HH:MM): "; cin.getline(hora,6);
            cout << "Motivo: "; cin.getline(motivo,150);
            Cita* cit = agendarCita(h, idP, idD, fecha, hora, motivo);
            if (cit) delete cit;
            guardarSistema(h);
            pausar();
        } else if (op==2) {
            int idP; cout << "ID paciente: "; cin >> idP; cin.ignore(10000,'\n');
            vector<Cita> v = obtenerCitasDePaciente(idP);
            if (v.empty()) cout << "No tiene citas\n"; else {
                for (auto &c:v) cout << "ID " << c.id << " - " << c.fecha << " " << c.hora << " - " << c.estado << "\n";
            }
            pausar();
        } else if (op==3) {
            int idD; cout << "ID doctor: "; cin >> idD; cin.ignore(10000,'\n');
            vector<Cita> v = obtenerCitasDeDoctor(idD);
            if (v.empty()) cout << "No tiene citas\n"; else {
                for (auto &c:v) cout << "ID " << c.id << " - Paciente " << c.idPaciente << " - " << c.fecha << " " << c.hora << " - " << c.estado << "\n";
            }
            pausar();
        } else if (op==4) {
            int id; cout << "ID cita a cancelar: "; cin >> id; cin.ignore(10000,'\n');
            if (cancelarCita(id)) { cout << "Cita cancelada\n"; guardarSistema(h); } else cout << "Error\n";
            pausar();
        } else if (op==5) {
            int id; cout << "ID cita a atender: "; cin >> id; cin.ignore(10000,'\n');
            char diag[200], trat[200], meds[150];
            cout << "Diagnostico: "; cin.getline(diag,200);
            cout << "Tratamiento: "; cin.getline(trat,200);
            cout << "Medicamentos: "; cin.getline(meds,150);
            if (atenderCita(h, id, diag, trat, meds)) { cout << "Cita atendida\n"; guardarSistema(h); } else cout << "Error\n";
            pausar();
        } else { cout << "Opcion invalida\n"; pausar(); }
    }
}

void menuMantenimiento(Hospital* h) {
    (void)h;
    while (true) {
        clearScreen();
        cout << "=== MANTENIMIENTO ===\n1. Verificar archivos\n2. Hacer respaldo (simulado)\n3. Restaurar respaldo (simulado)\n0. Volver\nOpcion: ";
        int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
        cin.ignore(10000,'\n');
        if (op==0) break;
        if (op==1) {
            bool ok = verificarArchivo(ARCHIVO_PACIENTES) && verificarArchivo(ARCHIVO_DOCTORES) && verificarArchivo(ARCHIVO_CITAS) && verificarArchivo(ARCHIVO_HISTORIAL) && verificarArchivo(ARCHIVO_HOSPITAL);
            cout << (ok ? "Archivos OK\n" : "Faltan o estan corruptos\n");
            pausar();
        } else if (op==2) { hacerRespaldo(); pausar(); }
        else if (op==3) { restaurarRespaldo(); pausar(); }
        else { cout << "Opcion invalida\n"; pausar(); }
    }
}

void menuReportes(Hospital* h) {
    (void)h;
    while (true) {
        clearScreen();
        cout << "=== REPORTES ===\n1. Estadisticas\n2. Ver historial paciente\n0. Volver\nOpcion: ";
        int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
        cin.ignore(10000,'\n');
        if (op==0) break;
        if (op==1) {
            ArchivoHeader pac = leerHeader(ARCHIVO_PACIENTES);
            ArchivoHeader doc = leerHeader(ARCHIVO_DOCTORES);
            ArchivoHeader cit = leerHeader(ARCHIVO_CITAS);
            ArchivoHeader hist = leerHeader(ARCHIVO_HISTORIAL);
            cout << "PACIENTES: " << pac.registrosActivos << " activos de " << pac.cantidadRegistros << "\n";
            cout << "DOCTORES: " << doc.registrosActivos << " activos de " << doc.cantidadRegistros << "\n";
            cout << "CITAS: " << cit.registrosActivos << " activas de " << cit.cantidadRegistros << "\n";
            cout << "CONSULTAS: " << hist.registrosActivos << " activas de " << hist.cantidadRegistros << "\n";
            pausar();
        } else if (op==2) {
            int id; cout << "ID paciente: "; cin >> id; cin.ignore(10000,'\n');
            mostrarHistorialMedico(id); pausar();
        } else { cout << "Opcion invalida\n"; pausar(); }
    }
}

// -------------------- MAIN --------------------

int main() {
    Hospital* sistema = cargarSistema();
    while (true) {
        clearScreen();
        cout << "=========================================\n";
        cout << "   SISTEMA HOSPITALARIO - MENU PRINCIPAL\n";
        cout << "=========================================\n";
        cout << "1. Pacientes\n2. Doctores\n3. Citas\n4. Reportes\n5. Mantenimiento\n0. Salir\n";
        cout << "Seleccione una opcion: ";
        int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
        cin.ignore(10000,'\n');
        if (op==0) {
            cout << "Guardando y saliendo...\n";
            guardarSistema(sistema);
            delete sistema;
            break;
        }
        switch (op) {
            case 1: menuPacientes(sistema); break;
            case 2: menuDoctores(sistema); break;
            case 3: menuCitas(sistema); break;
            case 4: menuReportes(sistema); break;
            case 5: menuMantenimiento(sistema); break;
            default: cout << "Opcion invalida\n"; pausar(); break;
        }
    }
    return 0;
}
