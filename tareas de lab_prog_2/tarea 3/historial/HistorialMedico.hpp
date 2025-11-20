#ifndef HISTORIAL_HPP
#define HISTORIAL_HPP


struct HistorialRecord {
int idConsulta;
int idPaciente;
char fecha[11];
char hora[6];
char diagnostico[200];
char tratamiento[200];
char medicamentos[150];
int idDoctor;
float costo;
int siguienteConsultaID;
bool eliminado;
};


class HistorialMedico {
private:
HistorialRecord r;
public:
HistorialMedico();
const HistorialRecord& toRecord() const;
void fromRecord(const HistorialRecord& rec);
};