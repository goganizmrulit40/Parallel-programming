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
vector<vector<double>> multiply_seq(const vector<vector<double>>& A,
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

    int repeats = 3; // количество замеров

    string filename = "results_mpi.txt";

    if (rank == 0) {
        ifstream check(filename);
        bool file_exists = check.good();
        check.close();
    
        // Открываем в режиме добавления
        ofstream file(filename, ios::app);
    
        if (!file_exists) {
            // Если файл новый — пишем заголовок
            file << "=== Запуск с " << world_size << " процессами ===\n";
            file << "Размер\tПроцессов\tВремя(сек)\tОбъем задачи\tУскорение\tЭффективность(%)\n";
        }
        else {
            // Если файл уже есть — добавляем разделитель и информацию о новом запуске
            file << "\n=== Запуск с " << world_size << " процессами ===\n";
        }
        file.close();

    }
    
    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];

        if (rank == 0) {
            cout << "\n========== РАЗМЕР МАТРИЦЫ: " << n << " ==========" << endl;
        }

        vector<vector<double>> A, B;
        double seq_time = 0.0;
        
        if (rank == 0) {
            A.resize(n, vector<double>(n));
            B.resize(n, vector<double>(n));
            fillRandom(A);
            fillRandom(B);
        
            // Последовательное время (классический алгоритм)
            cout << "  [Последовательный алгоритм]" << endl;
            double total_seq = 0.0;
            for (int r = 0; r < repeats; r++) {
                auto start = high_resolution_clock::now();
                multiply_seq(A, B);
                auto end = high_resolution_clock::now();
                total_seq += duration<double>(end - start).count();
            }
            seq_time = total_seq / repeats;
            cout << "    Время: " << fixed << setprecision(3) << seq_time << " сек" << endl;
        }
        
        MPI_Bcast(&seq_time, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        // Параллельное умножение с усреднением
        double total_mpi = 0.0;
        for (int r = 0; r < repeats; r++) {
            total_mpi += multiply_mpi(A, B, world_size, rank, n);
        }
        double mpi_time = total_mpi / repeats;
        
        if (rank == 0) {
            double speedup = seq_time / mpi_time;
            double efficiency = speedup / world_size * 100;
            long long operations = getOperationsCount(n);
            
            cout << "  [Параллельный алгоритм]" << endl;
            cout << "    Время: " << fixed << setprecision(3) << mpi_time << " сек" << endl;
            cout << "    Ускорение: " << fixed << setprecision(2) << speedup << "x" << endl;
            cout << "    Эффективность: " << fixed << setprecision(1) << efficiency << "%" << endl;
            cout << "    Объём задачи: " << operations << " операций" << endl;

            // Записываем в файл (добавляем строку)
            ofstream file(filename, ios::app);
            if (file.is_open()) {
                file << n << "\t" << world_size << "\t"
                    << fixed << setprecision(3) << mpi_time << "\t"
                    << operations << "\t"
                    << fixed << setprecision(2) << speedup << "\t"
                    << fixed << setprecision(1) << efficiency << "\n";
                file.close();
            }
        }

        MPI_Barrier(MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}
