#include "Paciente.hpp"
#include "../utilidades/Validaciones.hpp"


using namespace Util;


Paciente::Paciente() { memset(&r,0,sizeof(r)); r.id = 0; r.cantidadConsultas=0; r.primerConsultaID=-1; r.cantidadCitas=0; r.activo=false; r.eliminado=false; }


int Paciente::getId() const { return r.id; }
const char* Paciente::getNombre() const { return r.nombre; }
const char* Paciente::getApellido() const { return r.apellido; }
const char* Paciente::getCedula() const { return r.cedula; }
int Paciente::getEdad() const { return r.edad; }


void Paciente::setNombre(const char* nombre) { asegurarString(r.nombre, nombre, sizeof(r.nombre)); }
void Paciente::setApellido(const char* apellido) { asegurarString(r.apellido, apellido, sizeof(r.apellido)); }
void Paciente::setCedula(const char* cedula) { if (validarCedula(cedula)) asegurarString(r.cedula, cedula, sizeof(r.cedula)); }
void Paciente::setEdad(int edad) { if (edad>=0 && edad<=120) r.edad = edad; }


const PacienteRecord& Paciente::toRecord() const { return r; }
void Paciente::fromRecord(const PacienteRecord& rec) { r = rec; }