# ================================
#  MAKEFILE - Proyecto Hospital
# ================================

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I.

# Carpetas del proyecto
SRC_DIRS = . \
           hospital \
           pacientes \
           doctores \
           citas \
           historial \
           persistencia \
           utilidades

# Buscar todos los .cpp en las carpetas
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

# Carpeta para .o
OBJ_DIR = build

# Reemplazar .cpp por .o dentro de /build
OBJS = $(patsubst %.cpp,$(OBJ_DIR)/%.o,$(SRCS))

# Nombre del ejecutable
TARGET = hospital_app

# ================================

all: $(TARGET)

# Regla para crear el ejecutable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "-----------------------------------------"
	@echo " Compilación finalizada. Ejecutable listo "
	@echo "-----------------------------------------"

# Regla para cada archivo .o en build/
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpiar compilación
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

# Limpiar todo y recompilar
rebuild: clean all

# ================================
# Fin del Makefile
# ================================
.PHONY: all clean rebuild