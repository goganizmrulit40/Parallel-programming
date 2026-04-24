import matplotlib.pyplot as plt
import numpy as np

sizes = np.array([200, 400, 800, 1200, 1600, 2000])
time_seq = np.array([0.028, 0.228, 3.929, 10.392, 28.502, 50.103])
time_par = np.array([0.001, 0.011, 0.075, 0.297, 0.727, 1.400])
speedup = np.array([19.33, 20.82, 52.54, 34.95, 39.23, 35.79])
efficiency = np.array([241.6, 260.3, 656.8, 436.9, 490.4, 447.3])
operations = np.array([16e6, 128e6, 1024e6, 3456e6, 8192e6, 16000e6])

max_speedup = 8
max_efficiency = 100

#---------------------------------------
fig1, ax1 = plt.subplots(figsize=(10, 6))

ax1.bar(sizes.astype(str), speedup, color='green', alpha=0.7, edgecolor='darkgreen', linewidth=1.5, label='Экспериментальное ускорение')
ax1.axhline(y=max_speedup, color='red', linestyle='--', linewidth=2, label=f'Теоретический максимум ({max_speedup}×)')

ax1.set_xlabel('Размер матрицы N', fontsize=12)
ax1.set_ylabel('Ускорение (×)', fontsize=12)
ax1.set_title('График ускорения параллельного алгоритма относительно последовательного', fontsize=14)
ax1.legend(loc='upper left')
ax1.grid(True, axis='y', alpha=0.3)

for i, (n, s) in enumerate(zip(sizes, speedup)):
    ax1.text(i, s + 2, f'{s:.2f}×', ha='center', fontsize=9, fontweight='bold')

plt.tight_layout()
plt.savefig('graph_speedup.png', dpi=300, bbox_inches='tight')
plt.show()

#---------------------------------------
fig2, ax2 = plt.subplots(figsize=(10, 6))

colors = ['red' if e > max_efficiency else 'blue' for e in efficiency]
ax2.bar(sizes.astype(str), efficiency, color=colors, alpha=0.7, edgecolor='black', linewidth=1.5)
ax2.axhline(y=max_efficiency, color='green', linestyle='--', linewidth=2, label=f'Теоретический максимум ({max_efficiency}%)')

ax2.set_xlabel('Размер матрицы N', fontsize=12)
ax2.set_ylabel('Эффективность (%)', fontsize=12)
ax2.set_title('График эффективности параллелизации', fontsize=14)
ax2.legend(loc='upper left')
ax2.grid(True, axis='y', alpha=0.3)

plt.tight_layout()
plt.savefig('graph_efficiency.png', dpi=300, bbox_inches='tight')
plt.show()

#---------------------------------------
fig3, ax3 = plt.subplots(figsize=(10, 6))

ax3.plot(sizes, operations / 1e9, 'D-', color='purple', linewidth=2, markersize=8, label='Объём задачи (млрд операций)')
ax3.set_xlabel('Размер матрицы N', fontsize=12)
ax3.set_ylabel('Объём задачи (млрд операций)', fontsize=12)
ax3.set_title('График зависимости объёма вычислений от размера матрицы', fontsize=14)
ax3.grid(True, alpha=0.3)
ax3.legend(loc='upper left')

x_fit = np.linspace(200, 2000, 100)
y_fit = (2 * x_fit**3) / 1e9
ax3.plot(x_fit, y_fit, '--', color='gray', alpha=0.7, label='Теоретическая кубическая зависимость')

ax3.legend(loc='upper left')
plt.tight_layout()
plt.savefig('graph_operations.png', dpi=300, bbox_inches='tight')
plt.show()
