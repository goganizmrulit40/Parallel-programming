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

    return pd.DataFrame(data)


def main():
    filename = 'results_omp.txt'
    lines = parse_results_file(filename)
    print(f"Создан DataFrame с {len(df)} записями")


if __name__ == '__main__':
    main()
  
