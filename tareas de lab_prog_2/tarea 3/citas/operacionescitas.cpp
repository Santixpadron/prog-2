#include "operacionescitas.hpp"
#include "../persistencia/gestorarchivos.hpp"
#include "../utilidades/validaciones.hpp"
#include <iostream>
#include <cstring>
#include <algorithm>


using namespace std;
using namespace Gestor;
using namespace Util;


bool verificarDisponibilidad(int idDoctor, const char* fecha, const char* hora) {
        
ArchivoHeader h = Gestor::leerHeader("citas.bin");
for (int i=0;i<h.cantidadRegistros;i++) {
CitaRecord cr = Gestor::leerCitaPorIndice(i);
if (!cr.eliminado && cr.idDoctor==idDoctor && strcmp(cr.fecha, fecha)==0 && strcmp(cr.hora,hora)==0 && strcmp(cr.estado,"Cancelada")!=0) return false;
}
return true;
}


bool OperCitas::agendarCita(Hospital& h) {
int idP, idD; char fecha[11], hora[6], motivo[150];
cout << "ID paciente: "; cin >> idP; cin.ignore(10000,'\n');
cout << "ID doctor: "; cin >> idD; cin.ignore(10000,'\n');
cout << "Fecha (YYYY-MM-DD): "; cin.getline(fecha,11);
cout << "Hora (HH:MM): "; cin.getline(hora,6);
cout << "Motivo: "; cin.getline(motivo,150);
if (!validarFecha(fecha) || !validarHora(hora)) { cout << "Fecha/hora invalidas\n"; return false; }
if (!verificarDisponibilidad(idD, fecha, hora)) { cout << "Doctor no disponible\n"; return false; }
// crear
CitaRecord cr; memset(&cr,0,sizeof(cr)); cr.idPaciente = idP; cr.idDoctor = idD; strncpy(cr.fecha, fecha,10); strncpy(cr.hora,hora,5); strncpy(cr.motivo,motivo,149); strncpy(cr.estado,"Programada",19); cr.atendida=false; cr.eliminado=false; cr.consultaID=-1;
if (!Gestor::agregarCita(cr)) { cout << "Error al guardar cita\n"; return false; }
cout << "Cita agendada.\n"; return true;
}


bool OperCitas::cancelarCita(int id) {
int idx = Gestor::buscarIndiceCitaPorID(id);
if (idx==-1) return false;
CitaRecord cr = Gestor::leerCitaPorIndice(idx);
strncpy(cr.estado, "Cancelada", sizeof(cr.estado)-1); cr.atendida=false; cr.eliminado=true;
if (!Gestor::guardarCita(cr, idx)) return false;
ArchivoHeader h = Gestor::leerHeader("citas.bin"); h.registrosActivos = max(0, h.registrosActivos-1); Gestor::actualizarHeader("citas.bin", h);
return true;
}


bool OperCitas::atenderCita(Hospital& h, int id) {
int idx = Gestor::buscarIndiceCitaPorID(id);
if (idx==-1) return false;
CitaRecord cr = Gestor::leerCitaPorIndice(idx);
if (cr.atendida) return false;
// solicitar diagnostico
char diag[200], trat[200], meds[150];
cout << "Diagnostico: "; cin.getline(diag,200);
cout << "Tratamiento: "; cin.getline(trat,200);
cout << "Medicamentos: "; cin.getline(meds,150);
HistorialRecord hr; memset(&hr,0,sizeof(hr));
strncpy(hr.fecha, cr.fecha, sizeof(hr.fecha)-1); strncpy(hr.hora, cr.hora, sizeof(hr.hora)-1);
strncpy(hr.diagnostico, diag, sizeof(hr.diagnostico)-1); strncpy(hr.tratamiento, trat, sizeof(hr.tratamiento)-1); strncpy(hr.medicamentos, meds, sizeof(hr.medicamentos)-1);
hr.idDoctor = cr.idDoctor;
DoctorRecord dr = Gestor::leerDoctorPorIndice(Gestor::buscarIndiceDoctorPorID(cr.idDoctor));
hr.costo = dr.costoConsulta;
hr.eliminado = false; hr.siguienteConsultaID = -1; hr.idPaciente = cr.idPaciente;
if (!Gestor::agregarHistorial(hr)) { cout << "Error al crear historial\n"; return false; }
ArchivoHeader hh = Gestor::leerHeader("historiales.bin"); int idNuevaConsulta = hh.proximoID - 1;
cr.atendida = true; strncpy(cr.estado, "Atendida", sizeof(cr.estado)-1); cr.consultaID = idNuevaConsulta;
if (!Gestor::guardarCita(cr, idx)) return false;
cout << "Cita atendida y consulta creada #" << idNuevaConsulta << "\n";
return true;
}
