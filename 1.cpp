#include <iostream>
#include <vector>

using namespace std;

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

int main(){
    
}
