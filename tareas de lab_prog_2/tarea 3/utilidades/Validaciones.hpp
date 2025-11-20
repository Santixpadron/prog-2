#ifndef VALIDACIONES_HPP
#define VALIDACIONES_HPP


#include <cstring>


namespace Util {
bool validarEmail(const char* email);
bool validarFecha(const char* fecha);
bool validarHora(const char* hora);
bool validarCedula(const char* cedula);
void asegurarString(char* dest, const char* src, size_t n);
}


#endif