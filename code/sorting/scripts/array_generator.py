"""
INF-221 Algoritmos y Complejidad - 2026-2
Tarea 1 - Matías Jorquera - Rol 202310127-3

Generador reproducible de los arreglos definidos en el enunciado.
Referencia: NumPy, documentación de Generator.integers y ndarray.sort:
https://numpy.org/doc/stable/reference/random/generated/numpy.random.Generator.integers.html
"""

from __future__ import annotations

import argparse
from itertools import product
from pathlib import Path

import numpy as np


DEFAULT_OUTPUT = Path(__file__).resolve().parents[1] / "data" / "array_input"


def generate_array(
    size: int, data_type: str, domain: str, rng: np.random.Generator
) -> np.ndarray:
    upper = 10 if domain == "D1" else 10**7 + 1
    values = rng.integers(0, upper, size=size, dtype=np.int32)
    if data_type == "ascendente":
        values.sort()
    elif data_type == "descendente":
        values.sort()
        values = values[::-1]
    return values


def parse_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--sizes", nargs="+", type=int, default=[10, 10**3, 10**5, 10**7])
    parser.add_argument(
        "--types",
        nargs="+",
        choices=["ascendente", "descendente", "aleatorio"],
        default=["ascendente", "descendente", "aleatorio"],
    )
    parser.add_argument("--domains", nargs="+", choices=["D1", "D7"], default=["D1", "D7"])
    parser.add_argument("--samples", nargs="+", default=["a", "b", "c"])
    parser.add_argument("--seed", type=int, default=221)
    parser.add_argument("--output-dir", type=Path, default=DEFAULT_OUTPUT)
    parser.add_argument("--overwrite", action="store_true")
    return parser.parse_args()


def main() -> None:
    arguments = parse_arguments()
    arguments.output_dir.mkdir(parents=True, exist_ok=True)
    rng = np.random.default_rng(arguments.seed)

    for size, data_type, domain, sample in product(
        arguments.sizes, arguments.types, arguments.domains, arguments.samples
    ):
        path = arguments.output_dir / f"{size}_{data_type}_{domain}_{sample}.txt"
        if path.exists() and not arguments.overwrite:
            print(f"Omitido (ya existe): {path.name}")
            continue
        values = generate_array(size, data_type, domain, rng)
        values.tofile(path, sep=" ", format="%d")
        with path.open("a", encoding="utf-8") as stream:
            stream.write("\n")
        print(f"Generado: {path.name}")


if __name__ == "__main__":
    main()
