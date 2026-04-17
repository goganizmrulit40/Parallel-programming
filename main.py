import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import numpy as np

# Читаем данные из файла
sizes = [200, 400, 800, 1200, 1600, 2000]
configs = ['16x16', '32x32', '16x32', '32x16']

# Хранилище
data = {cfg: {size: [] for size in sizes} for cfg in configs}

with open('results_cuda.txt', 'r') as f:
    for line in f:
        parts = line.strip().split()
        if len(parts) >= 6 and parts[0].isdigit():
            size = int(parts[0])
            cfg = parts[1]
            speedup = float(parts[4])  # Ускорение из файла
            data[cfg][size].append(speedup)

# Усредняем
avg_speedup = {}
avg_efficiency = {}
threads = {'16x16': 256, '32x32': 1024, '16x32': 512, '32x16': 512}

for cfg in configs:
    avg_speedup[cfg] = []
    avg_efficiency[cfg] = []
    for size in sizes:
        vals = data[cfg][size]
        avg_sp = sum(vals) / len(vals)
        avg_speedup[cfg].append(avg_sp)
        avg_efficiency[cfg].append(avg_sp / threads[cfg] * 100)

# График ускорения
plt.figure(figsize=(12, 8))
colors = {'16x16': 'blue', '32x32': 'red', '16x32': 'green', '32x16': 'orange'}
for cfg in configs:
    plt.plot(sizes, avg_speedup[cfg], 'o-', color=colors[cfg], linewidth=2, markersize=8, label=cfg)

plt.xlabel('Размер матрицы', fontsize=12)
plt.ylabel('Ускорение', fontsize=12)
plt.title('Ускорение относительно последовательной версии (CPU)', fontsize=14)
plt.legend()
plt.grid(True, alpha=0.3)
plt.savefig('cuda_speedup_vs_size.png', dpi=150, bbox_inches='tight')
plt.show()

# График эффективности
plt.figure(figsize=(12, 8))
for cfg in configs:
    plt.plot(sizes, avg_efficiency[cfg], 'o-', color=colors[cfg], linewidth=2, markersize=8,
             label=f'{cfg} ({threads[cfg]} потоков)')

plt.xlabel('Размер матрицы', fontsize=12)
plt.ylabel('Эффективность (%)', fontsize=12)
plt.title('Эффективность использования GPU', fontsize=14)
plt.legend()
plt.grid(True, alpha=0.3)
plt.axhline(y=100, color='gray', linestyle='--', alpha=0.5)
plt.savefig('cuda_efficiency_vs_size.png', dpi=150, bbox_inches='tight')
plt.show()

# График для 2000
idx = 5  # 2000
times_2000 = []
speedups_2000 = [avg_speedup[cfg][idx] for cfg in configs]

plt.figure(figsize=(10, 6))
bars = plt.bar(configs, speedups_2000, color=[colors[c] for c in configs])
plt.xlabel('Конфигурация', fontsize=12)
plt.ylabel('Ускорение', fontsize=12)
plt.title('Ускорение для матрицы 2000×2000', fontsize=14)
for bar, val in zip(bars, speedups_2000):
    plt.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 10, f'{val:.0f}x', ha='center', fontsize=10)
plt.savefig('cuda_comparison_2000.png', dpi=150, bbox_inches='tight')
plt.show()