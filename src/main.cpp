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

void multiplicacionEstandar(const Matrix &A, int rA, int cA,
                            const Matrix &B, int rB, int cB,
                            Matrix &C, int rC, int cC, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double suma = 0;
            for (int k = 0; k < n; k++) {
                suma += A[rA + i][cA + k] * B[rB + k][cB + j];
            }
            C[rC + i][cC + j] = suma;
        }
    }
}
void sumar(const Matrix& A, int rA, int cA,
           const Matrix& B, int rB, int cB,
           Matrix& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[rA + i][cA + j] + B[rB + i][cB + j];
        }
    }
}

void restar(const Matrix& A, int rA, int cA,
            const Matrix& B, int rB, int cB,
            Matrix& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = A[rA + i][cA + j] - B[rB + i][cB + j];
        }
    }
}


void strassen(const Matrix &A, int rA, int cA,
              const Matrix &B, int rB, int cB,
              Matrix &C, int rC, int cC, int n) {
    
    // Caso base:  umbral de 32
    if (n <= 32) {
        multiplicacionEstandar(A, rA, cA, B, rB, cB, C, rC, cC, n);
        return;
    }

    int k = n / 2;

    Matrix P1(k, vector<double>(k)), P2(k, vector<double>(k)), P3(k, vector<double>(k)),
           P4(k, vector<double>(k)), P5(k, vector<double>(k)), P6(k, vector<double>(k)),
           P7(k, vector<double>(k));
    
    Matrix tA(k, vector<double>(k)), tB(k, vector<double>(k));

    // P1 = A11 * (B12 - B22)
    restar(B, rB, cB + k, B, rB + k, cB + k, tB, k);
    strassen(A, rA, cA, tB, 0, 0, P1, 0, 0, k);

    // P2 = (A11 + A12) * B22
    sumar(A, rA, cA, A, rA, cA + k, tA, k);
    strassen(tA, 0, 0, B, rB + k, cB + k, P2, 0, 0, k);

    // P3 = (A21 + A22) * B11
    sumar(A, rA + k, cA, A, rA + k, cA + k, tA, k);
    strassen(tA, 0, 0, B, rB, cB, P3, 0, 0, k);

    // P4 = A22 * (B21 - B11)
    restar(B, rB + k, cB, B, rB, cB, tB, k);
    strassen(A, rA + k, cA + k, tB, 0, 0, P4, 0, 0, k);

    // P5 = (A11 + A22) * (B11 + B22)
    sumar(A, rA, cA, A, rA + k, cA + k, tA, k);
    sumar(B, rB, cB, B, rB + k, cB + k, tB, k);
    strassen(tA, 0, 0, tB, 0, 0, P5, 0, 0, k);

    // P6 = (A12 - A22) * (B21 + B22)
    restar(A, rA, cA + k, A, rA + k, cA + k, tA, k);
    sumar(B, rB + k, cB, B, rB + k, cB + k, tB, k);
    strassen(tA, 0, 0, tB, 0, 0, P6, 0, 0, k);

    // P7 = (A11 - A21) * (B11 + B12)
    restar(A, rA, cA, A, rA + k, cA, tA, k);
    sumar(B, rB, cB, B, rB, cB + k, tB, k);
    strassen(tA, 0, 0, tB, 0, 0, P7, 0, 0, k);

    // Combinar resultados directamente en C
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            C[rC + i][cC + j] = P5[i][j] + P4[i][j] - P2[i][j] + P6[i][j];         // C11
            C[rC + i][cC + j + k] = P1[i][j] + P2[i][j];                         // C12
            C[rC + k + i][cC + j] = P3[i][j] + P4[i][j];                         // C21
            C[rC + k + i][cC + j + k] = P5[i][j] + P1[i][j] - P3[i][j] - P7[i][j]; // C22
        }
    }
}



int main() {
    srand(time(NULL));

    ofstream archivo("data/resultados.dat");
    if (!archivo.is_open()) return 1;
    
    archivo << "# n\tEstandar(us)\tStrassen(us)" << endl; 

    int nValores[] = {16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 256, 512, 1024};

    for (int n : nValores) {
        Matrix A(n, vector<double>(n)), B(n, vector<double>(n));
        Matrix C_std(n, vector<double>(n, 0.0)), C_str(n, vector<double>(n, 0.0));

        llenarMatriz(A, n);
        llenarMatriz(B, n);

        int repeticiones = (n <= 64) ? 100 : (n <= 256 ? 20 : 5); 
        
        // Medir Estándar
        long long sumaStd = 0;
        for (int r = 0; r < repeticiones; r++) {
            auto inicio = high_resolution_clock::now();
            multiplicacionEstandar(A, 0, 0, B, 0, 0, C_std, 0, 0, n);
            auto fin = high_resolution_clock::now();
            sumaStd += duration_cast<microseconds>(fin - inicio).count();
        }

        // Medir Strassen
        long long sumaStr = 0;
        for (int r = 0; r < repeticiones; r++) {
            auto inicio = high_resolution_clock::now();
            strassen(A, 0, 0, B, 0, 0, C_str, 0, 0, n);
            auto fin = high_resolution_clock::now();
            sumaStr += duration_cast<microseconds>(fin - inicio).count();
        }

        archivo << n << "\t" << (double)sumaStd/repeticiones << "\t" << (double)sumaStr/repeticiones << endl;
        cout << "n=" << n << " procesado." << endl;
    }
    archivo.close();
    return 0;
}
