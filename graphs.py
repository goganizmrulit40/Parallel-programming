import matplotlib.pyplot as plt
import numpy as np

# Данные
sizes = [200, 400, 800, 1200, 1600, 2000] # размеры матриц
times = [0.167, 1.599, 14.177, 49.020, 118.129, 281.599]  # среднее время
operations = [16e6, 128e6, 1024e6, 3456e6, 8192e6, 16000e6]  # объём задачи

# График 1: Время выполнения от размера матрицы
plt.figure(figsize=(8, 5))
plt.plot(sizes, times, 'bo-', linewidth=2, markersize=6)
plt.xlabel('Размер матрицы')
plt.ylabel('Время (сек)')
plt.title('Зависимость времени выполнения от размера матрицы')
plt.grid(True)
plt.savefig('time_vs_size.png', dpi=150, bbox_inches='tight')
plt.show()

# График 2: Время выполнения от объёма задачи
plt.figure(figsize=(8, 5))
plt.plot(operations, times, 'ro-', linewidth=2, markersize=6)
plt.xlabel('Объём задачи (операций)')
plt.ylabel('Время (сек)')
plt.title('Зависимость времени выполнения от объёма задачи')
plt.grid(True)
plt.savefig('time_vs_operations.png', dpi=150, bbox_inches='tight')
plt.show()

# График 3: Объём задачи от размера матрицы
plt.figure(figsize=(8, 5))
plt.plot(sizes, operations, 'go-', linewidth=2, markersize=6)
plt.xlabel('Размер матрицы')
plt.ylabel('Объём задачи (операций)')
plt.title('Зависимость объёма задачи от размера матрицы')
plt.grid(True)
plt.savefig('operations_vs_size.png', dpi=150, bbox_inches='tight')
plt.show()
