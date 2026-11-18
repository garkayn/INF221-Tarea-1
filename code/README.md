# Documentación

**Autor:** Matías Jorquera  
**Rol USM:** 202310127-3  
**Asignatura:** INF-221 Algoritmos y Complejidad, 2026-2

## Multiplicación de matrices

Se implementaron el método naive y el algoritmo de Strassen para multiplicar
matrices cuadradas. Ambos algoritmos verifican sus resultados mediante una suma
de verificación común.

### Programa principal

El programa principal se encuentra en:

```text
matrix_multiplication/matrix_multiplication.cpp
```

El programa lee pares de matrices desde `data/matrix_input`, ejecuta cada
algoritmo en un proceso independiente y guarda los resultados en:

```text
matrix_multiplication/data/matrix_output
```

Las mediciones se escriben en:

```text
matrix_multiplication/data/measurements/matrix_measurements.csv
```

Las columnas del CSV son: algoritmo, tamaño, tipo, dominio, muestra, tiempo
en milisegundos, memoria residente máxima en MiB, validación y archivos de
entrada.

La representación común de las matrices está en
`matrix_multiplication/algorithms/matrix_algorithms.hpp` y utiliza enteros de
64 bits para evitar desbordamientos intermedios.

Se implementaron:

- **Método naive:** tres ciclos anidados, con complejidad `O(n^3)`.
- **Strassen:** siete multiplicaciones recursivas, con complejidad
  `O(n^log2(7))`.

### Scripts

El generador de matrices se encuentra en:

```text
matrix_multiplication/scripts/matrix_generator.py
```

Al ejecutarse sin argumentos, genera matrices de dimensiones
`2^4 = 16`, `2^6 = 64`, `2^8 = 256` y `2^10 = 1024`. Por lo tanto, el mayor
tamaño generado por defecto es `1024 x 1024`. Para cada dimensión combina los
tipos `dispersa`, `diagonal` y `densa`, los dominios `D0` y `D10`, y las tres
muestras `a`, `b` y `c`. Como genera dos matrices por combinación, produce
72 pares, equivalentes a 144 archivos de entrada.

Se ejecuta con:

```bash
cd matrix_multiplication/scripts
python3 matrix_generator.py
cd ../..
```

El generador de gráficos se encuentra en:

```text
matrix_multiplication/scripts/plot_generator.py
```

Lee `matrix_measurements.csv`, calcula medianas por algoritmo y
genera los siguientes archivos en `matrix_multiplication/data/plots`:

- `matrix_time.png`: tiempo mediano de multiplicación.
- `matrix_memory.png`: memoria residente máxima.

## Ordenamiento de arreglo unidimensional

Se implementaron los algoritmos Merge Sort, Quick Sort, Patience Sort y
`std::sort`.

### Programa principal

El programa principal se encuentra en:

```text
sorting/sorting.cpp
```

Lee los arreglos desde `sorting/data/array_input`, ejecuta cada algoritmo en
un proceso independiente, verifica que el resultado quede ordenado y guarda
los arreglos ordenados en:

```text
sorting/data/array_output
```

Las mediciones se escriben en:

```text
sorting/data/measurements/sorting_measurements.csv
```

Las columnas del CSV son: algoritmo, tamaño, tipo, dominio, muestra, tiempo
en milisegundos, memoria residente máxima en MiB, validación y archivo de
entrada.

Las variantes implementadas son:

- **Merge Sort:** versión estable con un arreglo auxiliar reutilizado durante
  toda la recursión.
- **Quick Sort:** pivote mediana de tres, partición ternaria y recursión sobre
  la partición menor.
- **Patience Sort:** construcción de pilas mediante búsqueda binaria y mezcla
  final mediante un min-heap.
- **`std::sort`:** adaptador directo de la biblioteca estándar.

### Scripts

El generador de arreglos se encuentra en:

```text
sorting/scripts/array_generator.py
```

Genera 72 archivos de entrada con tamaños `10`, `1000`, `100000` y
`10000000`, los tipos `ascendente`, `descendente` y `aleatorio`, los dominios
`D1` y `D7`, y las muestras `a`, `b` y `c`.

Como las rutas del script son relativas a su directorio, se ejecuta con:

```bash
cd sorting/scripts
python3 array_generator.py
cd ../..
```

El generador de gráficos se encuentra en:

```text
sorting/scripts/plot_generator.py
```

Este script genera los gráficos de tiempo, memoria y efecto de la distribución
de entrada en `sorting/data/plots`:

- `sorting_time.png`: tiempo mediano.
- `sorting_memory.png`: memoria residente máxima.
- `sorting_by_distribution.png`: comparación entre arreglos aleatorios,
  ascendentes y descendentes.

## Plataforma y requisitos

El proyecto está preparado para Linux de 64 bits y requiere:

- `make`.
- Un compilador C++17, probado con `g++ 15.2.0`.
- Python 3.10 o superior.
- NumPy entre las versiones `1.26` y `3.0`.
- Matplotlib entre las versiones `3.8` y `4.0`.

La medición de memoria utiliza `wait4(2)` y `ru_maxrss`, mientras que los
procesos de medición se ejecutan mediante `fork` y `execv`.

### Instalación de las dependencias de Python

Se recomienda utilizar un entorno virtual para instalar las dependencias:

```bash
python3 -m venv .venv
source .venv/bin/activate
python -m pip install "numpy>=1.26,<3.0" "matplotlib>=3.8,<4.0"
```

## Compilación y ejecución

Desde el directorio `code`, en Linux, el flujo completo se ejecuta con:

```bash
make
make PYTHON=python benchmark
```

`make` y `make benchmark` limpian los objetos y ejecutables anteriores,
generan los datasets, compilan los programas, ejecutan las mediciones y
generan los gráficos.

También es posible ejecutar un solo experimento completo:

```bash
make sorting-experiment PYTHON=python
make matrix-experiment PYTHON=python
```

Las etapas se pueden ejecutar por separado:

```bash
make clean
make generate
make build
make measure
make plots PYTHON=python
```

También existen comandos específicos para cada experimento:

```bash
make generate-sorting
make generate-matrix
make sorting
make matrix
make measure-sorting
make measure-matrix
make plots-sorting PYTHON=python
make plots-matrix PYTHON=python
```

El target `run` ejecuta las mediciones usando los inputs y ejecutables
existentes, sin regenerar datasets. Los targets `sorting` y `matrix` solo
compilan su respectivo programa.

También es posible ejecutar cada experimento por separado (los usé para testear, pero mejor los dejo por si acaso):

```bash
make PYTHON=python -C sorting benchmark
make PYTHON=python -C matrix_multiplication benchmark
```

Los ejecutables utilizan rutas relativas, por lo que las reglas `run` de los
Makefiles deben ejecutarse desde sus respectivos directorios.

## Metodología de medición

Para evitar que la memoria máxima de un algoritmo contamine la medición del
siguiente, cada ejecución se realiza en un proceso trabajador nuevo. El
trabajador lee una copia del dataset, mide únicamente la llamada al algoritmo
con `std::chrono::steady_clock` y calcula una suma de verificación.

El proceso padre obtiene la memoria residente máxima mediante `wait4(2)` y
`ru_maxrss`.

Por lo tanto, el tiempo informado excluye la lectura y escritura de archivos,
mientras que la memoria corresponde al proceso completo e incluye el
ejecutable, los datos de entrada y la memoria auxiliar.

## Limpieza

```bash
make clean
```

Esta regla elimina ejecutables, objetos y archivos de dependencias, pero
conserva datasets, resultados, mediciones y gráficos.

## Referencias principales

- T. H. Cormen, C. L. Leiserson, R. L. Rivest y C. Stein, *Introduction to
  Algorithms*, 4.a edición, MIT Press, 2022.
- C. L. Mallows, “Patience Sorting”, *SIAM Review*, 4(2), 148-149, 1962.
- V. Strassen, “Gaussian elimination is not optimal”, *Numerische
  Mathematik*, 13, 354-356, 1969.
- ISO/IEC 14882:2024, *Programming languages - C++*.
- Documentación oficial de Linux, Python, NumPy y Matplotlib.
