#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <omp.h>
#include <ctime>

using namespace std;
using namespace chrono;

// --------Структуры данных-----------
// 
// СД для информации о процессоре
struct SystemInfo {
    int physical_cores;
    int logical_processors;
    int max_threads;
};

// СД для хранения результатов экспериментов
struct ExperimentResult {
    int size;
    int threads;
    double time;
    long long operations;
    double speedup;
    double efficiency;
    bool correct;
};

// Функция для умножения матриц - последовательное умножение
vector<vector<double>> multiply_seq(const vector<vector<double>>& A,
    const vector<vector<double>>& B) {
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

// Функция для умножения матриц - последовательное умножение
vector<vector<double>> multiply_omp(const vector<vector<double>>& A,
    const vector<vector<double>>& B, int num_threads) {
    int n = A.size();
    vector<vector<double>> C(n, vector<double>(n, 0.0));

    omp_set_num_threads(num_threads);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        for (int k = 0; k < n; k++) {
            double aik = A[i][k];
            for (int j = 0; j < n; j++) {
                C[i][j] += aik * B[k][j];
            }
        }
    }
    return C;
}

// ----------- Вспомогательные функции --------------

// Функция для усреднения результатов
double measureTime(const vector<vector<double>>&A,
    const vector<vector<double>>&B,
    int threads,
    int repeats = 3) {
    
    double total_time = 0.0;
    
    for (int r = 0; r < repeats; r++) {
        auto start = high_resolution_clock::now();
        vector<vector<double>> C = multiply_omp(A, B, threads);
        auto end = high_resolution_clock::now();
        total_time += duration<double>(end - start).count();
    }
    
    return total_time / repeats;
    
}

// Функция проверки корректности результатов
bool checkResult(const vector<vector<double>>& C1,
    const vector<vector<double>>& C2) {
    int n = C1.size();
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

// Функция для заполнения матрицы случайными числами
void fillRandom(vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 10;
        }
    }
}

// Функция для записи матриц в файл
void writeMatrix(const vector<vector<double>>& matrix, const string& filename) {
    ofstream file(filename);
    int n = matrix.size();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file << matrix[i][j] << " ";
        }
        file << endl;
    }
    file.close();
}

// Функция для вычисления объема задачи
long long getOperationsCount(int n) {
    return 2LL * n * n * n;
}

// Функция для меток времени
string getCurrentTime() {
    time_t now = time(0);
    struct tm timeinfo;
    char buffer[80];
    localtime_s(&timeinfo, &now);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return string(buffer);
}

// Функция для информации о системе
SystemInfo getSystemInfo() {
    SystemInfo info;
    info.logical_processors = omp_get_num_procs();
    info.physical_cores = info.logical_processors / 2;
    info.max_threads = omp_get_max_threads();
    return info;
}

// ----------- Функции для работы с файлами ---------------

// Функция для инициализации файла
void initResultsFile(const string& filename) {
    ifstream test_file(filename);
    bool file_exists = test_file.good();
    test_file.close();

    ofstream results(filename, ios::app);

    if (!file_exists) {
        results << "Size\tThreads\tTime\tOperations\tSpeedup\tEfficiency\tCorrect" << endl;
    }
    else {
        results << "\n";
    }

    SystemInfo info = getSystemInfo();
    results << "# " << getCurrentTime() << " | Ядра: " << info.physical_cores
        << " | Потоки: " << info.logical_processors << endl;
    results.close();
}

// Функция для сохранения файла
void saveResult(const string& filename, const ExperimentResult& res) {
    ofstream results(filename, ios::app);
    results << res.size << "\t" << res.threads << "\t"
        << res.time << "\t" << res.operations << "\t"
        << res.speedup << "\t" << res.efficiency << "\t"
        << (res.correct ? "OK" : "FAIL") << endl;
    results.close();
}

int main() {
    setlocale(LC_ALL, "rus");

    int sizes[] = { 200, 400, 800, 1200, 1600, 2000 };
    int num_sizes = 6;

    ofstream results("results.txt", ios::app);

    results << "\n";

    for (int s = 0; s < num_sizes; s++) {
        int n = sizes[s];
        cout << "Размер матрицы: " << n << endl;

        vector<vector<double>> A(n, vector<double>(n));
        vector<vector<double>> B(n, vector<double>(n));

        fillRandom(A);
        fillRandom(B);

        writeMatrix(A, "A_" + to_string(n) + ".txt");
        writeMatrix(B, "B_" + to_string(n) + ".txt");

        auto start = high_resolution_clock::now();

        vector<vector<double>> C = multiply_seq(A, B);

        auto end = high_resolution_clock::now();

        double time = duration<double>(end - start).count();

        string filenameC = "C_" + to_string(n) + ".txt";
        writeMatrix(C, filenameC);

        long long operations = getOperationsCount(n);

        results << n << "\t" << fixed << setprecision(3) << time << "\t" << operations << endl;
    }

    results.close();
}
