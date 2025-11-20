#ifndef OPERACIONES_PACIENTES_HPP
#define OPERACIONES_PACIENTES_HPP


#include "Paciente.hpp"
#include "../hospital/Hospital.hpp"


namespace OperPac {
bool registrarPaciente(Hospital& h);
void listarPacientes();
Paciente buscarPacientePorID(int id);
Paciente buscarPacientePorCedula(const char* cedula);
bool eliminarPaciente(int id);
}


#endif