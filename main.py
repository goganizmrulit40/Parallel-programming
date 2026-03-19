import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import re


def parse_results_file(filename):
    """Парсит файл результатов и возвращает DataFrame с данными"""

    # Пробуем разные кодировки
    encodings = ['utf-8', 'cp1251', 'cp866', 'latin-1']
    lines = None

    for enc in encodings:
        try:
            with open(filename, 'r', encoding=enc) as f:
                lines = f.readlines()
            print(f"Файл прочитан в кодировке: {enc}")
            break
        except UnicodeDecodeError:
            continue

    if lines is None:
        # Если ничего не помогло, читаем в бинарном режиме и заменяем ошибки
        with open(filename, 'rb') as f:
            raw_data = f.read()
        lines = raw_data.decode('utf-8', errors='ignore').split('\n')
        print("Файл прочитан с игнорированием ошибок кодировки")
    
    data = []
    current_run = None

    for line in lines:
        line = line.strip()

        # Пропускаем пустые строки
        if not line:
            continue

        # Проверяем, является ли строка заголовком запуска
        if line.startswith('#'):
            # Извлекаем дату и время (первые 19 символов после #)
            if len(line) > 20:
                current_run = line[2:21].strip()
            continue

        # Проверяем, является ли строка данными
        parts = line.split('\t')
        if len(parts) >= 7:
            try:
                size = int(parts[0])
                threads = int(parts[1])
                time = float(parts[2])
                operations = float(parts[3])
                speedup = float(parts[4])
                efficiency = float(parts[5])
                status = parts[6]

                data.append({
                    'run': current_run,
                    'size': size,
                    'threads': threads,
                    'time': time,
                    'operations': operations,
                    'speedup': speedup,
                    'efficiency': efficiency,
                    'status': status
                })
            except (ValueError, IndexError) as e:
                # Пропускаем строки с ошибками
                continue

    return pd.DataFrame(data)


def plot_time_vs_threads(df):
    """График зависимости времени от количества потоков для разных размеров"""
    plt.figure(figsize=(12, 8))

    sizes = sorted(df['size'].unique())
    colors = plt.cm.viridis(np.linspace(0, 1, len(sizes)))

    for size, color in zip(sizes, colors):
        data = df[df['size'] == size].groupby('threads')['time'].mean().reset_index()
        data = data.sort_values('threads')
        plt.plot(data['threads'], data['time'], 'o-',
                 color=color, linewidth=2, markersize=8,
                 label=f'N={size}')

    plt.xlabel('Количество потоков', fontsize=12)
    plt.ylabel('Время выполнения (сек)', fontsize=12)
    plt.title('Зависимость времени выполнения от количества потоков', fontsize=14)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.xticks(sorted(df['threads'].unique()))
    plt.savefig('time_vs_threads.png', dpi=150, bbox_inches='tight')
    plt.show()


def plot_efficiency_vs_threads(df):
    """График эффективности от количества потоков"""
    plt.figure(figsize=(12, 8))

    sizes = sorted(df['size'].unique())
    colors = plt.cm.viridis(np.linspace(0, 1, len(sizes)))

    for size, color in zip(sizes, colors):
        data = df[df['size'] == size].groupby('threads')['efficiency'].mean().reset_index()
        data = data.sort_values('threads')
        plt.plot(data['threads'], data['efficiency'], 'o-',
                 color=color, linewidth=2, markersize=8,
                 label=f'N={size}')

    plt.xlabel('Количество потоков', fontsize=12)
    plt.ylabel('Эффективность (%)', fontsize=12)
    plt.title('Эффективность распараллеливания', fontsize=14)
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.xticks(sorted(df['threads'].unique()))
    plt.ylim(0, 120)
    plt.savefig('efficiency_vs_threads.png', dpi=150, bbox_inches='tight')
    plt.show()


def main():
    filename = 'results_omp.txt'
    lines = parse_results_file(filename)
       
    if df.empty:
        print("Нет данных для обработки!")
        return

    print(f"Загружено {len(df)} записей")
    print(f"Размеры матриц: {sorted(df['size'].unique())}")
    print(f"Количество потоков: {sorted(df['threads'].unique())}")
    print(f"Количество запусков: {df['run'].nunique()}")

    plot_time_vs_threads(df)
    plot_efficiency_vs_threads(df)


if __name__ == '__main__':
    main()
