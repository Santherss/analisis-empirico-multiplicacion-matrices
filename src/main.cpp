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

//Funcion auxiliar para dividir la matriz A en 4 submatrices
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

void strassen(const Matrix &A, const Matrix &B, Matrix &C, int n) {
    if (n <= 64) {
        multiplicacionEstandar(A, B, C, n);
        return;
    }

    int k = n / 2;

    // Dividir
    Matrix a(k, vector<double>(k)), b(k, vector<double>(k)), c(k, vector<double>(k)), d(k, vector<double>(k));
    Matrix e(k, vector<double>(k)), f(k, vector<double>(k)), g(k, vector<double>(k)), h(k, vector<double>(k));

    dividir(A, a, b, c, d, n); 
    dividir(B, e, f, g, h, n);

    // Conquistar
    Matrix P1(k, vector<double>(k)), P2(k, vector<double>(k)), P3(k, vector<double>(k)), P4(k, vector<double>(k)),
           P5(k, vector<double>(k)), P6(k, vector<double>(k)), P7(k, vector<double>(k));
    
    Matrix tA(k, vector<double>(k)), tB(k, vector<double>(k));

    // P1 = a * (f - h)
    restar(f, h, tB, k);
    strassen(a, tB, P1, k);

    // P2 = (a + b) * h
    sumar(a, b, tA, k);
    strassen(tA, h, P2, k);

    // P3 = (c + d) * e
    sumar(c, d, tA, k);
    strassen(tA, e, P3, k);

    // P4 = d * (g - e)
    restar(g, e, tB, k);
    strassen(d, tB, P4, k);

    // P5 = (a + d) * (e + h)
    sumar(a, d, tA, k);
    sumar(e, h, tB, k);
    strassen(tA, tB, P5, k);

    // P6 = (b - d) * (g + h)
    restar(b, d, tA, k);
    sumar(g, h, tB, k);
    strassen(tA, tB, P6, k);

    // P7 = (a - c) * (e + f)
    restar(a, c, tA, k);
    sumar(e, f, tB, k);
    strassen(tA, tB, P7, k);

    //Combinar
    Matrix r(k, vector<double>(k)), s(k, vector<double>(k)), t(k, vector<double>(k)), u(k, vector<double>(k));
    Matrix aux1(k, vector<double>(k)), aux2(k, vector<double>(k));

    // r = P5 + P4 - P2 + P6
    sumar(P5, P4, aux1, k);
    restar(aux1, P2, aux2, k);
    sumar(aux2, P6, r, k);

    // s = P1 + P2
    sumar(P1, P2, s, k);

    // t = P3 + P4
    sumar(P3, P4, t, k);

    // u = P5 + P1 - P3 - P7
    sumar(P5, P1, aux1, k);
    restar(aux1, P3, aux2, k);
    restar(aux2, P7, u, k);

    unir(r, s, t, u, C, n);
}

int main() {
    srand(time(NULL));

    ofstream archivo("data/resultados.dat");
    if (!archivo.is_open()) return 1;
    
    archivo << "# n\tEstandar(us)\tStrassen(us)" << endl; 

    // Solo potencias de 2 para Strassen 
    int nValores[] = {16, 32, 64, 128, 256, 512, 1024};

    for (int n : nValores) {
        Matrix A(n, vector<double>(n)), B(n, vector<double>(n));
        Matrix C_std(n, vector<double>(n, 0.0)), C_str(n, vector<double>(n, 0.0));

        llenarMatriz(A, n);
        llenarMatriz(B, n);

        int repeticiones = (n < 128) ? 20 : 5; 
        
        // Medir Estándar
        long long sumaStd = 0;
        for (int r = 0; r < repeticiones; r++) {
            auto inicio = high_resolution_clock::now();
            multiplicacionEstandar(A, B, C_std, n);
            auto fin = high_resolution_clock::now();
            sumaStd += duration_cast<microseconds>(fin - inicio).count();
        }

        // Medir Strassen
        long long sumaStr = 0;
        for (int r = 0; r < repeticiones; r++) {
            auto inicio = high_resolution_clock::now();
            strassen(A, B, C_str, n);
            auto fin = high_resolution_clock::now();
            sumaStr += duration_cast<microseconds>(fin - inicio).count();
        }

        archivo << n << "\t" << (double)sumaStd/repeticiones << "\t" << (double)sumaStr/repeticiones << endl;
        cout << "n=" << n << " procesado." << endl;
    }
    archivo.close();
    return 0;
}
