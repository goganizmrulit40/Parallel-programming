#include <iostream>
#include <vector>

using namespace std;

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
    }
}
