import matplotlib.pyplot as plt
import numpy as np

# Данные из results_mpi.txt
sizes = [200, 400, 800, 1200, 1600, 2000]
processes = [1, 2, 4, 6, 8, 12]

# Время выполнения (сек) для каждого количества процессов
times = {
    1:  [0.007, 0.049, 0.418, 1.530, 3.683, 7.893],
    2:  [0.004, 0.029, 0.449, 0.864, 3.276, 6.676],
    4:  [0.002, 0.023, 0.339, 0.847, 3.246, 4.435],
    6:  [0.002, 0.013, 0.293, 1.013, 2.434, 3.132],
    8:  [0.002, 0.015, 0.180, 0.666, 1.536, 3.034],
    12: [0.002, 0.012, 0.205, 0.633, 1.556, 2.989]
}

# Ускорение для каждого количества процессов
speedups = {
    1:  [1.08, 1.18, 1.26, 1.64, 2.03, 3.11],
    2:  [2.06, 2.16, 2.51, 3.78, 2.67, 4.56],
    4:  [3.85, 3.15, 2.50, 4.98, 3.46, 7.32],
    6:  [3.04, 5.64, 2.32, 3.64, 4.25, 8.67],
    8:  [3.98, 4.11, 3.14, 4.14, 5.04, 9.31],
    12: [4.78, 5.13, 2.89, 4.08, 4.91, 8.43]
}

# Эффективность (%) для каждого количества процессов
efficiencies = {
    1:  [108.1, 117.8, 126.3, 164.2, 202.6, 311.0],
    2:  [102.8, 107.9, 125.7, 189.1, 133.3, 228.2],
    4:  [96.1, 78.8, 62.5, 124.6, 86.4, 182.9],
    6:  [50.6, 93.9, 38.7, 60.7, 70.8, 144.4],
    8:  [49.7, 51.4, 39.3, 51.7, 63.0, 116.4],
    12: [39.8, 42.8, 24.0, 34.0, 40.9, 70.2]
}

# Время выполнения от размера матрицы
plt.figure(figsize=(12, 8))
colors = ['blue', 'green', 'red', 'orange', 'purple', 'brown']

for i, p in enumerate(processes):
    plt.plot(sizes, times[p], 'o-', color=colors[i], linewidth=2,
             markersize=8, label=f'{p} процессов')

plt.xlabel('Размер матрицы', fontsize=12)
plt.ylabel('Время выполнения (сек)', fontsize=12)
plt.title('Зависимость времени выполнения от размера матрицы', fontsize=14)
plt.legend()
plt.grid(True, alpha=0.3)
plt.savefig('time_vs_size.png', dpi=150, bbox_inches='tight')
plt.show()

# Эффективность от количества процессов
plt.figure(figsize=(12, 8))

for i, size in enumerate(sizes):
    eff_at_size = [efficiencies[p][i] for p in processes]
    plt.plot(processes, eff_at_size, 'o-', linewidth=2,
             markersize=8, label=f'Размер {size}')

plt.xlabel('Количество процессов', fontsize=12)
plt.ylabel('Эффективность (%)', fontsize=12)
plt.title('Эффективность распараллеливания', fontsize=14)
plt.legend()
plt.grid(True, alpha=0.3)
plt.xticks(processes)
plt.ylim(0, 350)
plt.savefig('efficiency_vs_processes.png', dpi=150, bbox_inches='tight')
plt.show()

# Эффективность от размера матрицы
plt.figure(figsize=(12, 8))

for i, p in enumerate(processes):
    plt.plot(sizes, efficiencies[p], 'o-', color=colors[i], linewidth=2,
             markersize=8, label=f'{p} процессов')

plt.xlabel('Размер матрицы', fontsize=12)
plt.ylabel('Эффективность (%)', fontsize=12)
plt.title('Эффективность от размера матрицы', fontsize=14)
plt.legend()
plt.grid(True, alpha=0.3)
plt.savefig('efficiency_vs_size.png', dpi=150, bbox_inches='tight')
plt.show()
