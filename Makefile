# Variables de compilación
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17
TARGET = matrix_bench
SRC = src/main.cpp
DATA_FILE = data/resultados.dat
OUT_IMAGE = data/grafica.png

# Regla principal
all: setup $(TARGET)

# Crear carpeta de datos si no existe
setup:
	@mkdir -p data

# Compilar el ejecutable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Ejecutar 
run: all
	./$(TARGET)

# Generar la gráfica usando Gnuplot
plot: run
	@echo "Generando gráfica en $(OUT_IMAGE)"
	@echo "set terminal png size 800,600; \
	       set output '$(OUT_IMAGE)'; \
	       set title 'Rendimiento: Multiplicación de Matrices'; \
	       set xlabel 'Tamaño n'; \
	       set ylabel 'Tiempo (us)'; \
	       set grid; \
	       plot '$(DATA_FILE)' with linespoints lw 2 pt 7 title 'Algoritmo Estándar'" | gnuplot

# Limpiar archivos binarios y datos
clean:
	rm -f $(TARGET)
	rm -rf data/*.dat data/*.png

.PHONY: all setup run plot clean
