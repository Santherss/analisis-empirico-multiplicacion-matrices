# Variables de compilación
CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17 -march=native
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
	@echo "Generando gráfica en $(OUT_IMAGE)..."
	@echo "set terminal pngcairo size 1024,768 font 'Verdana,12'; \
	       set output '$(OUT_IMAGE)'; \
	       set title 'Análisis de Rendimiento: Estándar vs Strassen (Umbral n=32)'; \
	       set xlabel 'Dimensión de la Matriz (n)'; \
	       set ylabel 'Tiempo de Ejecución (microsegundos)'; \
	       set logscale xy 2; \
	       set xtics (16, 24, 32, 40, 48, 64, 128, 256, 512, 1024); \
	       set grid xtics ytics ls 12; \
	       set key left top; \
	       plot '$(DATA_FILE)' using 1:2 with linespoints lw 2 lc rgb '#e41a1c' pt 7 ps 1.5 title 'Algoritmo Estándar', \
	            '$(DATA_FILE)' using 1:3 with linespoints lw 2 lc rgb '#377eb8' pt 5 ps 1.5 title 'Algoritmo Strassen'" | gnuplot
# Limpiar archivos binarios y datos
clean:
	rm -f $(TARGET)
	rm -rf data/*.dat data/*.png

.PHONY: all setup run plot clean
