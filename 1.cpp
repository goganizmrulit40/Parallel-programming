#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
using namespace chrono;

// Функция для умножения матриц
vector<vector<double>> multiply(const vector<vector<double>>& A,
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

// Функция для заполнения матрицы случайными числами
void fillRandom(vector<vector<double>>& matrix) {
    int n = matrix.size();
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            matrix[i][j] = rand() % 100 / 10.0;
        }
    }
}

// Функция для записи матрицы в файл
void writeMatrix(const vector<vector<double>>& matrix, const string& filename) {
    ofstream file(filename);
    int n = matrix.size();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            file << matrix[i][j];
        }
        file << "; ";
    }
    file.close();
}


int main() {
    int sizes[] = { 200, 400, 800, 1200, 1600, 2000 };
    int num_sizes = 6;
    
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

        vector<vector<double>> C = multiply(A, B);
        
        auto end = high_resolution_clock::now();
        
        double time = duration<double>(end - start).count();
        
        cout << time << endl;
    }
}


