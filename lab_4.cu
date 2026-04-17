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

// CUDA ЯДРО
__global__ void multiply_cuda_kernel(const double* A, const double* B, double* C, int n) {
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < n && col < n) {
        double sum = 0.0;
        for (int k = 0; k < n; k++) {
            sum += A[row * n + k] * B[k * n + col];
        }
        C[row * n + col] = sum;
    }
}

// ФУНКЦИИ ДЛЯ РАБОТЫ С МАТРИЦАМИ
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

double* flattenMatrix(const vector<vector<double>>& matrix) {
    int n = (int)matrix.size();
    double* flat = new double[n * n];
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            flat[i * n + j] = matrix[i][j];
        }
    }
    return flat;
}

bool compareResults(const vector<vector<double>>& C1,
    const vector<vector<double>>& C2) {
    int n = (int)C1.size();
    double eps = 1e-8;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (abs(C1[i][j] - C2[i][j]) > eps) {
                return false;
            }
        }
    }
    return true;
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

    // Размеры матриц
    int sizes[] = { 200, 400, 800, 1200, 1600, 2000 };
    int num_sizes = 6;

    // Конфигурации блоков
    struct BlockConfig {
        int threads_x;
        int threads_y;
        string name;
    };
    
    BlockConfig block_configs[] = {
        {16, 16, "16x16"},
        {32, 32, "32x32"},
        {16, 32, "16x32"},
        {32, 16, "32x16"}
    };
    int num_configs = 4;

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        cout << "\n========== РАЗМЕР МАТРИЦЫ: " << n << " ==========" << endl;

        // Создание матриц
        vector<vector<double>> A(n, vector<double>(n));
        vector<vector<double>> B(n, vector<double>(n));
        fillRandom(A);
        fillRandom(B);

        // Последовательное время
        cout << "  [Последовательный алгоритм]" << endl;
        auto start_seq = high_resolution_clock::now();
        vector<vector<double>> C_seq = multiply_seq(A, B);
        auto end_seq = high_resolution_clock::now();
        double seq_time = duration<double>(end_seq - start_seq).count();
        cout << "  Время: " << fixed << setprecision(3) << seq_time << " сек" << endl;

        // Подготовка данных для CUDA
        double* h_A = flattenMatrix(A);
        double* h_B = flattenMatrix(B);
        double* h_C = new double[n * n];

        double* d_A, * d_B, * d_C;
        cudaMalloc(&d_A, n * n * sizeof(double));
        cudaMalloc(&d_B, n * n * sizeof(double));
        cudaMalloc(&d_C, n * n * sizeof(double));
        
        cudaMemcpy(d_A, h_A, n * n * sizeof(double), cudaMemcpyHostToDevice);
        cudaMemcpy(d_B, h_B, n * n * sizeof(double), cudaMemcpyHostToDevice);

        for (int cfg = 0; cfg < num_configs; cfg++) {

            dim3 threadsPerBlock(16, 16);
            dim3 numBlocks((n + 15) / 16, (n + 15) / 16);

            // Прогрев
            multiply_cuda_kernel<<<numBlocks, threadsPerBlock>>>(d_A, d_B, d_C, n);
            cudaDeviceSynchronize();

            // Замер времени (3 замера)
            double total_time = 0.0;
            int repeats = 3;

            for (int r = 0; r < repeats; r++) {
                cudaEvent_t start, stop;
                cudaEventCreate(&start);
                cudaEventCreate(&stop);
                
                cudaEventRecord(start);
                multiply_cuda_kernel << <numBlocks, threadsPerBlock >> > (d_A, d_B, d_C, n);
                cudaEventRecord(stop);
                cudaEventSynchronize(stop);
                
                float milliseconds;
                cudaEventElapsedTime(&milliseconds, start, stop);
                total_time += milliseconds / 1000.0;
                
                cudaEventDestroy(start);
                cudaEventDestroy(stop);
            }
    
            double avg_time = total_time / repeats;
            double speedup = seq_time / avg_time;
            double efficiency = speedup / (threadsPerBlock.x * threadsPerBlock.y) * 100;
            
            cout << "  " << setw(10) << block_configs[cfg].name << " | "
                << fixed << setprecision(3) << setw(8) << avg_time << "   | "
                << setw(6) << fixed << setprecision(2) << speedup << "x   | "
                << setw(6) << fixed << setprecision(1) << efficiency << "%" << endl;

            // Запись результатов в файл
            ofstream file("results_cuda.txt", ios::app);
            file << n << "\t" << block_configs[cfg].name << "\t"
                << fixed << setprecision(3) << avg_time << "\t"
                << operations << "\t"
                << fixed << setprecision(2) << speedup << "\t"
                << fixed << setprecision(1) << efficiency << "\n";
            file.close();
        }
                    
         cudaMemcpy(h_C, d_C, n * n * sizeof(double), cudaMemcpyDeviceToHost);
         
         // Проверка корректности
         vector<vector<double>> C_cuda(n, vector<double>(n));
         for (int i = 0; i < n; i++) {
             for (int j = 0; j < n; j++) {
                 C_cuda[i][j] = h_C[i * n + j];
             }
         }
         
         bool correct = compareResults(C_seq, C_cuda);
         cout << "  Проверка корректности: " << (correct ? "OK" : "ERROR") << endl;
         
         // Очистка памяти
         delete[] h_A;
         delete[] h_B;
         delete[] h_C;
         cudaFree(d_A);
         cudaFree(d_B);
         cudaFree(d_C);

    }

    return 0;
}
