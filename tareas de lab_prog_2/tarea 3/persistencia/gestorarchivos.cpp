#include "GestorArchivos.hpp"
#include <fstream>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cstring>


using namespace std;


// archivos
static const char* ARCHIVO_PACIENTES = "pacientes.bin";
static const char* ARCHIVO_DOCTORES = "doctores.bin";
static const char* ARCHIVO_CITAS = "citas.bin";
static const char* ARCHIVO_HISTORIAL = "historiales.bin";
static const char* ARCHIVO_HOSPITAL = "hospital.bin";


const int VERSION_ACTUAL = 1;


namespace Gestor {


bool inicializarArchivo(const char* nombreArchivo, int proximoID) {
ofstream out(nombreArchivo, ios::binary | ios::trunc);
if (!out.is_open()) return false;
ArchivoHeader h; h.cantidadRegistros = 0; h.proximoID = proximoID; h.registrosActivos = 0; h.version = VERSION_ACTUAL;
out.write((char*)&h, sizeof(h));
out.close();
return true;
}


ArchivoHeader leerHeader(const char* nombreArchivo) {
ArchivoHeader h; memset(&h,0,sizeof(h));
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
bool agregarPaciente(const PacienteRecord& p) {

// ---------- PACIENTES ----------
ArchivoHeader h = leerHeader(ARCHIVO_PACIENTES);
fstream f(ARCHIVO_PACIENTES, ios::binary | ios::in | ios::out);
if (!f.is_open()) return false;
f.seekp(calcularPosicion(h.cantidadRegistros, sizeof(PacienteRecord)));
f.write((char*)&p, sizeof(PacienteRecord));
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
// ---------- DOCTORES ----------
bool agregarDoctor(const DoctorRecord& d) {
ArchivoHeader h = leerHeader(ARCHIVO_DOCTORES);
DoctorRecord copy = d; copy.id = h.proximoID;
ofstream f(ARCHIVO_DOCTORES, ios::binary | ios::app);
if (!f.is_open()) return false;
f.write((char*)&copy, sizeof(DoctorRecord)); f.close();
h.cantidadRegistros++; h.registrosActivos++; h.proximoID++; actualizarHeader(ARCHIVO_DOCTORES,h);
return true;
}


bool guardarDoctor(const DoctorRecord& d, int indice) {
if (indice < 0) return false;
fstream f(ARCHIVO_DOCTORES, ios::binary | ios::in | ios::out);
if (!f.is_open()) return false;
f.seekp(calcularPosicion(indice, sizeof(DoctorRecord)));
f.write((char*)&d, sizeof(DoctorRecord)); f.close();
return true;
}


DoctorRecord leerDoctorPorIndice(int indice) {
DoctorRecord d; memset(&d,0,sizeof(d));
ifstream f(ARCHIVO_DOCTORES, ios::binary);
if (!f.is_open()) return d;
long pos = calcularPosicion(indice, sizeof(DoctorRecord));
f.seekg(pos); f.read((char*)&d, sizeof(DoctorRecord)); f.close();
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
bool agregarHistorial(const HistorialRecord& hrec) {
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
}