#include <iostream>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <fstream>
#include <vector>
#include <cstdlib>  // Para system()

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
    int idPaciente;                 // NUEVO: Referencia al paciente
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
const int MAX_CITAS_PACIENTE = 20;
const int MAX_PACIENTES_DOCTOR = 50;
const int MAX_CITAS_DOCTOR = 30;

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
int buscarIndiceHistorialPorID(int id);

// Funciones CRUD para pacientes
bool guardarPaciente(const Paciente& paciente, int indice = -1);
bool agregarPaciente(Paciente& paciente);
Paciente buscarPacientePorID(int id);
Paciente buscarPacientePorCedula(const char* cedula);
bool actualizarPaciente(int id, const Paciente& nuevosDatos);
bool eliminarPaciente(int id);

// Funciones CRUD para doctores
bool guardarDoctor(const Doctor& doctor, int indice = -1);
bool agregarDoctor(Doctor& doctor);
Doctor buscarDoctorPorID(int id);
bool actualizarDoctor(int id, const Doctor& nuevosDatos);
bool eliminarDoctor(int id);

// Funciones CRUD para citas
bool guardarCita(const Cita& cita, int indice = -1);
bool agregarCita(Cita& cita);
Cita buscarCitaPorID(int id);
bool actualizarCita(int id, const Cita& nuevosDatos);
bool cancelarCita(int id);

// Funciones de historial médico
bool guardarHistorial(const HistorialMedico& historial, int indice = -1);
bool agregarHistorial(HistorialMedico& historial);
HistorialMedico buscarHistorialPorID(int id);
bool agregarConsultaAlHistorial(int idPaciente, const HistorialMedico& consulta);
vector<HistorialMedico> obtenerHistorialCompleto(int idPaciente);
void mostrarHistorialMedico(int idPaciente);
HistorialMedico* obtenerUltimaConsulta(int idPaciente);

// Funciones de citas
Cita* agendarCita(Hospital* hospital, int idPaciente, int idDoctor,
                  const char* fecha, const char* hora, const char* motivo);
bool atenderCita(Hospital* hospital, int idCita, const char* diagnostico,
                 const char* tratamiento, const char* medicamentos);
vector<Cita> obtenerCitasDePaciente(int idPaciente);
vector<Cita> obtenerCitasDeDoctor(int idDoctor);
vector<Cita> obtenerCitasPorFecha(const char* fecha);
bool verificarDisponibilidad(int idDoctor, const char* fecha, const char* hora);
void listarCitasPendientes();

// Funciones de búsqueda avanzada
vector<Paciente> buscarPacientesPorNombre(const char* nombre);
vector<Doctor> buscarDoctoresPorEspecialidad(const char* especialidad);
vector<Doctor> buscarDoctoresPorNombre(const char* nombre);

// Funciones de relaciones
bool asignarPacienteADoctor(int idDoctor, int idPaciente);
bool removerPacienteDeDoctor(int idDoctor, int idPaciente);
void listarPacientesDeDoctor(int idDoctor);

// Funciones de validación
bool validarEmail(const char* email);
bool validarFecha(const char* fecha);
bool validarHora(const char* hora);
int compararFechas(const char* fecha1, const char* fecha2);

// Funciones de mantenimiento
bool compactarArchivoPacientes();
bool compactarArchivoDoctores();
bool compactarArchivoCitas();
bool verificarIntegridadReferencial();
bool hacerRespaldo();
bool restaurarRespaldo();

// Funciones del sistema
Hospital* cargarSistema();
bool guardarSistema(Hospital* hospital);
void limpiarSistema(Hospital* hospital);

// Funciones de utilidad
void limpiarBufferEntrada();
void leerLinea(char* buffer, int size);
int leerEntero();
char* copiarString(const char* origen);

// Funciones de menú
void menuPacientes(Hospital* h);
void menuDoctores(Hospital* h);
void menuCitas(Hospital* h);
void menuMantenimiento(Hospital* h);
void menuReportes(Hospital* h);
void mostrarDatosHospital(Hospital* h);

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

HistorialMedico leerHistorialPorIndice(int indice) {
    HistorialMedico historial;
    memset(&historial, 0, sizeof(HistorialMedico));
    
    ifstream archivo(ARCHIVO_HISTORIAL, ios::binary);
    if (archivo.is_open()) {
        long posicion = calcularPosicion(indice, sizeof(HistorialMedico));
        archivo.seekg(posicion);
        archivo.read((char*)&historial, sizeof(HistorialMedico));
        archivo.close();
    }
    return historial;
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

int buscarIndiceCitaPorID(int id) {
    ArchivoHeader header = leerHeader(ARCHIVO_CITAS);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita c = leerCitaPorIndice(i);
        if (c.id == id && !c.eliminado) {
            return i;
        }
    }
    return -1;
}

int buscarIndiceHistorialPorID(int id) {
    ArchivoHeader header = leerHeader(ARCHIVO_HISTORIAL);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        HistorialMedico h = leerHistorialPorIndice(i);
        if (h.idConsulta == id && !h.eliminado) {
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

Cita buscarCitaPorID(int id) {
    int indice = buscarIndiceCitaPorID(id);
    if (indice != -1) {
        return leerCitaPorIndice(indice);
    }
    
    Cita vacio;
    memset(&vacio, 0, sizeof(Cita));
    return vacio;
}

HistorialMedico buscarHistorialPorID(int id) {
    int indice = buscarIndiceHistorialPorID(id);
    if (indice != -1) {
        return leerHistorialPorIndice(indice);
    }
    
    HistorialMedico vacio;
    memset(&vacio, 0, sizeof(HistorialMedico));
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

bool actualizarPaciente(int id, const Paciente& nuevosDatos) {
    int indice = buscarIndicePacientePorID(id);
    if (indice == -1) return false;
    
    return guardarPaciente(nuevosDatos, indice);
}

bool eliminarPaciente(int id) {
    int indice = buscarIndicePacientePorID(id);
    if (indice == -1) return false;
    
    Paciente paciente = leerPacientePorIndice(indice);
    paciente.eliminado = true;
    paciente.activo = false;
    
    // Actualizar header
    ArchivoHeader header = leerHeader(ARCHIVO_PACIENTES);
    header.registrosActivos--;
    actualizarHeader(ARCHIVO_PACIENTES, header);
    
    return guardarPaciente(paciente, indice);
}

bool guardarDoctor(const Doctor& doctor, int indice) {
    if (indice == -1) {
        indice = buscarIndiceDoctorPorID(doctor.id);
        if (indice == -1) return false;
    }
    
    fstream archivo(ARCHIVO_DOCTORES, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    long posicion = calcularPosicion(indice, sizeof(Doctor));
    archivo.seekp(posicion);
    archivo.write((char*)&doctor, sizeof(Doctor));
    archivo.close();
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

bool actualizarDoctor(int id, const Doctor& nuevosDatos) {
    int indice = buscarIndiceDoctorPorID(id);
    if (indice == -1) return false;
    
    return guardarDoctor(nuevosDatos, indice);
}

bool eliminarDoctor(int id) {
    int indice = buscarIndiceDoctorPorID(id);
    if (indice == -1) return false;
    
    Doctor doctor = leerDoctorPorIndice(indice);
    doctor.eliminado = true;
    doctor.disponible = false;
    
    // Actualizar header
    ArchivoHeader header = leerHeader(ARCHIVO_DOCTORES);
    header.registrosActivos--;
    actualizarHeader(ARCHIVO_DOCTORES, header);
    
    return guardarDoctor(doctor, indice);
}

bool guardarCita(const Cita& cita, int indice) {
    if (indice == -1) {
        indice = buscarIndiceCitaPorID(cita.id);
        if (indice == -1) return false;
    }
    
    fstream archivo(ARCHIVO_CITAS, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    long posicion = calcularPosicion(indice, sizeof(Cita));
    archivo.seekp(posicion);
    archivo.write((char*)&cita, sizeof(Cita));
    archivo.close();
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

bool actualizarCita(int id, const Cita& nuevosDatos) {
    int indice = buscarIndiceCitaPorID(id);
    if (indice == -1) return false;
    
    return guardarCita(nuevosDatos, indice);
}

bool cancelarCita(int id) {
    int indice = buscarIndiceCitaPorID(id);
    if (indice == -1) return false;
    
    Cita cita = leerCitaPorIndice(indice);
    strcpy(cita.estado, "Cancelada");
    cita.atendida = false;
    
    return guardarCita(cita, indice);
}

bool guardarHistorial(const HistorialMedico& historial, int indice) {
    if (indice == -1) {
        indice = buscarIndiceHistorialPorID(historial.idConsulta);
        if (indice == -1) return false;
    }
    
    fstream archivo(ARCHIVO_HISTORIAL, ios::binary | ios::in | ios::out);
    if (!archivo.is_open()) return false;
    
    long posicion = calcularPosicion(indice, sizeof(HistorialMedico));
    archivo.seekp(posicion);
    archivo.write((char*)&historial, sizeof(HistorialMedico));
    archivo.close();
    return true;
}

bool agregarHistorial(HistorialMedico& historial) {
    ArchivoHeader header = leerHeader(ARCHIVO_HISTORIAL);
    
    // Asignar ID
    historial.idConsulta = header.proximoID;
    
    ofstream archivo(ARCHIVO_HISTORIAL, ios::binary | ios::app);
    if (!archivo.is_open()) return false;
    
    archivo.write((char*)&historial, sizeof(HistorialMedico));
    archivo.close();
    
    // Actualizar header
    header.cantidadRegistros++;
    header.proximoID++;
    header.registrosActivos++;
    actualizarHeader(ARCHIVO_HISTORIAL, header);
    
    return true;
}

// ===================== FUNCIONES DE HISTORIAL MÉDICO =====================

bool agregarConsultaAlHistorial(int idPaciente, const HistorialMedico& consulta) {
    Paciente paciente = buscarPacientePorID(idPaciente);
    if (paciente.id == 0) return false;
    
    HistorialMedico nuevaConsulta = consulta;
    nuevaConsulta.idPaciente = idPaciente;
    nuevaConsulta.siguienteConsultaID = -1;
    nuevaConsulta.eliminado = false;
    
    if (paciente.primerConsultaID == -1) {
        // Primera consulta del paciente
        if (agregarHistorial(nuevaConsulta)) {
            paciente.primerConsultaID = nuevaConsulta.idConsulta;
            paciente.cantidadConsultas = 1;
            return guardarPaciente(paciente);
        }
    } else {
        // Buscar la última consulta
        HistorialMedico ultima = buscarHistorialPorID(paciente.primerConsultaID);
        HistorialMedico* actual = &ultima;
        
        while (actual->siguienteConsultaID != -1) {
            actual = &buscarHistorialPorID(actual->siguienteConsultaID);
        }
        
        // Agregar nueva consulta
        if (agregarHistorial(nuevaConsulta)) {
            // Actualizar enlace de la última consulta
            actual->siguienteConsultaID = nuevaConsulta.idConsulta;
            guardarHistorial(*actual);
            
            // Actualizar paciente
            paciente.cantidadConsultas++;
            return guardarPaciente(paciente);
        }
    }
    
    return false;
}

vector<HistorialMedico> obtenerHistorialCompleto(int idPaciente) {
    vector<HistorialMedico> historial;
    Paciente paciente = buscarPacientePorID(idPaciente);
    
    if (paciente.id == 0 || paciente.primerConsultaID == -1) {
        return historial;
    }
    
    int siguienteID = paciente.primerConsultaID;
    while (siguienteID != -1) {
        HistorialMedico consulta = buscarHistorialPorID(siguienteID);
        if (consulta.idConsulta != 0 && !consulta.eliminado) {
            historial.push_back(consulta);
            siguienteID = consulta.siguienteConsultaID;
        } else {
            break;
        }
    }
    
    return historial;
}

void mostrarHistorialMedico(int idPaciente) {
    vector<HistorialMedico> historial = obtenerHistorialCompleto(idPaciente);
    Paciente paciente = buscarPacientePorID(idPaciente);
    
    if (paciente.id == 0) {
        cout << "Paciente no encontrado" << endl;
        return;
    }
    
    cout << "=== HISTORIAL MÉDICO DE " << paciente.nombre << " " << paciente.apellido << " ===" << endl;
    
    if (historial.empty()) {
        cout << "No hay consultas registradas" << endl;
        return;
    }
    
    for (const auto& consulta : historial) {
        cout << "Consulta #" << consulta.idConsulta << " - " << consulta.fecha << " " << consulta.hora << endl;
        cout << "Diagnóstico: " << consulta.diagnostico << endl;
        cout << "Tratamiento: " << consulta.tratamiento << endl;
        cout << "Medicamentos: " << consulta.medicamentos << endl;
        cout << "Costo: $" << fixed << setprecision(2) << consulta.costo << endl;
        cout << "----------------------------------------" << endl;
    }
}

HistorialMedico* obtenerUltimaConsulta(int idPaciente) {
    vector<HistorialMedico> historial = obtenerHistorialCompleto(idPaciente);
    if (historial.empty()) {
        return nullptr;
    }
    
    HistorialMedico* ultima = new HistorialMedico;
    *ultima = historial.back();
    return ultima;
}

// ===================== FUNCIONES DE CITAS =====================

Cita* agendarCita(Hospital* hospital, int idPaciente, int idDoctor,
                  const char* fecha, const char* hora, const char* motivo) {
    // Verificar existencia
    Paciente paciente = buscarPacientePorID(idPaciente);
    Doctor doctor = buscarDoctorPorID(idDoctor);
    
    if (paciente.id == 0 || doctor.id == 0) {
        cout << "Paciente o doctor no encontrado" << endl;
        return nullptr;
    }
    
    // Verificar disponibilidad
    if (!verificarDisponibilidad(idDoctor, fecha, hora)) {
        cout << "El doctor no está disponible en esa fecha y hora" << endl;
        return nullptr;
    }
    
    // Verificar límites
    if (paciente.cantidadCitas >= MAX_CITAS_PACIENTE) {
        cout << "El paciente ha alcanzado el límite de citas" << endl;
        return nullptr;
    }
    
    if (doctor.cantidadCitas >= MAX_CITAS_DOCTOR) {
        cout << "El doctor ha alcanzado el límite de citas" << endl;
        return nullptr;
    }
    
    // Crear cita
    Cita nuevaCita;
    memset(&nuevaCita, 0, sizeof(Cita));
    nuevaCita.idPaciente = idPaciente;
    nuevaCita.idDoctor = idDoctor;
    strncpy(nuevaCita.fecha, fecha, sizeof(nuevaCita.fecha)-1);
    strncpy(nuevaCita.hora, hora, sizeof(nuevaCita.hora)-1);
    strncpy(nuevaCita.motivo, motivo, sizeof(nuevaCita.motivo)-1);
    strncpy(nuevaCita.estado, "Programada", sizeof(nuevaCita.estado)-1);
    nuevaCita.atendida = false;
    nuevaCita.eliminado = false;
    nuevaCita.consultaID = -1;
    
    if (agregarCita(nuevaCita)) {
        // Actualizar contadores
        hospital->totalCitasAgendadas++;
        
        // Agregar cita al paciente
        paciente.citasIDs[paciente.cantidadCitas] = nuevaCita.id;
        paciente.cantidadCitas++;
        guardarPaciente(paciente);
        
        // Agregar cita al doctor
        doctor.citasIDs[doctor.cantidadCitas] = nuevaCita.id;
        doctor.cantidadCitas++;
        guardarDoctor(doctor);
        
        cout << "Cita agendada correctamente con ID: " << nuevaCita.id << endl;
        
        Cita* copia = new Cita;
        *copia = nuevaCita;
        return copia;
    }
    
    return nullptr;
}

bool atenderCita(Hospital* hospital, int idCita, const char* diagnostico,
                 const char* tratamiento, const char* medicamentos) {
    Cita cita = buscarCitaPorID(idCita);
    if (cita.id == 0 || cita.atendida) {
        cout << "Cita no encontrada o ya atendida" << endl;
        return false;
    }
    
    // Crear registro en historial
    HistorialMedico consulta;
    memset(&consulta, 0, sizeof(HistorialMedico));
    consulta.idPaciente = cita.idPaciente;
    consulta.idDoctor = cita.idDoctor;
    strncpy(consulta.fecha, cita.fecha, sizeof(consulta.fecha)-1);
    strncpy(consulta.hora, cita.hora, sizeof(consulta.hora)-1);
    strncpy(consulta.diagnostico, diagnostico, sizeof(consulta.diagnostico)-1);
    strncpy(consulta.tratamiento, tratamiento, sizeof(consulta.tratamiento)-1);
    strncpy(consulta.medicamentos, medicamentos, sizeof(consulta.medicamentos)-1);
    
    Doctor doctor = buscarDoctorPorID(cita.idDoctor);
    consulta.costo = doctor.costoConsulta;
    
    if (agregarConsultaAlHistorial(cita.idPaciente, consulta)) {
        // Actualizar cita
        cita.atendida = true;
        strcpy(cita.estado, "Atendida");
        cita.consultaID = consulta.idConsulta;
        strncpy(cita.observaciones, diagnostico, sizeof(cita.observaciones)-1);
        
        // Actualizar contadores
        hospital->totalConsultasRealizadas++;
        
        cout << "Cita atendida y registrada en historial" << endl;
        return guardarCita(cita);
    }
    
    return false;
}

vector<Cita> obtenerCitasDePaciente(int idPaciente) {
    vector<Cita> citas;
    Paciente paciente = buscarPacientePorID(idPaciente);
    
    if (paciente.id == 0) return citas;
    
    for (int i = 0; i < paciente.cantidadCitas; i++) {
        Cita cita = buscarCitaPorID(paciente.citasIDs[i]);
        if (cita.id != 0 && !cita.eliminado) {
            citas.push_back(cita);
        }
    }
    
    return citas;
}

vector<Cita> obtenerCitasDeDoctor(int idDoctor) {
    vector<Cita> citas;
    Doctor doctor = buscarDoctorPorID(idDoctor);
    
    if (doctor.id == 0) return citas;
    
    for (int i = 0; i < doctor.cantidadCitas; i++) {
        Cita cita = buscarCitaPorID(doctor.citasIDs[i]);
        if (cita.id != 0 && !cita.eliminado) {
            citas.push_back(cita);
        }
    }
    
    return citas;
}

vector<Cita> obtenerCitasPorFecha(const char* fecha) {
    vector<Cita> citas;
    ArchivoHeader header = leerHeader(ARCHIVO_CITAS);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.eliminado && strcmp(cita.fecha, fecha) == 0) {
            citas.push_back(cita);
        }
    }
    
    return citas;
}

bool verificarDisponibilidad(int idDoctor, const char* fecha, const char* hora) {
    vector<Cita> citasDoctor = obtenerCitasDeDoctor(idDoctor);
    
    for (const auto& cita : citasDoctor) {
        if (strcmp(cita.fecha, fecha) == 0 && strcmp(cita.hora, hora) == 0 &&
            strcmp(cita.estado, "Cancelada") != 0) {
            return false;
        }
    }
    
    return true;
}

void listarCitasPendientes() {
    ArchivoHeader header = leerHeader(ARCHIVO_CITAS);
    
    cout << "=== CITAS PENDIENTES ===" << endl;
    bool hayCitas = false;
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.eliminado && !cita.atendida && strcmp(cita.estado, "Cancelada") != 0) {
            Paciente paciente = buscarPacientePorID(cita.idPaciente);
            Doctor doctor = buscarDoctorPorID(cita.idDoctor);
            
            cout << "Cita #" << cita.id << " - " << cita.fecha << " " << cita.hora << endl;
            cout << "Paciente: " << paciente.nombre << " " << paciente.apellido << endl;
            cout << "Doctor: " << doctor.nombre << " " << doctor.apellido << endl;
            cout << "Motivo: " << cita.motivo << endl;
            cout << "----------------------------------------" << endl;
            hayCitas = true;
        }
    }
    
    if (!hayCitas) {
        cout << "No hay citas pendientes" << endl;
    }
}

// ===================== FUNCIONES DE BÚSQUEDA AVANZADA =====================

vector<Paciente> buscarPacientesPorNombre(const char* nombre) {
    vector<Paciente> resultados;
    ArchivoHeader header = leerHeader(ARCHIVO_PACIENTES);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Paciente p = leerPacientePorIndice(i);
        if (!p.eliminado) {
            if (strstr(p.nombre, nombre) != nullptr || strstr(p.apellido, nombre) != nullptr) {
                resultados.push_back(p);
            }
        }
    }
    
    return resultados;
}

vector<Doctor> buscarDoctoresPorEspecialidad(const char* especialidad) {
    vector<Doctor> resultados;
    ArchivoHeader header = leerHeader(ARCHIVO_DOCTORES);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (!d.eliminado && strstr(d.especialidad, especialidad) != nullptr) {
            resultados.push_back(d);
        }
    }
    
    return resultados;
}

vector<Doctor> buscarDoctoresPorNombre(const char* nombre) {
    vector<Doctor> resultados;
    ArchivoHeader header = leerHeader(ARCHIVO_DOCTORES);
    
    for (int i = 0; i < header.cantidadRegistros; i++) {
        Doctor d = leerDoctorPorIndice(i);
        if (!d.eliminado) {
            if (strstr(d.nombre, nombre) != nullptr || strstr(d.apellido, nombre) != nullptr) {
                resultados.push_back(d);
            }
        }
    }
    
    return resultados;
}

// ===================== FUNCIONES DE RELACIONES =====================

bool asignarPacienteADoctor(int idDoctor, int idPaciente) {
    Doctor doctor = buscarDoctorPorID(idDoctor);
    Paciente paciente = buscarPacientePorID(idPaciente);
    
    if (doctor.id == 0 || paciente.id == 0) return false;
    
    // Verificar si ya está asignado
    for (int i = 0; i < doctor.cantidadPacientes; i++) {
        if (doctor.pacientesIDs[i] == idPaciente) {
            return false; // Ya está asignado
        }
    }
    
    // Verificar límite
    if (doctor.cantidadPacientes >= MAX_PACIENTES_DOCTOR) {
        cout << "El doctor ha alcanzado el límite de pacientes" << endl;
        return false;
    }
    
    // Asignar
    doctor.pacientesIDs[doctor.cantidadPacientes] = idPaciente;
    doctor.cantidadPacientes++;
    
    return guardarDoctor(doctor);
}

bool removerPacienteDeDoctor(int idDoctor, int idPaciente) {
    Doctor doctor = buscarDoctorPorID(idDoctor);
    if (doctor.id == 0) return false;
    
    int indice = -1;
    for (int i = 0; i < doctor.cantidadPacientes; i++) {
        if (doctor.pacientesIDs[i] == idPaciente) {
            indice = i;
            break;
        }
    }
    
    if (indice == -1) return false;
    
    // Remover
    for (int i = indice; i < doctor.cantidadPacientes - 1; i++) {
        doctor.pacientesIDs[i] = doctor.pacientesIDs[i + 1];
    }
    doctor.cantidadPacientes--;
    
    return guardarDoctor(doctor);
}

void listarPacientesDeDoctor(int idDoctor) {
    Doctor doctor = buscarDoctorPorID(idDoctor);
    if (doctor.id == 0) {
        cout << "Doctor no encontrado" << endl;
        return;
    }
    
    cout << "=== PACIENTES DEL DR. " << doctor.nombre << " " << doctor.apellido << " ===" << endl;
    
    if (doctor.cantidadPacientes == 0) {
        cout << "No tiene pacientes asignados" << endl;
        return;
    }
    
    for (int i = 0; i < doctor.cantidadPacientes; i++) {
        Paciente paciente = buscarPacientePorID(doctor.pacientesIDs[i]);
        if (paciente.id != 0) {
            cout << "ID: " << paciente.id << " - " << paciente.nombre << " " << paciente.apellido;
            cout << " - " << paciente.cedula << " - Edad: " << paciente.edad << endl;
        }
    }
}

// ===================== FUNCIONES DE VALIDACIÓN =====================

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
    
    int año = atoi(fecha);
    int mes = atoi(fecha + 5);
    int dia = atoi(fecha + 8);
    
    if (mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > 31) return false;
    
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
    
    int hh = atoi(hora);
    int mm = atoi(hora + 3);
    
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    
    return true;
}

int compararFechas(const char* fecha1, const char* fecha2) {
    return strcmp(fecha1, fecha2);
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
        
        hospital->totalPacientesRegistrados = headerPac.registrosActivos;
        hospital->totalDoctoresRegistrados = headerDoc.registrosActivos;
        hospital->totalCitasAgendadas = headerCit.registrosActivos;
        hospital->totalConsultasRealizadas = headerHist.registrosActivos;
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

// ===================== FUNCIONES DE MANTENIMIENTO =====================

bool compactarArchivoPacientes() {
    // Implementación básica - en producción sería más compleja
    cout << "Compactación de archivo de pacientes..." << endl;
    return true;
}

bool compactarArchivoDoctores() {
    cout << "Compactación de archivo de doctores..." << endl;
    return true;
}

bool compactarArchivoCitas() {
    cout << "Compactación de archivo de citas..." << endl;
    return true;
}

bool verificarIntegridadReferencial() {
    cout << "Verificando integridad referencial..." << endl;
    
    ArchivoHeader headerCitas = leerHeader(ARCHIVO_CITAS);
    bool integridad = true;
    
    for (int i = 0; i < headerCitas.cantidadRegistros; i++) {
        Cita cita = leerCitaPorIndice(i);
        if (!cita.eliminado) {
            Paciente paciente = buscarPacientePorID(cita.idPaciente);
            Doctor doctor = buscarDoctorPorID(cita.idDoctor);
            
            if (paciente.id == 0 || paciente.eliminado) {
                cout << "Cita #" << cita.id << " referencia a paciente inválido: " << cita.idPaciente << endl;
                integridad = false;
            }
            
            if (doctor.id == 0 || doctor.eliminado) {
                cout << "Cita #" << cita.id << " referencia a doctor inválido: " << cita.idDoctor << endl;
                integridad = false;
            }
        }
    }
    
    if (integridad) {
        cout << "Integridad referencial verificada correctamente" << endl;
    }
    
    return integridad;
}

bool hacerRespaldo() {
    cout << "Creando respaldo del sistema..." << endl;
    // En implementación real, copiaría todos los archivos .bin a una carpeta de respaldo
    return true;
}

bool restaurarRespaldo() {
    cout << "Restaurando desde respaldo..." << endl;
    // En implementación real, restauraría los archivos desde la carpeta de respaldo
    return true;
}

// ===================== FUNCIONES DE UTILIDAD =====================

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
    cout << "ID  NOMBRE COMPLETO              CEDULA     EDAD CONS. CITAS" << endl;
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
                 << " " << setw(5) << p.cantidadConsultas
                 << " " << setw(5) << p.cantidadCitas << endl;
        }
    }
    cout << "=================================================================" << endl;
}

void listarDoctores(Hospital* hospital) {
    ArchivoHeader header = leerHeader(ARCHIVO_DOCTORES);
    
    cout << "=================================================================" << endl;
    cout << "                     LISTA DE DOCTORES                          " << endl;
    cout << "=================================================================" << endl;
    cout << "ID  NOMBRE COMPLETO              ESPECIALIDAD           EXP. COSTO PAC. CITAS" << endl;
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
                 << " " << setw(6) << fixed << setprecision(2) << d.costoConsulta
                 << " " << setw(4) << d.cantidadPacientes
                 << " " << setw(5) << d.cantidadCitas << endl;
        }
    }
    cout << "=================================================================" << endl;
}

// ===================== MENÚS COMPLETOS CON SYSTEM =====================

void menuPacientes(Hospital* h) {
    int opcion;
    do {
        system("cls");
        cout << "==========================================" << endl;
        cout << "         GESTION DE PACIENTES" << endl;
        cout << "==========================================" << endl;
        cout << "1. Crear paciente" << endl;
        cout << "2. Listar pacientes" << endl;
        cout << "3. Buscar paciente por cedula" << endl;
        cout << "4. Actualizar paciente" << endl;
        cout << "5. Eliminar paciente" << endl;
        cout << "6. Ver historial medico" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "==========================================" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1: {
                system("cls");
                cout << "=== CREAR NUEVO PACIENTE ===" << endl;
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
                system("pause");
                break;
            }
            case 2:
                system("cls");
                listarPacientes(h);
                system("pause");
                break;
            case 3: {
                system("cls");
                cout << "=== BUSCAR PACIENTE POR CEDULA ===" << endl;
                char cedula[20];
                cout << "Cedula a buscar: ";
                leerLinea(cedula, 20);
                Paciente p = buscarPacientePorCedula(cedula);
                if (p.id != 0) {
                    cout << "Paciente encontrado: " << p.nombre << " " << p.apellido << " (ID: " << p.id << ")" << endl;
                    cout << "Edad: " << p.edad << " - Telefono: " << p.telefono << " - Email: " << p.email << endl;
                } else {
                    cout << "Paciente no encontrado" << endl;
                }
                system("pause");
                break;
            }
            case 4: {
                system("cls");
                cout << "=== ACTUALIZAR PACIENTE ===" << endl;
                int id;
                cout << "ID del paciente a actualizar: ";
                id = leerEntero();
                // Implementar actualización completa
                cout << "Funcionalidad en desarrollo..." << endl;
                system("pause");
                break;
            }
            case 5: {
                system("cls");
                cout << "=== ELIMINAR PACIENTE ===" << endl;
                int id;
                cout << "ID del paciente a eliminar: ";
                id = leerEntero();
                if (eliminarPaciente(id)) {
                    cout << "Paciente eliminado correctamente" << endl;
                } else {
                    cout << "Error al eliminar paciente" << endl;
                }
                system("pause");
                break;
            }
            case 6: {
                system("cls");
                cout << "=== HISTORIAL MEDICO ===" << endl;
                int idPaciente;
                cout << "ID del paciente para ver historial: ";
                idPaciente = leerEntero();
                mostrarHistorialMedico(idPaciente);
                system("pause");
                break;
            }
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
                system("pause");
        }
    } while (opcion != 0);
}

void menuDoctores(Hospital* h) {
    int opcion;
    do {
        system("cls");
        cout << "==========================================" << endl;
        cout << "          GESTION DE DOCTORES" << endl;
        cout << "==========================================" << endl;
        cout << "1. Crear doctor" << endl;
        cout << "2. Listar doctores" << endl;
        cout << "3. Buscar doctor por especialidad" << endl;
        cout << "4. Asignar paciente a doctor" << endl;
        cout << "5. Listar pacientes de doctor" << endl;
        cout << "6. Eliminar doctor" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "==========================================" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1: {
                system("cls");
                cout << "=== CREAR NUEVO DOCTOR ===" << endl;
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
                if (d) delete d;
                system("pause");
                break;
            }
            case 2:
                system("cls");
                listarDoctores(h);
                system("pause");
                break;
            case 3: {
                system("cls");
                cout << "=== BUSCAR DOCTOR POR ESPECIALIDAD ===" << endl;
                char especialidad[50];
                cout << "Especialidad a buscar: ";
                leerLinea(especialidad, 50);
                vector<Doctor> resultados = buscarDoctoresPorEspecialidad(especialidad);
                if (resultados.empty()) {
                    cout << "No se encontraron doctores con esa especialidad" << endl;
                } else {
                    cout << "=== DOCTORES ENCONTRADOS ===" << endl;
                    for (const auto& d : resultados) {
                        cout << "ID: " << d.id << " - Dr. " << d.nombre << " " << d.apellido;
                        cout << " - " << d.especialidad << " - Exp: " << d.aniosExperiencia << " años" << endl;
                        cout << "Costo consulta: $" << fixed << setprecision(2) << d.costoConsulta << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
                system("pause");
                break;
            }
            case 4: {
                system("cls");
                cout << "=== ASIGNAR PACIENTE A DOCTOR ===" << endl;
                int idDoctor, idPaciente;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                if (asignarPacienteADoctor(idDoctor, idPaciente)) {
                    cout << "Paciente asignado correctamente al doctor" << endl;
                } else {
                    cout << "Error al asignar paciente al doctor" << endl;
                }
                system("pause");
                break;
            }
            case 5: {
                system("cls");
                cout << "=== PACIENTES DE DOCTOR ===" << endl;
                int idDoctor;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                listarPacientesDeDoctor(idDoctor);
                system("pause");
                break;
            }
            case 6: {
                system("cls");
                cout << "=== ELIMINAR DOCTOR ===" << endl;
                int id;
                cout << "ID del doctor a eliminar: ";
                id = leerEntero();
                if (eliminarDoctor(id)) {
                    cout << "Doctor eliminado correctamente" << endl;
                } else {
                    cout << "Error al eliminar doctor" << endl;
                }
                system("pause");
                break;
            }
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
                system("pause");
        }
    } while (opcion != 0);
}

void menuCitas(Hospital* h) {
    int opcion;
    do {
        system("cls");
        cout << "==========================================" << endl;
        cout << "           GESTION DE CITAS" << endl;
        cout << "==========================================" << endl;
        cout << "1. Agendar cita" << endl;
        cout << "2. Listar citas pendientes" << endl;
        cout << "3. Cancelar cita" << endl;
        cout << "4. Atender cita" << endl;
        cout << "5. Ver citas de paciente" << endl;
        cout << "6. Ver citas de doctor" << endl;
        cout << "7. Buscar citas por fecha" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "==========================================" << endl;
        cout << "Seleccione una opcion: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1: {
                system("cls");
                cout << "=== AGENDAR CITA ===" << endl;
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
                
                Cita* cita = agendarCita(h, idPaciente, idDoctor, fecha, hora, motivo);
                if (cita) delete cita;
                system("pause");
                break;
            }
            case 2:
                system("cls");
                listarCitasPendientes();
                system("pause");
                break;
            case 3: {
                system("cls");
                cout << "=== CANCELAR CITA ===" << endl;
                int idCita;
                cout << "ID de la cita a cancelar: ";
                idCita = leerEntero();
                if (cancelarCita(idCita)) {
                    cout << "Cita cancelada correctamente" << endl;
                } else {
                    cout << "Error al cancelar la cita" << endl;
                }
                system("pause");
                break;
            }
            case 4: {
                system("cls");
                cout << "=== ATENDER CITA ===" << endl;
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
                if (atenderCita(h, idCita, diagnostico, tratamiento, medicamentos)) {
                    cout << "Cita atendida correctamente" << endl;
                } else {
                    cout << "Error al atender la cita" << endl;
                }
                system("pause");
                break;
            }
            case 5: {
                system("cls");
                cout << "=== CITAS DE PACIENTE ===" << endl;
                int idPaciente;
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                vector<Cita> citas = obtenerCitasDePaciente(idPaciente);
                if (citas.empty()) {
                    cout << "No se encontraron citas para este paciente" << endl;
                } else {
                    cout << "=== CITAS DEL PACIENTE ===" << endl;
                    for (const auto& cita : citas) {
                        Doctor doctor = buscarDoctorPorID(cita.idDoctor);
                        cout << "Cita #" << cita.id << " - " << cita.fecha << " " << cita.hora;
                        cout << " - Dr. " << doctor.nombre << " " << doctor.apellido;
                        cout << " - " << cita.estado << " - " << cita.motivo << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
                system("pause");
                break;
            }
            case 6: {
                system("cls");
                cout << "=== CITAS DE DOCTOR ===" << endl;
                int idDoctor;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                vector<Cita> citas = obtenerCitasDeDoctor(idDoctor);
                if (citas.empty()) {
                    cout << "No se encontraron citas para este doctor" << endl;
                } else {
                    cout << "=== CITAS DEL DOCTOR ===" << endl;
                    for (const auto& cita : citas) {
                        Paciente paciente = buscarPacientePorID(cita.idPaciente);
                        cout << "Cita #" << cita.id << " - Paciente: " << paciente.nombre << " " << paciente.apellido;
                        cout << " - " << cita.fecha << " " << cita.hora << " - " << cita.estado << endl;
                        cout << "Motivo: " << cita.motivo << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
                system("pause");
                break;
            }
            case 7: {
                system("cls");
                cout << "=== CITAS POR FECHA ===" << endl;
                char fecha[11];
                cout << "Fecha a buscar (YYYY-MM-DD): ";
                leerLinea(fecha, 11);
                vector<Cita> citas = obtenerCitasPorFecha(fecha);
                if (citas.empty()) {
                    cout << "No hay citas para esta fecha" << endl;
                } else {
                    cout << "=== CITAS PARA " << fecha << " ===" << endl;
                    for (const auto& cita : citas) {
                        Paciente paciente = buscarPacientePorID(cita.idPaciente);
                        Doctor doctor = buscarDoctorPorID(cita.idDoctor);
                        cout << "Cita #" << cita.id << " - " << cita.hora << " - " << paciente.nombre << " " << paciente.apellido;
                        cout << " con Dr. " << doctor.nombre << " - " << cita.estado << endl;
                        cout << "Motivo: " << cita.motivo << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
                system("pause");
                break;
            }
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
                system("pause");
        }
    } while (opcion != 0);
}

void menuMantenimiento(Hospital* h) {
    int opcion;
    do {
        system("cls");
        cout << "==========================================" << endl;
        cout << "         MANTENIMIENTO DEL SISTEMA" << endl;
        cout << "==========================================" << endl;
        cout << "1. Verificar integridad de archivos" << endl;
        cout << "2. Compactar archivos" << endl;
        cout << "3. Hacer respaldo" << endl;
        cout << "4. Restaurar desde respaldo" << endl;
        cout << "5. Estadísticas de uso" << endl;
        cout << "6. Verificar integridad referencial" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "==========================================" << endl;
        cout << "Seleccione una opción: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1:
                system("cls");
                cout << "=== VERIFICAR ARCHIVOS ===" << endl;
                if (verificarArchivo(ARCHIVO_PACIENTES) && verificarArchivo(ARCHIVO_DOCTORES) &&
                    verificarArchivo(ARCHIVO_CITAS) && verificarArchivo(ARCHIVO_HISTORIAL)) {
                    cout << "✓ Todos los archivos están en buen estado" << endl;
                } else {
                    cout << "✗ Algunos archivos tienen problemas" << endl;
                }
                system("pause");
                break;
            case 2:
                system("cls");
                cout << "=== COMPACTAR ARCHIVOS ===" << endl;
                compactarArchivoPacientes();
                compactarArchivoDoctores();
                compactarArchivoCitas();
                cout << "✓ Compactación completada" << endl;
                system("pause");
                break;
            case 3:
                system("cls");
                cout << "=== CREAR RESPALDO ===" << endl;
                if (hacerRespaldo()) {
                    cout << "✓ Respaldo creado correctamente" << endl;
                } else {
                    cout << "✗ Error al crear respaldo" << endl;
                }
                system("pause");
                break;
            case 4:
                system("cls");
                cout << "=== RESTAURAR RESPALDO ===" << endl;
                if (restaurarRespaldo()) {
                    cout << "✓ Respaldo restaurado correctamente" << endl;
                } else {
                    cout << "✗ Error al restaurar respaldo" << endl;
                }
                system("pause");
                break;
            case 5: {
                system("cls");
                cout << "=== ESTADÍSTICAS DE USO ===" << endl;
                ArchivoHeader pac = leerHeader(ARCHIVO_PACIENTES);
                ArchivoHeader doc = leerHeader(ARCHIVO_DOCTORES);
                ArchivoHeader cit = leerHeader(ARCHIVO_CITAS);
                ArchivoHeader hist = leerHeader(ARCHIVO_HISTORIAL);
                
                cout << "PACIENTES: " << pac.registrosActivos << " activos de " << pac.cantidadRegistros << " totales" << endl;
                cout << "DOCTORES:  " << doc.registrosActivos << " activos de " << doc.cantidadRegistros << " totales" << endl;
                cout << "CITAS:     " << cit.registrosActivos << " activas de " << cit.cantidadRegistros << " totales" << endl;
                cout << "CONSULTAS: " << hist.registrosActivos << " activas de " << hist.cantidadRegistros << " totales" << endl;
                cout << "----------------------------------------" << endl;
                cout << "Espacio utilizado: " << (pac.cantidadRegistros * sizeof(Paciente) +
                                                 doc.cantidadRegistros * sizeof(Doctor) +
                                                 cit.cantidadRegistros * sizeof(Cita) +
                                                 hist.cantidadRegistros * sizeof(HistorialMedico)) / 1024 << " KB" << endl;
                system("pause");
                break;
            }
            case 6:
                system("cls");
                cout << "=== INTEGRIDAD REFERENCIAL ===" << endl;
                verificarIntegridadReferencial();
                system("pause");
                break;
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
                system("pause");
        }
    } while (opcion != 0);
}

void menuReportes(Hospital* h) {
    int opcion;
    do {
        system("cls");
        cout << "==========================================" << endl;
        cout << "         REPORTES Y CONSULTAS" << endl;
        cout << "==========================================" << endl;
        cout << "1. Buscar pacientes por nombre" << endl;
        cout << "2. Buscar doctores por especialidad" << endl;
        cout << "3. Buscar doctores por nombre" << endl;
        cout << "4. Listar pacientes de doctor" << endl;
        cout << "5. Ver historial médico de paciente" << endl;
        cout << "0. Volver al menu principal" << endl;
        cout << "==========================================" << endl;
        cout << "Seleccione una opción: ";
        opcion = leerEntero();

        switch (opcion) {
            case 1: {
                system("cls");
                cout << "=== BUSCAR PACIENTES POR NOMBRE ===" << endl;
                char nombre[50];
                cout << "Nombre a buscar: ";
                leerLinea(nombre, 50);
                vector<Paciente> resultados = buscarPacientesPorNombre(nombre);
                if (resultados.empty()) {
                    cout << "No se encontraron pacientes" << endl;
                } else {
                    cout << "=== PACIENTES ENCONTRADOS ===" << endl;
                    for (const auto& p : resultados) {
                        cout << "ID: " << p.id << " - " << p.nombre << " " << p.apellido;
                        cout << " - " << p.cedula << " - Edad: " << p.edad;
                        cout << " - Tel: " << p.telefono << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
                system("pause");
                break;
            }
            case 2: {
                system("cls");
                cout << "=== BUSCAR DOCTORES POR ESPECIALIDAD ===" << endl;
                char especialidad[50];
                cout << "Especialidad a buscar: ";
                leerLinea(especialidad, 50);
                vector<Doctor> resultados = buscarDoctoresPorEspecialidad(especialidad);
                if (resultados.empty()) {
                    cout << "No se encontraron doctores con esa especialidad" << endl;
                } else {
                    cout << "=== DOCTORES ENCONTRADOS ===" << endl;
                    for (const auto& d : resultados) {
                        cout << "ID: " << d.id << " - Dr. " << d.nombre << " " << d.apellido;
                        cout << " - " << d.especialidad << " - Exp: " << d.aniosExperiencia << " años" << endl;
                        cout << "Costo: $" << fixed << setprecision(2) << d.costoConsulta;
                        cout << " - Pacientes: " << d.cantidadPacientes << " - Citas: " << d.cantidadCitas << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
                system("pause");
                break;
            }
            case 3: {
                system("cls");
                cout << "=== BUSCAR DOCTORES POR NOMBRE ===" << endl;
                char nombre[50];
                cout << "Nombre a buscar: ";
                leerLinea(nombre, 50);
                vector<Doctor> resultados = buscarDoctoresPorNombre(nombre);
                if (resultados.empty()) {
                    cout << "No se encontraron doctores" << endl;
                } else {
                    cout << "=== DOCTORES ENCONTRADOS ===" << endl;
                    for (const auto& d : resultados) {
                        cout << "ID: " << d.id << " - Dr. " << d.nombre << " " << d.apellido;
                        cout << " - " << d.especialidad << " - Exp: " << d.aniosExperiencia << " años" << endl;
                        cout << "Costo: $" << fixed << setprecision(2) << d.costoConsulta;
                        cout << " - Tel: " << d.telefono << " - Email: " << d.email << endl;
                        cout << "----------------------------------------" << endl;
                    }
                }
                system("pause");
                break;
            }
            case 4: {
                system("cls");
                cout << "=== PACIENTES DE DOCTOR ===" << endl;
                int idDoctor;
                cout << "ID del doctor: ";
                idDoctor = leerEntero();
                listarPacientesDeDoctor(idDoctor);
                system("pause");
                break;
            }
            case 5: {
                system("cls");
                cout << "=== HISTORIAL MEDICO ===" << endl;
                int idPaciente;
                cout << "ID del paciente: ";
                idPaciente = leerEntero();
                mostrarHistorialMedico(idPaciente);
                system("pause");
                break;
            }
            case 0:
                cout << "Volviendo al menu principal..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
                system("pause");
        }
    } while (opcion != 0);
}

void mostrarDatosHospital(Hospital* h) {
    system("cls");
    ArchivoHeader headerPac = leerHeader(ARCHIVO_PACIENTES);
    ArchivoHeader headerDoc = leerHeader(ARCHIVO_DOCTORES);
    ArchivoHeader headerCit = leerHeader(ARCHIVO_CITAS);
    ArchivoHeader headerHist = leerHeader(ARCHIVO_HISTORIAL);
    
    cout << "==========================================" << endl;
    cout << "      INFORMACION DEL HOSPITAL" << endl;
    cout << "==========================================" << endl;
    cout << "Nombre: " << h->nombre << endl;
    cout << "Direccion: " << h->direccion << endl;
    cout << "Telefono: " << h->telefono << endl;
    cout << "------------------------------------------" << endl;
    cout << "Pacientes registrados: " << headerPac.registrosActivos << endl;
    cout << "Doctores registrados:  " << headerDoc.registrosActivos << endl;
    cout << "Citas registradas:     " << headerCit.registrosActivos << endl;
    cout << "Consultas realizadas:  " << headerHist.registrosActivos << endl;
    cout << "==========================================" << endl;
    system("pause");
}

// ===================== FUNCION PRINCIPAL MODIFICADA =====================

int main() {
    system("cls");
    cout << "==========================================" << endl;
    cout << "  SISTEMA DE GESTION HOSPITALARIA v2.0" << endl;
    cout << "     (Sistema con Persistencia)" << endl;
    cout << "==========================================" << endl;
    cout << "Cargando sistema desde archivos..." << endl;

    Hospital* hospital = cargarSistema();

    int opcion;
    do {
        system("cls");
        cout << "==========================================" << endl;
        cout << "           MENU PRINCIPAL" << endl;
        cout << "==========================================" << endl;
        cout << "1. Gestion de Pacientes" << endl;
        cout << "2. Gestion de Doctores" << endl;
        cout << "3. Gestion de Citas" << endl;
        cout << "4. Reportes y Consultas" << endl;
        cout << "5. Mantenimiento del Sistema" << endl;
        cout << "6. Informacion del Hospital" << endl;
        cout << "0. Salir del Sistema" << endl;
        cout << "==========================================" << endl;
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
                menuReportes(hospital);
                break;
            case 5:
                menuMantenimiento(hospital);
                break;
            case 6:
                mostrarDatosHospital(hospital);
                break;
            case 0:
                cout << "Guardando y saliendo del sistema..." << endl;
                break;
            default:
                cout << "Opcion invalida" << endl;
                system("pause");
        }
    } while (opcion != 0);

    limpiarSistema(hospital);
    system("cls");
    cout << "==========================================" << endl;
    cout << "  Sistema cerrado correctamente" << endl;
    cout << "   Gracias por usar el sistema" << endl;
    cout << "==========================================" << endl;
    system("pause");
    return 0;
}