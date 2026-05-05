#include <iostream>
#include <vector>
#include <chrono>
#include <fstream>
#include <cstdlib> 
#include <ctime>

using namespace std;
using namespace std::chrono;
using Matrix = vector<vector<double>>;

void llenarMatriz(Matrix &M, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            M[i][j] = (double)(rand() % 100); 
        }
    }
}

void multiplicacionEstandar(const Matrix &A, const Matrix &B, Matrix &C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0; 
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

//Funciones auxiliares para aritmetica de matrices 
void sumar(const Matrix& A, const Matrix& B, Matrix& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] + B[i][j];
        }
    }
}

void restar(const Matrix& A, const Matrix& B, Matrix& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[i][j] - B[i][j];
        }
    }
}

//Funcion auxiliar para dividir la matriz en 4 submatrices
void dividir(const Matrix &A, Matrix &A11, Matrix &A12, Matrix &A21, Matrix &A22, int n) {
    int k = n / 2;
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            A11[i][j] = A[i][j];
            A12[i][j] = A[i][j + k];
            A21[i][j] = A[i + k][j];
            A22[i][j] = A[i + k][j + k];
        }
    }
}


void unir(const Matrix &C11, const Matrix &C12, const Matrix &C21, const Matrix &C22, Matrix &C, int n) {
    int k = n / 2;
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            C[i][j] = C11[i][j];
            C[i][j + k] = C12[i][j];
            C[i + k][j] = C21[i][j];
            C[i + k][j + k] = C22[i][j];
        }
    }
}
int main() {
    srand(time(NULL));

    ofstream archivo("data/resultados.dat");
    if (!archivo.is_open()) {
        cerr << "Error: No se pudo abrir la carpeta data/." << endl;
        return 1;
    }
    
    archivo << "# n\tTiempo(us)" << endl; 

    int nValores[] = {16, 24, 32, 48, 64, 96, 128, 192, 256, 512};

    for (int n : nValores) {
        Matrix A(n, vector<double>(n));
        Matrix B(n, vector<double>(n));
        Matrix C(n, vector<double>(n, 0.0));

        llenarMatriz(A, n);
        llenarMatriz(B, n);

        // Para valores de n < 128 se repite 50 veces; para n >= 128 se repite 5 veces
        int repeticiones = (n < 128) ? 50 : 5; 
        long long sumaTiempos = 0;

        cout << "Para n = " << n << " (" << repeticiones << " veces)  " << flush;

        for (int r = 0; r < repeticiones; r++) {
            auto inicio = high_resolution_clock::now();
            multiplicacionEstandar(A, B, C, n);
            auto fin = high_resolution_clock::now();
            
            auto duracion = duration_cast<microseconds>(fin - inicio);
            sumaTiempos += duracion.count();
        }

        // Para promedio de las repeticiones
        double promedio = (double)sumaTiempos / repeticiones;

        archivo << n << "\t" << promedio << endl;
        cout<<"Promedio: "<< promedio << "us" << endl;
    }

    archivo.close();

    return 0;
}
