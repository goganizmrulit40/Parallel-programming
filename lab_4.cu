#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cuda_runtime.h>

using namespace std;
using namespace chrono;

vector<vector<double>> multiply_seq(const vector<vector<double>>& A,
    const vector<vector<double>>& B) {
    int n = (int)A.size();
    vector<vector<double>> C(n, vector<double>(n, 0.0));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    return C;
}

void fillRandom(vector<vector<double>>& matrix) {
    int n = (int)matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

long long getOperationsCount(int n) {
    return 2LL * n * n * n;
}

int main() {
    setlocale(LC_ALL, "rus");
    srand((unsigned int)time(NULL));

    // Информация о GPU
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    
    cout << "=========================================================" << endl;
    cout << "ПАРАЛЛЕЛЬНОЕ УМНОЖЕНИЕ МАТРИЦ (CUDA)" << endl;
    cout << "=========================================================" << endl;
    
    for (int device = 0; device < deviceCount; device++) {
        cudaDeviceProp prop;
        cudaGetDeviceProperties(&prop, device);
        cout << "GPU: " << prop.name << endl;
    }
    cout << "=========================================================" << endl;


    int sizes[] = { 200, 400, 800, 1200, 1600, 2000 };
    int num_sizes = 6;

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        cout << "\n========== РАЗМЕР МАТРИЦЫ: " << n << " ==========" << endl;

        vector<vector<double>> A(n, vector<double>(n));
        vector<vector<double>> B(n, vector<double>(n));
        fillRandom(A);
        fillRandom(B);

        auto start_seq = high_resolution_clock::now();
        vector<vector<double>> C_seq = multiply_seq(A, B);
        auto end_seq = high_resolution_clock::now();
        double seq_time = duration<double>(end_seq - start_seq).count();
        cout << "  Время: " << fixed << setprecision(3) << seq_time << " сек" << endl;
    }

    return 0;
}
