#include "Validaciones.hpp"
namespace Util {


void asegurarString(char* dest, const char* src, size_t n) {
if (!src) { dest[0] = '\0'; return; }
strncpy(dest, src, n-1);
dest[n-1] = '\0';
}


bool validarEmail(const char* email) {
if (!email) return false;
int len = (int)strlen(email);
if (len < 3 || len >= 50) return false;
const char* at = strchr(email, '@');
if (!at || at == email) return false;
if (strchr(at+1, '@')) return false;
if (!strchr(at+1, '.')) return false;
return true;
}


bool validarFecha(const char* fecha) {
if (!fecha) return false;
if (strlen(fecha) != 10) return false; // YYYY-MM-DD
if (fecha[4] != '-' || fecha[7] != '-') return false;
for (int i=0;i<10;i++) {
if (i==4||i==7) continue;
if (!isdigit((unsigned char)fecha[i])) return false;
}
int mes = atoi(string(fecha, fecha+10).substr(5,2).c_str());
int dia = atoi(string(fecha, fecha+10).substr(8,2).c_str());
if (mes<1||mes>12) return false;
if (dia<1||dia>31) return false;
return true;
}


bool validarHora(const char* hora) {
if (!hora) return false;
if (strlen(hora) != 5) return false; // HH:MM
if (hora[2] != ':') return false;
for (int i=0;i<5;i++) {
if (i==2) continue;
if (!isdigit((unsigned char)hora[i])) return false;
}
int hh = atoi(string(hora, hora+2).c_str());
int mm = atoi(string(hora+3, hora+5).c_str());
if (hh<0||hh>23) return false;
if (mm<0||mm>59) return false;
return true;
}


bool validarCedula(const char* cedula) {
if (!cedula) return false;
int len = (int)strlen(cedula);
if (len < 3 || len > 20) return false;
if (!(cedula[0]=='V' || cedula[0]=='v' || cedula[0]=='E' || cedula[0]=='e')) return false;
if (cedula[1] != '-') return false;
for (int i=2;i<len;i++) if (!isdigit((unsigned char)cedula[i])) return false;
return true;
}


}