#include <iostream>
#include <limits>
#include "hospital/Hospital.hpp"
#include "persistencia/GestorArchivos.hpp"
#include "pacientes/operacionesPacientes.hpp"
#include "doctores/operacionesDoctores.hpp"
#include "citas/operacionesCitas.hpp"


using namespace std;
using namespace Gestor;
using namespace OperPac;
using namespace OperDoc;
using namespace OperCitas;

void pausar() { cout << "\nPresione ENTER para continuar..."; cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

void menuPacientes(Hospital& hosp) {
	while (true) {
		cout << "=== GESTION DE PACIENTES ===\n";
		cout << "1. Crear paciente\n2. Listar pacientes\n3. Buscar por cedula\n4. Eliminar paciente\n0. Volver\n";
		cout << "Opcion: ";
		int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
		cin.ignore(10000,'\n');
		if (op==0) break;
		if (op==1) { OperPac::registrarPaciente(hosp); pausar(); }
		else if (op==2) { OperPac::listarPacientes(); pausar(); }
		else if (op==3) { char ced[20]; cout << "Cedula: "; cin.getline(ced,20); OperPac::buscarPacientePorCedula(ced); pausar(); }
		else if (op==4) { int id; cout << "ID a eliminar: "; cin >> id; cin.ignore(10000,'\n'); OperPac::eliminarPaciente(id); pausar(); }
		else { cout << "Opcion invalida\n"; pausar(); }
	}
}

void menuDoctores(Hospital& hosp) {
	while (true) {
		cout << "=== GESTION DE DOCTORES ===\n";
		cout << "1. Crear doctor\n2. Listar doctores\n3. Eliminar doctor\n0. Volver\n";
		cout << "Opcion: ";
		int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
		cin.ignore(10000,'\n');
		if (op==0) break;
		if (op==1) { OperDoc::registrarDoctor(hosp); pausar(); }
		else if (op==2) { OperDoc::listarDoctores(); pausar(); }
		else if (op==3) { int id; cout << "ID a eliminar: "; cin >> id; cin.ignore(10000,'\n'); OperDoc::eliminarDoctor(id); pausar(); }
		else { cout << "Opcion invalida\n"; pausar(); }
	}
}

void menuCitas(Hospital& hosp) {
	while (true) {
		cout << "=== GESTION DE CITAS ===\n";
		cout << "1. Agendar cita\n2. Cancelar cita\n3. Atender cita\n0. Volver\n";
		cout << "Opcion: ";
		int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
		cin.ignore(10000,'\n');
		if (op==0) break;
		if (op==1) { OperCitas::agendarCita(hosp); pausar(); }
		else if (op==2) { int id; cout << "ID cita a cancelar: "; cin >> id; cin.ignore(10000,'\n'); OperCitas::cancelarCita(id); pausar(); }
		else if (op==3) { int id; cout << "ID cita a atender: "; cin >> id; cin.ignore(10000,'\n'); OperCitas::atenderCita(hosp, id); pausar(); }
		else { cout << "Opcion invalida\n"; pausar(); }
	}
}

void menuReportes(Hospital& hosp) {
	(void)hosp;
	while (true) {
		cout << "=== REPORTES ===\n1. Estadisticas\n0. Volver\nOpcion: ";
		int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
		cin.ignore(10000,'\n');
		if (op==0) break;
		if (op==1) {
			ArchivoHeader pa = Gestor::leerHeader("pacientes.bin");
			ArchivoHeader da = Gestor::leerHeader("doctores.bin");
			ArchivoHeader ca = Gestor::leerHeader("citas.bin");
			ArchivoHeader ha = Gestor::leerHeader("historiales.bin");
			cout << "PACIENTES: " << pa.registrosActivos << " activos de " << pa.cantidadRegistros << "\n";
			cout << "DOCTORES: " << da.registrosActivos << " activos de " << da.cantidadRegistros << "\n";
			cout << "CITAS: " << ca.registrosActivos << " activas de " << ca.cantidadRegistros << "\n";
			cout << "CONSULTAS: " << ha.registrosActivos << " activas de " << ha.cantidadRegistros << "\n";
			pausar();
		} else { cout << "Opcion invalida\n"; pausar(); }
	}
}

void menuMantenimiento(Hospital& hosp) {
	(void)hosp;
	while (true) {
		cout << "=== MANTENIMIENTO ===\n1. Verificar archivos\n2. Hacer respaldo (simulado)\n3. Restaurar respaldo (simulado)\n0. Volver\nOpcion: ";
		int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
		cin.ignore(10000,'\n');
		if (op==0) break;
		if (op==1) {
			bool ok = Gestor::verificarArchivo("pacientes.bin") && Gestor::verificarArchivo("doctores.bin") && Gestor::verificarArchivo("citas.bin") && Gestor::verificarArchivo("historiales.bin") && Gestor::verificarArchivo("hospital.bin");
			cout << (ok ? "Archivos OK\n" : "Faltan o estan corruptos\n"); pausar();
		} else if (op==2) { cout << "Respaldo simulado...\n"; pausar(); }
		else if (op==3) { cout << "Restauracion simulada...\n"; pausar(); }
		else { cout << "Opcion invalida\n"; pausar(); }
	}
}

int main() {
	// inicializar archivos si faltan
	if (!Gestor::verificarArchivo("pacientes.bin")) Gestor::inicializarArchivo("pacientes.bin");
	if (!Gestor::verificarArchivo("doctores.bin")) Gestor::inicializarArchivo("doctores.bin");
	if (!Gestor::verificarArchivo("citas.bin")) Gestor::inicializarArchivo("citas.bin");
	if (!Gestor::verificarArchivo("historiales.bin")) Gestor::inicializarArchivo("historiales.bin");
	if (!Gestor::verificarArchivo("hospital.bin")) Gestor::inicializarArchivo("hospital.bin");

	Hospital hosp;
	HospitalRecord hr = Gestor::leerHospital();
	hosp.fromRecord(hr);

	while (true) {
		cout << "=========================================\n";
		cout << " SISTEMA HOSPITALARIO - MENU PRINCIPAL\n";
		cout << "=========================================\n";
		cout << "1. Pacientes\n2. Doctores\n3. Citas\n4. Reportes\n5. Mantenimiento\n0. Salir\n";
		cout << "Seleccione una opcion: ";
		int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
		cin.ignore(10000,'\n');
		if (op==0) { cout << "Guardando y saliendo...\n"; Gestor::guardarHospital(hosp.toRecord()); break; }
		if (op==1) { menuPacientes(hosp); }
		else if (op==2) { menuDoctores(hosp); }
		else if (op==3) { menuCitas(hosp); }
		else if (op==4) { menuReportes(hosp); }
		else if (op==5) { menuMantenimiento(hosp); }
		else { cout << "Opcion invalida\n"; pausar(); }
	}
	return 0;
}