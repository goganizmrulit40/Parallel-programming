#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <mpi.h>

using namespace std;
using namespace chrono;

// Функция для умножения матриц
vector<vector<double>> multiply(const vector<vector<double>>& A,
    const vector<vector<double>>& B){
    int n = A.size();
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

// Функция для заполнения матрицы случайными числами
void fillRandom(vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

// Функция для вычисления объема задачи
long long getOperationsCount(int n) {
    return 2LL * n * n * n;
}

// Параллельное умножение - заглушка
double multiply_mpi(const vector<vector<double>>& A,
    const vector<vector<double>>& B,
    int size, int rank, int n) {
    
    double time = 0.0;
    if (rank == 0) {
        auto start = high_resolution_clock::now();
        multiply_seq(A, B);
        auto end = high_resolution_clock::now();
        time = duration<double>(end - start).count();
    }
    return time;
}


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "rus");
    srand(time(NULL));

    MPI_Init(&argc, &argv);

    int rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int sizes[] = { 200, 400, 800, 1200, 1600, 2000 };
    int num_sizes = 6;

    if (rank == 0) {
        cout << "Количество процессов: " << world_size << endl;
    }

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];

        if (rank == 0) {
            cout << "\n========== РАЗМЕР МАТРИЦЫ: " << n << " ==========" << endl;
        }

        vector<vector<double>> A, B;

        if (rank == 0) {
            A.resize(n, vector<double>(n));
            B.resize(n, vector<double>(n));
            fillRandom(A);
            fillRandom(B);
        }

        double mpi_time = multiply_mpi(A, B, world_size, rank, n);

        if (rank == 0) {
            cout << "  Время: " << fixed << setprecision(3) << mpi_time << " сек" << endl;
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
