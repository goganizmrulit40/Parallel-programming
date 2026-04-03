#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <mpi.h>

using namespace std;
using namespace chrono;

// Функция для умножения матриц - последовательное умножение
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

// Параллельное умножение с MPI (оптимизированный алгоритм i-k-j)
double multiply_mpi(const vector<vector<double>>& A,
    const vector<vector<double>>& B,
    int size, int rank, int n) {

    vector<double> B_flat(n * n, 0.0);

    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                B_flat[i * n + j] = B[i][j];
            }
        }
    }

    MPI_Bcast(B_flat.data(), n * n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    vector<vector<double>> B_mat(n, vector<double>(n));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            B_mat[i][j] = B_flat[i * n + j];
        }
    }

    int rows_per_proc = n / size;
    int remainder = n % size;
    int local_rows = rows_per_proc + (rank < remainder ? 1 : 0);

    vector<vector<double>> A_local(local_rows, vector<double>(n));

    if (rank == 0) {
        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < n; j++) {
                A_local[i][j] = A[i][j];
            }
        }

        int offset = local_rows;
        for (int p = 1; p < size; p++) {
            int p_rows = rows_per_proc + (p < remainder ? 1 : 0);
            vector<double> flat_rows(p_rows * n);
            for (int i = 0; i < p_rows; i++) {
                for (int j = 0; j < n; j++) {
                    flat_rows[i * n + j] = A[offset + i][j];
                }
            }
            MPI_Send(flat_rows.data(), p_rows * n, MPI_DOUBLE, p, 0, MPI_COMM_WORLD);
            offset += p_rows;
        }
    }
    else {
        vector<double> flat_rows(local_rows * n);
        MPI_Recv(flat_rows.data(), local_rows * n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < n; j++) {
                A_local[i][j] = flat_rows[i * n + j];
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double start_time = MPI_Wtime();

    vector<vector<double>> C_local(local_rows, vector<double>(n, 0.0));
    for (int i = 0; i < local_rows; i++) {
        for (int k = 0; k < n; k++) {
            double aik = A_local[i][k];
            for (int j = 0; j < n; j++) {
                C_local[i][j] += aik * B_mat[k][j];
            }
        }
    }

    if (rank == 0) {
        vector<vector<double>> C(n, vector<double>(n, 0.0));

        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < n; j++) {
                C[i][j] = C_local[i][j];
            }
        }

        int offset = local_rows;
        for (int p = 1; p < size; p++) {
            int p_rows = rows_per_proc + (p < remainder ? 1 : 0);
            vector<double> flat_rows(p_rows * n);
            MPI_Recv(flat_rows.data(), p_rows * n, MPI_DOUBLE, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int i = 0; i < p_rows; i++) {
                for (int j = 0; j < n; j++) {
                    C[offset + i][j] = flat_rows[i * n + j];
                }
            }
            offset += p_rows;
        }

        double end_time = MPI_Wtime();
        return end_time - start_time;
    }
    else {
        vector<double> flat_rows(local_rows * n);
        for (int i = 0; i < local_rows; i++) {
            for (int j = 0; j < n; j++) {
                flat_rows[i * n + j] = C_local[i][j];
            }
        }
        MPI_Send(flat_rows.data(), local_rows * n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
        return 0.0;
    }
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
