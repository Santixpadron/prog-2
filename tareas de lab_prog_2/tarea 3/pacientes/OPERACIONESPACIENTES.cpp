#include "operacionesPacientes.hpp"
#include "../persistencia/GestorArchivos.hpp"
#include "../utilidades/validaciones.hpp"
#include <iostream>


using namespace std;
using namespace Gestor;


bool OperPac::registrarPaciente(Hospital& h) {
Paciente pObj;
PacienteRecord pr; memset(&pr,0,sizeof(pr));
char nombre[50], apellido[50], cedula[20], tipoS[5], telefono[15], direccion[100], email[50];
int edad; char sexo;
cout << "Nombre: "; cin.getline(nombre,50);
cout << "Apellido: "; cin.getline(apellido,50);
cout << "Cedula (V-...): "; cin.getline(cedula,20);
if (!Util::validarCedula(cedula)) { cout << "Cedula invalida\n"; return false; }
if (buscarPacientePorCedula(cedula).getId() != 0) { cout << "Cedula ya registrada\n"; return false; }
cout << "Edad: "; cin >> edad; cin.ignore(10000,'\n');
cout << "Sexo (M/F): "; cin >> sexo; cin.ignore(10000,'\n');
cout << "Tipo sangre: "; cin.getline(tipoS,5);
cout << "Telefono: "; cin.getline(telefono,15);
cout << "Direccion: "; cin.getline(direccion,100);
cout << "Email: "; cin.getline(email,50);
if (!Util::validarEmail(email)) Util::asegurarString(email, "noemail@hospital", sizeof(email));
// asignar
OperPac::listarPacientes(); // opcional: mostrar
OperPac::listarPacientes();
pObj.setNombre(nombre); pObj.setApellido(apellido); pObj.setCedula(cedula); pObj.setEdad(edad);
// persistir usando record
pr = pObj.toRecord(); // but id etc will be set in gestor
if (!Gestor::agregarPaciente(pr)) { cout << "Error al crear paciente\n"; return false; }
// actualizar hospital contadores
h.toRecord(); // placeholder
cout << "Paciente creado.\n";
return true;
}


void OperPac::listarPacientes() {
ArchivoHeader h = Gestor::leerHeader("pacientes.bin");
cout << "=== LISTA DE PACIENTES (" << h.registrosActivos << " activos) ===\n";
for (int i=0;i<h.cantidadRegistros;i++) {
PacienteRecord pr = Gestor::leerPacientePorIndice(i);
if (!pr.eliminado) {
cout << pr.id << " - " << pr.nombre << " " << pr.apellido << " (" << pr.cedula << ")\n";
}
}
}


Paciente OperPac::buscarPacientePorID(int id) {
Paciente vacio;
int idx = Gestor::buscarIndicePacientePorID(id);
if (idx == -1) return vacio;
PacienteRecord pr = Gestor::leerPacientePorIndice(idx);
Paciente p; p.fromRecord(pr); return p;
}


Paciente OperPac::buscarPacientePorCedula(const char* cedula) {
Paciente vacio;
PacienteRecord pr = Gestor::buscarPacientePorCedula(cedula);
if (pr.id == 0) return vacio;
Paciente p; p.fromRecord(pr); return p;
}


bool OperPac::eliminarPaciente(int id) {
int idx = Gestor::buscarIndicePacientePorID(id);
if (idx == -1) return false;
PacienteRecord pr = Gestor::leerPacientePorIndice(idx);
pr.eliminado = true; pr.activo = false;
if (!Gestor::guardarPaciente(pr, idx)) return false;
ArchivoHeader head = Gestor::leerHeader("pacientes.bin");
head.registrosActivos = max(0, head.registrosActivos - 1);
Gestor::actualizarHeader("pacientes.bin", head);
return true;
}