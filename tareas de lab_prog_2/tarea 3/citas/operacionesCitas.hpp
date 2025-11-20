#ifndef OPERACIONES_CITAS_HPP
#define OPERACIONES_CITAS_HPP


#include "Cita.hpp"
#include "../hospital/Hospital.hpp"


namespace OperCitas {
bool agendarCita(Hospital& h);
bool cancelarCita(int id);
bool atenderCita(Hospital& h, int id);
}