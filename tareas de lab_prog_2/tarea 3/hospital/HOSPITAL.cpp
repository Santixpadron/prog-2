#include "Hospital.hpp"
#include "../utilidades/Validaciones.hpp"


using namespace Util;


Hospital::Hospital() {
memset(&r,0,sizeof(r));
asegurarString(r.nombre, "Hospital Central", sizeof(r.nombre));
asegurarString(r.direccion, "Direccion no especificada", sizeof(r.direccion));
asegurarString(r.telefono, "0000000000", sizeof(r.telefono));
r.siguienteIdPaciente = 1;
r.siguienteIdDoctor = 1;
r.siguienteIdCita = 1;
r.siguienteIdConsulta = 1;
r.totalPacientesRegistrados = 0;
r.totalDoctoresRegistrados = 0;
r.totalCitasAgendadas = 0;
r.totalConsultasRealizadas = 0;
}


const char* Hospital::getNombre() const { return r.nombre; }
void Hospital::setNombre(const char* nombre) { asegurarString(r.nombre, nombre, sizeof(r.nombre)); }


const HospitalRecord& Hospital::toRecord() const { return r; }
void Hospital::fromRecord(const HospitalRecord& rec) { r = rec; }