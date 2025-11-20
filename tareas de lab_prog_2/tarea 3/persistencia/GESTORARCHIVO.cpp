#include "GestorArchivos.hpp"
ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
PacienteRecord copy = p;
copy.id = h.proximoID;
ofstream f(ARCHIVO_PACIENTES, ios::binary | ios::app);
if (!f.is_open()) return false;
f.write((char*)&copy, sizeof(PacienteRecord));
f.close();
h.cantidadRegistros++; h.registrosActivos++; h.proximoID++;
actualizarHeader(ARCHIVO_PACIENTES, h);
return true;
}


bool guardarPaciente(const PacienteRecord& p, int indice) {
if (indice < 0) return false;
fstream f(ARCHIVO_PACIENTES, ios::binary | ios::in | ios::out);
if (!f.is_open()) return false;
f.seekp(calcularPosicion(indice, sizeof(PacienteRecord)));
f.write((char*)&p, sizeof(PacienteRecord));
f.close();
return true;
}


PacienteRecord leerPacientePorIndice(int indice) {
PacienteRecord p; memset(&p,0,sizeof(p));
ifstream f(ARCHIVO_PACIENTES, ios::binary);
if (!f.is_open()) return p;
long pos = calcularPosicion(indice, sizeof(PacienteRecord));
f.seekg(pos);
f.read((char*)&p, sizeof(PacienteRecord));
f.close();
return p;
}


int buscarIndicePacientePorID(int id) {
ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
for (int i=0;i<h.cantidadRegistros;i++) {
PacienteRecord p = leerPacientePorIndice(i);
if (p.id == id && !p.eliminado) return i;
}
return -1;
}


PacienteRecord buscarPacientePorCedula(const char* cedula) {
ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
PacienteRecord vacio; memset(&vacio,0,sizeof(vacio));
for (int i=0;i<h.cantidadRegistros;i++) {
PacienteRecord p = leerPacientePorIndice(i);
if (!p.eliminado && strcmp(p.cedula, cedula) == 0) return p;
}
return vacio;
}
return d;
}


int buscarIndiceDoctorPorID(int id) {
ArchivoHeader h = leerHeader(ARCHIVO_DOCTORES);
for (int i=0;i<h.cantidadRegistros;i++) {
DoctorRecord d = leerDoctorPorIndice(i);
if (d.id == id && !d.eliminado) return i;
}
return -1;
}


// ---------- CITAS ----------


bool agregarCita(const CitaRecord& c) {
ArchivoHeader h = leerHeader(ARCHIVO_CITAS);
CitaRecord copy = c; copy.id = h.proximoID;
ofstream f(ARCHIVO_CITAS, ios::binary | ios::app);
if (!f.is_open()) return false;
f.write((char*)&copy, sizeof(CitaRecord)); f.close();
h.cantidadRegistros++; h.registrosActivos++; h.proximoID++; actualizarHeader(ARCHIVO_CITAS,h);
return true;
}


bool guardarCita(const CitaRecord& c, int indice) {
if (indice < 0) return false;
fstream f(ARCHIVO_CITAS, ios::binary | ios::in | ios::out);
if (!f.is_open()) return false;
f.seekp(calcularPosicion(indice, sizeof(CitaRecord)));
f.write((char*)&c, sizeof(CitaRecord)); f.close();
return true;
}


CitaRecord leerCitaPorIndice(int indice) {
CitaRecord c; memset(&c,0,sizeof(c));
ifstream f(ARCHIVO_CITAS, ios::binary);
if (!f.is_open()) return c;
long pos = calcularPosicion(indice, sizeof(CitaRecord)); f.seekg(pos); f.read((char*)&c, sizeof(CitaRecord)); f.close();
return c;
}


int buscarIndiceCitaPorID(int id) {
ArchivoHeader h = leerHeader(ARCHIVO_CITAS);
for (int i=0;i<h.cantidadRegistros;i++) {
CitaRecord c = leerCitaPorIndice(i);
if (c.id == id && !c.eliminado) return i;
}
return -1;
}
// ---------- HISTORIAL ----------
ArchivoHeader h = leerHeader(ARCHIVO_HISTORIAL);
HistorialRecord copy = hrec; copy.idConsulta = h.proximoID;
ofstream f(ARCHIVO_HISTORIAL, ios::binary | ios::app);
if (!f.is_open()) return false;
f.write((char*)&copy, sizeof(HistorialRecord)); f.close();
h.cantidadRegistros++; h.registrosActivos++; h.proximoID++; actualizarHeader(ARCHIVO_HISTORIAL,h);
return true;
}


bool guardarHistorial(const HistorialRecord& hrec, int indice) {
if (indice < 0) return false;
fstream f(ARCHIVO_HISTORIAL, ios::binary | ios::in | ios::out);
if (!f.is_open()) return false;
f.seekp(calcularPosicion(indice, sizeof(HistorialRecord)));
f.write((char*)&hrec, sizeof(HistorialRecord)); f.close();
return true;
}


HistorialRecord leerHistorialPorIndice(int indice) {
HistorialRecord h; memset(&h,0,sizeof(h));
ifstream f(ARCHIVO_HISTORIAL, ios::binary);
if (!f.is_open()) return h;
long pos = calcularPosicion(indice, sizeof(HistorialRecord)); f.seekg(pos); f.read((char*)&h, sizeof(HistorialRecord)); f.close();
return h;
}


int buscarIndiceHistorialPorID(int id) {
ArchivoHeader h = leerHeader(ARCHIVO_HISTORIAL);
for (int i=0;i<h.cantidadRegistros;i++) {
HistorialRecord hr = leerHistorialPorIndice(i);
if (hr.idConsulta == id && !hr.eliminado) return i;
}
return -1;
}


// ---------- HOSPITAL ----------


bool guardarHospital(const HospitalRecord& hrec) {
// ensure file exists with header
if (!verificarArchivo(ARCHIVO_HOSPITAL)) inicializarArchivo(ARCHIVO_HOSPITAL);
fstream f(ARCHIVO_HOSPITAL, ios::binary | ios::in | ios::out);
if (!f.is_open()) return false;
f.seekp(sizeof(ArchivoHeader)); f.write((char*)&hrec, sizeof(HospitalRecord)); f.close();
return true;
}


HospitalRecord leerHospital() {
HospitalRecord h; memset(&h,0,sizeof(h));
if (!verificarArchivo(ARCHIVO_HOSPITAL)) inicializarArchivo(ARCHIVO_HOSPITAL);
ifstream f(ARCHIVO_HOSPITAL, ios::binary);
if (!f.is_open()) return h;
ArchivoHeader hh; f.read((char*)&hh, sizeof(hh));
f.seekg(sizeof(ArchivoHeader));
if (f.peek() != EOF) f.read((char*)&h, sizeof(HospitalRecord));
f.close();
return h;
}


} // namespace Gestor