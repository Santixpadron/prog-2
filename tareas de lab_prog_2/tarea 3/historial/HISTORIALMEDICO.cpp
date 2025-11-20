#include "HistorialMedico.hpp"
#include <cstring>


HistorialMedico::HistorialMedico() { memset(&r,0,sizeof(r)); r.idConsulta=0; r.idPaciente=0; r.siguienteConsultaID=-1; r.eliminado=false; }
const HistorialRecord& HistorialMedico::toRecord() const { return r; }
void HistorialMedico::fromRecord(const HistorialRecord& rec) { r = rec;
} 
