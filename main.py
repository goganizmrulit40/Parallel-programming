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

    return lines


def main():
    filename = 'results_omp.txt'
    lines = parse_results_file(filename)
    print(f"Прочитано {len(lines)} строк")


if __name__ == '__main__':
    main()
  
