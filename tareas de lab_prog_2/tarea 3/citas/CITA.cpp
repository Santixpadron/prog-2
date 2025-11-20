#include "Cita.hpp"
#include <cstring>


Cita::Cita() { memset(&r,0,sizeof(r)); r.id=0; r.atendida=false; r.eliminado=false; r.consultaID=-1; }
const CitaRecord& Cita::toRecord() const { return r; }
void Cita::fromRecord(const CitaRecord& rec) { r = rec; }
```


---


### File: citas/operacionesCitas.hpp
```cpp
#ifndef OPERACIONES_CITAS_HPP
#define OPERACIONES_CITAS_HPP


#include "Cita.hpp"
#include "../hospital/Hospital.hpp"


namespace OperCitas {
bool agendarCita(Hospital& h);
bool cancelarCita(int id);
bool atenderCita(Hospital& h, int id);
}