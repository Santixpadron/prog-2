#ifndef GESTOR_ARCHIVOS_HPP
#define GESTOR_ARCHIVOS_HPP


#include <string>
#include <vector>
#include "../hospital/Hospital.hpp"
#include "../pacientes/Paciente.hpp"
#include "../doctores/Doctor.hpp"
#include "../citas/Cita.hpp"
#include "../historial/HistorialMedico.hpp"


struct ArchivoHeader {
int cantidadRegistros;
int proximoID;
int registrosActivos;
int version;
};


namespace Gestor {
bool inicializarArchivo(const char* nombreArchivo, int proximoID = 1);
ArchivoHeader leerHeader(const char* nombreArchivo);
bool actualizarHeader(const char* nombreArchivo, const ArchivoHeader& h);
bool verificarArchivo(const char* nombreArchivo);


// Paciente
bool agregarPaciente(const PacienteRecord& p);
bool guardarPaciente(const PacienteRecord& p, int indice);
PacienteRecord leerPacientePorIndice(int indice);
int buscarIndicePacientePorID(int id);
PacienteRecord buscarPacientePorCedula(const char* cedula);


// Doctor
bool agregarDoctor(const DoctorRecord& d);
bool guardarDoctor(const DoctorRecord& d, int indice);
DoctorRecord leerDoctorPorIndice(int indice);
int buscarIndiceDoctorPorID(int id);


// Cita
bool agregarCita(const CitaRecord& c);
bool guardarCita(const CitaRecord& c, int indice);
CitaRecord leerCitaPorIndice(int indice);
int buscarIndiceCitaPorID(int id);


// Historial
bool agregarHistorial(const HistorialRecord& h);
bool guardarHistorial(const HistorialRecord& h, int indice);
HistorialRecord leerHistorialPorIndice(int indice);
int buscarIndiceHistorialPorID(int id);


// Hospital
bool guardarHospital(const HospitalRecord& hrec);
HospitalRecord leerHospital();
}


#endif// GESTOR_ARCHIVOS_HPP