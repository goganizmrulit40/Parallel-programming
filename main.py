import numpy as np


def read_matrix(filename, n):
    """Чтение матрицы из файла (числа через пробел, строки через endl)"""
    with open(filename, 'r') as f:
        matrix = []
        for line in f:
            row = list(map(float, line.strip().split()))
            matrix.append(row)
    return np.array(matrix)
  

def verify_size(n):
    """Проверка для одного размера"""
    print(f"\n--- Размер {n} ---")

    try:
        A = read_matrix(f'A_{n}.txt', n)
        B = read_matrix(f'B_{n}.txt', n)
        C_prog = read_matrix(f'C_{n}.txt', n)

        C_true = np.dot(A, B)

        diff = np.max(np.abs(C_prog - C_true))
        print(f"  Максимальная разница: {diff:.2e}")

        if diff < 1e-8:
            print(" ВЕРНО")
            return True
        else:
            print(" НЕВЕРНО")
            return False

    except Exception as e:
        print(f"  ОШИБКА: {e}")
        return False
