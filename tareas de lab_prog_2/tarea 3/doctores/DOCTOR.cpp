#include "Doctor.hpp"
#include <cstring>


Doctor::Doctor() { memset(&r,0,sizeof(r)); r.id=0; r.cantidadPacientes=0; r.cantidadCitas=0; r.disponible=true; r.eliminado=false; }
const DoctorRecord& Doctor::toRecord() const { return r; }
void Doctor::fromRecord(const DoctorRecord& rec) { r = rec; 
}