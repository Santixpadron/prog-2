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
cout << "1. Pacientes\n2. Doctores\n3. Citas\n0. Salir\n";
cout << "Seleccione una opcion: ";
int op; if (!(cin >> op)) { cin.clear(); cin.ignore(10000,'\n'); op=-1; }
cin.ignore(10000,'\n');
if (op==0) { cout << "Guardando y saliendo...\n"; Gestor::guardarHospital(hosp.toRecord()); break; }
if (op==1) { OperPac::registrarPaciente(hosp); pausar(); }
else if (op==2) { OperDoc::registrarDoctor(hosp); pausar(); }
else if (op==3) { OperCitas::agendarCita(hosp); pausar(); }
else { cout << "Opcion invalida\n"; pausar(); }
}
return 0;
}