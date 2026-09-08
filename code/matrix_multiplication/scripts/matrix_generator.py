"""
INF-221 Algoritmos y Complejidad - 2026-2
Tarea 1 - Matías Jorquera - Rol 202310127-3

Generador reproducible de pares de matrices definidos en el enunciado.
Referencia: NumPy, documentación de Generator.integers:
https://numpy.org/doc/stable/reference/random/generated/numpy.random.Generator.integers.html
"""

from __future__ import annotations

import argparse
from itertools import product
from pathlib import Path

import numpy as np


DEFAULT_OUTPUT = Path(__file__).resolve().parents[1] / "data" / "matrix_input"


def generate_matrix(
    size: int, matrix_type: str, domain: str, rng: np.random.Generator
) -> np.ndarray:
    upper = 2 if domain == "D0" else 10
    if matrix_type == "densa":
        return rng.integers(0, upper, size=(size, size), dtype=np.int64)

    matrix = np.zeros((size, size), dtype=np.int64)
    if matrix_type == "diagonal":
        np.fill_diagonal(matrix, rng.integers(0, upper, size=size))
    elif matrix_type == "dispersa":
        count = max(1, size * size // 10)
        positions = rng.choice(size * size, size=count, replace=False)
        matrix.flat[positions] = rng.integers(1, upper, size=count)
    return matrix


def write_matrix(path: Path, matrix: np.ndarray) -> None:
    np.savetxt(path, matrix, fmt="%d")


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sizes", nargs="+", type=int, default=[16, 64, 256, 1024])
    parser.add_argument(
        "--types",
        nargs="+",
        choices=["dispersa", "diagonal", "densa"],
        default=["dispersa", "diagonal", "densa"],
    )
    parser.add_argument("--domains", nargs="+", choices=["D0", "D10"], default=["D0", "D10"])
    parser.add_argument("--samples", nargs="+", default=["a", "b", "c"])
    parser.add_argument("--seed", type=int, default=221)
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--overwrite", action="store_true")
    return parser.parse_args()


def main() -> None:
    arguments = parse_arguments()
    arguments.output_dir.mkdir(parents=True, exist_ok=True)
    rng = np.random.default_rng(arguments.seed)

    for size, matrix_type, domain, sample in product(
        arguments.sizes, arguments.types, arguments.domains, arguments.samples
    ):
        base = f"{size}_{matrix_type}_{domain}_{sample}"
        left_path = arguments.output_dir / f"{base}_1.txt"
        right_path = arguments.output_dir / f"{base}_2.txt"
        if left_path.exists() and right_path.exists() and not arguments.overwrite:
            print(f"Omitido (ya existe): {base}")
            continue
        write_matrix(left_path, generate_matrix(size, matrix_type, domain, rng))
        write_matrix(right_path, generate_matrix(size, matrix_type, domain, rng))
        print(f"Generado: {left_path.name}, {right_path.name}")


if __name__ == "__main__":
    main()
