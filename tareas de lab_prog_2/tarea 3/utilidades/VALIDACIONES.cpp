#include "Validaciones.hpp"
#include <cctype>
#include <cstring>
#include <string>
#include <cstdlib>

using namespace std;

namespace Util {

void asegurarString(char* dest, const char* src, size_t n) {
    if (!dest || n == 0) return;
    
    if (!src) {
        dest[0] = '\0';
        return;
    }
    
    strncpy(dest, src, n - 1);
    dest[n - 1] = '\0';
}

bool validarEmail(const char* email) {
    if (!email) return false;
    
    int len = static_cast<int>(strlen(email));
    if (len < 3 || len >= 50) return false;
    
    const char* at = strchr(email, '@');
    if (!at || at == email) return false;
    
    // Verificar que solo haya un '@'
    if (strchr(at + 1, '@')) return false;
    
    // Verificar que haya al menos un '.' después del '@'
    const char* dot = strchr(at + 1, '.');
    if (!dot) return false;
    
    // Verificar que el '.' no esté inmediatamente después del '@'
    if (dot == at + 1) return false;
    
    // Verificar que haya al menos un carácter después del '.'
    if (*(dot + 1) == '\0') return false;
    
    return true;
}

bool validarFecha(const char* fecha) {
    if (!fecha) return false;
    
    // Formato: YYYY-MM-DD
    if (strlen(fecha) != 10) return false;
    if (fecha[4] != '-' || fecha[7] != '-') return false;
    
    // Verificar que todos los caracteres excepto los guiones sean dígitos
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(static_cast<unsigned char>(fecha[i]))) return false;
    }

    // Extraer año, mes y día
    int anio = atoi(string(fecha, fecha + 4).c_str());
    int mes = atoi(string(fecha + 5, fecha + 7).c_str());
    int dia = atoi(string(fecha + 8, fecha + 10).c_str());
    
    // Validar rangos básicos
    if (anio < 1900 || anio > 2100) return false;
    if (mes < 1 || mes > 12) return false;
    if (dia < 1 || dia > 31) return false;
    
    // Validaciones específicas por mes
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        if (dia > 30) return false;
    } else if (mes == 2) {
        // Validación básica para febrero (sin considerar años bisiestos)
        if (dia > 29) return false;
    }
    
    return true;
}

bool validarHora(const char* hora) {
    if (!hora) return false;
    
    // Formato: HH:MM
    if (strlen(hora) != 5) return false;
    if (hora[2] != ':') return false;
    
    // Verificar que todos los caracteres excepto los dos puntos sean dígitos
    for (int i = 0; i < 5; i++) {
        if (i == 2) continue;
        if (!isdigit(static_cast<unsigned char>(hora[i]))) return false;
    }
    
    int hh = atoi(string(hora, hora + 2).c_str());
    int mm = atoi(string(hora + 3, hora + 5).c_str());
    
    if (hh < 0 || hh > 23) return false;
    if (mm < 0 || mm > 59) return false;
    
    return true;
}

bool validarCedula(const char* cedula) {
    if (!cedula) return false;
    
    int len = static_cast<int>(strlen(cedula));
    if (len < 3 || len > 20) return false;
    
    // Verificar formato: [VEve]- seguido de dígitos
    char primerChar = cedula[0];
    if (!(primerChar == 'V' || primerChar == 'v' || 
          primerChar == 'E' || primerChar == 'e')) {
        return false;
    }
    
    if (cedula[1] != '-') return false;
    
    // Verificar que el resto sean dígitos
    for (int i = 2; i < len; i++) {
        if (!isdigit(static_cast<unsigned char>(cedula[i]))) return false;
    }
    
    return true;
}

} // namespace Util