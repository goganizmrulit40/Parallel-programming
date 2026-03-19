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


if __name__ == '__main__':
    main()
  
