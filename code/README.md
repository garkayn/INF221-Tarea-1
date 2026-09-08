# Tarea 1 - Implementación y reproducción experimental

**Autor:** Matías Jorquera  
**Rol USM:** 202310127-3  
**Asignatura:** INF-221 Algoritmos y Complejidad, 2026-2

Implementé cuatro algoritmos de ordenamiento y dos algoritmos de
multiplicación de matrices. Los programas principales leen todos los datasets
válidos, ejecutan cada algoritmo en un proceso independiente, verifican que
los resultados coincidan y guardan las mediciones en CSV. Los scripts de
Python construyen los gráficos directamente desde esos CSV.

## Plataforma principal y requisitos

- Linux de 64 bits con `make`, `g++` y las utilidades POSIX estándar.
- Compilador C++ con soporte para C++17 (probado con `g++ 15.2.0`).
- GNU Make.
- Python 3.10 o superior.
- NumPy y Matplotlib: `python3 -m pip install -r requirements.txt`.

El proyecto requiere Linux. La medición de memoria usa `wait4(2)` y
`ru_maxrss`, y los procesos de medición se ejecutan mediante `fork` y
`execv`.

### Instalación recomendada de Python

Algunas distribuciones Linux marcan el Python del sistema como
`EXTERNALLY-MANAGED` y no permiten instalar paquetes directamente con
`pip`. Para mantener aisladas las dependencias de esta tarea, uso un entorno
virtual:

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install -r requirements.txt
```

Si `venv` no está instalado, en distribuciones basadas en Debian/Ubuntu se
debe instalar primero `python3-venv`. Cada vez que se abra una nueva terminal
hay que activar nuevamente el entorno con `source .venv/bin/activate`.

## Compilación y ejecución

Desde este directorio, en Linux:

```bash
make
make PYTHON=python benchmark
```

También puedo ejecutar cada experimento por separado:

```bash
make PYTHON=python -C sorting benchmark
make PYTHON=python -C matrix_multiplication benchmark
```

Los ejecutables usan rutas relativas, por lo que las reglas `run` de los
Makefiles deben ejecutarse desde sus respectivos directorios.

## Generación de datasets

Los generadores usan una semilla predeterminada igual a 221 para que los
experimentos sean reproducibles. Sin argumentos producen todas las
combinaciones indicadas en el enunciado:

```bash
python3 sorting/scripts/array_generator.py
python3 matrix_multiplication/scripts/matrix_generator.py
```

También aceptan subconjuntos. Por ejemplo:

```bash
python3 sorting/scripts/array_generator.py --sizes 10 1000 100000
python3 matrix_multiplication/scripts/matrix_generator.py --sizes 16 64 256
```

Los archivos existentes se conservan, salvo que agregue `--overwrite`.

## Ordenamiento

Implementé las siguientes variantes:

- **Merge Sort:** versión estable, `O(n log n)`, con un arreglo auxiliar
  reutilizado durante toda la recursión.
- **Quick Sort:** pivote mediana de tres, partición ternaria y recursión sobre
  la partición menor. La variante evita una pila lineal y maneja eficientemente
  valores repetidos.
- **Patience Sort:** construcción de pilas mediante búsqueda binaria y mezcla
  final mediante un min-heap. Las pilas se almacenan como nodos enlazados en
  un vector para reducir la sobrecarga de asignaciones.
- **std::sort:** adaptador directo a la biblioteca estándar de C++.

La interfaz común está en `sorting/algorithms/sorting_algorithms.hpp`. El
programa `sorting/sorting.cpp` deja los arreglos ordenados en
`sorting/data/array_output` y escribe
`sorting/data/measurements/sorting_measurements.csv`.

Columnas del CSV: algoritmo, tamaño, tipo, dominio, muestra, tiempo en
milisegundos, memoria residente máxima en MiB, validación y archivo de entrada.

## Multiplicación de matrices

Implementé:

- **Método clásico:** tres ciclos anidados, complejidad `O(n^3)`.
- **Strassen:** siete multiplicaciones recursivas, complejidad
  `O(n^log2(7))`, relleno con ceros para dimensiones que no son potencias de
  dos y caso base clásico para bloques de hasta 64 elementos.

La representación común está en
`matrix_multiplication/algorithms/matrix_algorithms.hpp` y usa enteros de 64
bits para evitar desbordamientos intermedios. El programa principal escribe
las matrices resultantes en `matrix_multiplication/data/matrix_output` y las
mediciones en `matrix_multiplication/data/measurements/matrix_measurements.csv`.

## Metodología de medición

Para que el máximo de memoria de un algoritmo no contamine al siguiente,
ejecuto cada medición en un proceso trabajador nuevo. El trabajador lee una
copia del dataset, mide únicamente la llamada al algoritmo con
`std::chrono::steady_clock` y calcula una suma de verificación. El proceso
padre obtiene la memoria residente máxima mediante `wait4(2)` y `ru_maxrss`.

Por lo tanto, el tiempo excluye lectura y escritura de archivos, mientras que
la memoria corresponde al proceso completo: incluye ejecutable, datos de
entrada y memoria auxiliar. Esta definición debe mantenerse al comparar todos
los algoritmos.

## Gráficos

Los scripts `plot_generator.py` usan Matplotlib con backend no interactivo y
generan gráficos de tiempo, memoria y sensibilidad al tipo de entrada en los
directorios `data/plots`. También producen `sorting_summary.csv` y
`matrix_summary.csv`, usando la mediana de las muestras disponibles.

## Limpieza

```bash
make clean
```

Esta regla elimina ejecutables, objetos y archivos de dependencias, pero
conserva datasets, resultados, mediciones y gráficos.

## Referencias principales

- T. H. Cormen, C. E. Leiserson, R. L. Rivest y C. Stein, *Introduction to
  Algorithms*, 4.a edición, MIT Press, 2022.
- C. L. Mallows, “Patience Sorting”, *SIAM Review*, 4(2), 148-149, 1962.
- V. Strassen, “Gaussian elimination is not optimal”, *Numerische
  Mathematik*, 13, 354-356, 1969.
- ISO/IEC 14882:2024, *Programming languages - C++*.
- Documentación oficial de Linux, Python, NumPy y Matplotlib indicada en los
  encabezados de los archivos correspondientes.
