"""
INF-221 Algoritmos y Complejidad - 2026-2
Tarea 1 - Matías Jorquera - Rol 202310127-3

Genera gráficos PNG y un resumen CSV desde matrix_measurements.csv.
Referencias:
- Matplotlib, API de pyplot: https://matplotlib.org/stable/api/pyplot_summary.html
- Python, módulo csv: https://docs.python.org/3/library/csv.html
"""

from __future__ import annotations

import csv
from collections import defaultdict
from pathlib import Path
from statistics import median

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt


ROOT = Path(__file__).resolve().parents[1]
INPUT = ROOT / "data" / "measurements" / "matrix_measurements.csv"
OUTPUT = ROOT / "data" / "plots"

LABELS = {"naive": "Naive", "strassen": "Strassen"}
COLORS = {"naive": "#ea580c", "strassen": "#0891b2"}


def read_measurements() -> list[dict[str, object]]:
    if not INPUT.exists():
        raise FileNotFoundError(
            f"No existe {INPUT}. Ejecute primero el programa de matrices."
        )

    rows: list[dict[str, object]] = []
    with INPUT.open(newline="", encoding="utf-8") as stream:
        for row in csv.DictReader(stream):
            if row["correct"] != "1":
                continue
            rows.append(
                {
                    "algorithm": row["algorithm"],
                    "n": int(row["n"]),
                    "type": row["type"],
                    "domain": row["domain"],
                    "sample": row["sample"],
                    "time_ms": float(row["time_ms"]),
                    "peak_memory_mb": float(row["peak_memory_mb"]),
                }
            )
    if not rows:
        raise ValueError("El CSV no contiene mediciones correctas.")
    return rows


def aggregate(rows: list[dict[str, object]]) -> list[dict[str, object]]:
    groups: dict[tuple[str, int], list[dict[str, object]]] = defaultdict(list)
    for row in rows:
        groups[(str(row["algorithm"]), int(row["n"]))].append(row)

    result: list[dict[str, object]] = []
    for (algorithm, size), group in sorted(groups.items()):
        result.append(
            {
                "algorithm": algorithm,
                "n": size,
                "samples": len(group),
                "median_time_ms": median(float(row["time_ms"]) for row in group),
                "median_peak_memory_mb": median(
                    float(row["peak_memory_mb"]) for row in group
                ),
            }
        )
    return result


def line_plot(
    summary: list[dict[str, object]],
    metric: str,
    ylabel: str,
    title: str,
    filename: str,
    logarithmic_y: bool,
) -> None:
    fig, axis = plt.subplots(figsize=(8.2, 4.8), layout="constrained")
    for algorithm in LABELS:
        points = [row for row in summary if row["algorithm"] == algorithm]
        if not points:
            continue
        axis.plot(
            [int(row["n"]) for row in points],
            [max(float(row[metric]), 1e-9) for row in points],
            marker="o",
            linewidth=2,
            markersize=5,
            color=COLORS[algorithm],
            label=LABELS[algorithm],
        )
    axis.set_xscale("log", base=2)
    if logarithmic_y:
        axis.set_yscale("log")
    axis.set_xlabel("Dimensión de la matriz (n)")
    axis.set_ylabel(ylabel)
    axis.set_title(title)
    axis.grid(True, which="both", linestyle=":", alpha=0.45)
    axis.legend()
    fig.savefig(OUTPUT / filename, dpi=180)
    plt.close(fig)


def main() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    rows = read_measurements()
    summary = aggregate(rows)
    line_plot(
        summary,
        "median_time_ms",
        "Tiempo mediano (ms)",
        "Tiempo de multiplicación de matrices",
        "matrix_time.png",
        True,
    )
    line_plot(
        summary,
        "median_peak_memory_mb",
        "Memoria residente máxima (MiB)",
        "Memoria máxima de multiplicación de matrices",
        "matrix_memory.png",
        False,
    )


if __name__ == "__main__":
    main()
