#include "operacionesDoctores.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include <iostream>


using namespace std;
using namespace Gestor;


bool OperDoc::registrarDoctor(Hospital& h) {
DoctorRecord dr; memset(&dr,0,sizeof(dr));
char nombre[50], apellido[50], ced[20], esp[50], horario[50], telefono[15], email[50];
int exp; float costo;
cout << "Nombre: "; cin.getline(nombre,50);
cout << "Apellido: "; cin.getline(apellido,50);
cout << "Cedula profesional: "; cin.getline(ced,20);
cout << "Especialidad: "; cin.getline(esp,50);
cout << "Anios experiencia: "; cin >> exp; cin.ignore(10000,'\n');
cout << "Costo consulta: "; cin >> costo; cin.ignore(10000,'\n');
cout << "Horario: "; cin.getline(horario,50);
cout << "Telefono: "; cin.getline(telefono,15);
cout << "Email: "; cin.getline(email,50);
strncpy(dr.nombre, nombre, sizeof(dr.nombre)-1);
strncpy(dr.apellido, apellido, sizeof(dr.apellido)-1);
strncpy(dr.cedula, ced, sizeof(dr.cedula)-1);
strncpy(dr.especialidad, esp, sizeof(dr.especialidad)-1);
dr.aniosExperiencia = exp; dr.costoConsulta = costo;
strncpy(dr.horarioAtencion, horario, sizeof(dr.horarioAtencion)-1);
strncpy(dr.telefono, telefono, sizeof(dr.telefono)-1);
strncpy(dr.email, email, sizeof(dr.email)-1);
if (!Gestor::agregarDoctor(dr)) { cout << "Error al crear doctor\n"; return false; }
cout << "Doctor creado.\n";
return true;
}


void OperDoc::listarDoctores() {
ArchivoHeader h = Gestor::leerHeader("doctores.bin");
cout << "=== LISTA DE DOCTORES (" << h.registrosActivos << " activos) ===\n";
for (int i=0;i<h.cantidadRegistros;i++) {
DoctorRecord dr = Gestor::leerDoctorPorIndice(i);
if (!dr.eliminado) cout << dr.id << " - " << dr.nombre << " " << dr.apellido << " - " << dr.especialidad << "\n";
}
}


bool OperDoc::eliminarDoctor(int id) {
int idx = Gestor::buscarIndiceDoctorPorID(id);
if (idx == -1) return false;
DoctorRecord dr = Gestor::leerDoctorPorIndice(idx);
dr.eliminado = true; dr.disponible = false;
if (!Gestor::guardarDoctor(dr, idx)) return false;
ArchivoHeader head = Gestor::leerHeader("doctores.bin"); head.registrosActivos = max(0, head.registrosActivos-1);
Gestor::actualizarHeader("doctores.bin", head);
return true;
}