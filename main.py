import numpy as np


def read_matrix(filename, n):
    """Чтение матрицы из файла (числа через пробел, строки через endl)"""
    with open(filename, 'r') as f:
        matrix = []
        for line in f:
            row = list(map(float, line.strip().split()))
            matrix.append(row)
    return np.array(matrix)
  
