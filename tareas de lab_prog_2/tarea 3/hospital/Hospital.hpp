#ifndef HOSPITAL_HPP
#define HOSPITAL_HPP


#include <cstring>


struct HospitalRecord {
char nombre[100];
char direccion[150];
char telefono[15];
int siguienteIdPaciente;
int siguienteIdDoctor;
int siguienteIdCita;
int siguienteIdConsulta;
int totalPacientesRegistrados;
int totalDoctoresRegistrados;
int totalCitasAgendadas;
int totalConsultasRealizadas;
};


class Hospital {
private:
HospitalRecord r;
public:
Hospital();
// getters
const char* getNombre() const;
// setters
void setNombre(const char* nombre);
// raw access for persistence
const HospitalRecord& toRecord() const;
void fromRecord(const HospitalRecord& rec);
};


#endif