#ifndef DOCTOR_HPP
#define DOCTOR_HPP


const int MAX_PACIENTES_DOCTOR = 50;
const int MAX_CITAS_DOCTOR = 50;


struct DoctorRecord {
int id;
char nombre[50];
char apellido[50];
char cedula[20];
char especialidad[50];
int aniosExperiencia;
float costoConsulta;
char horarioAtencion[50];
char telefono[15];
char email[50];
int cantidadPacientes;
int pacientesIDs[MAX_PACIENTES_DOCTOR];
int cantidadCitas;
int citasIDs[MAX_CITAS_DOCTOR];
bool disponible;
bool eliminado;
};


class Doctor {
private:
DoctorRecord r;
public:
Doctor();
const DoctorRecord& toRecord() const;
void fromRecord(const DoctorRecord& rec);
};


#endif