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
SUMMARY = ROOT / "data" / "measurements" / "matrix_summary.csv"
OUTPUT = ROOT / "data" / "plots"

LABELS = {"naive": "Clásico", "strassen": "Strassen"}
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


def write_summary(summary: list[dict[str, object]]) -> None:
    with SUMMARY.open("w", newline="", encoding="utf-8") as stream:
        writer = csv.DictWriter(stream, fieldnames=list(summary[0]))
        writer.writeheader()
        writer.writerows(summary)


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


def matrix_type_plot(rows: list[dict[str, object]]) -> None:
    types_by_size: dict[int, set[str]] = defaultdict(set)
    for row in rows:
        types_by_size[int(row["n"])].add(str(row["type"]))
    candidates = [size for size, types in types_by_size.items() if len(types) >= 2]
    if not candidates:
        return
    selected_size = max(candidates)
    types = sorted(types_by_size[selected_size])

    fig, axis = plt.subplots(figsize=(8.2, 4.8), layout="constrained")
    width = 0.34
    positions = list(range(len(types)))
    for index, algorithm in enumerate(LABELS):
        medians = []
        for data_type in types:
            samples = [
                float(row["time_ms"])
                for row in rows
                if row["algorithm"] == algorithm
                and row["type"] == data_type
                and row["n"] == selected_size
            ]
            medians.append(median(samples) if samples else 0.0)
        offset = (index - 0.5) * width
        axis.bar(
            [position + offset for position in positions],
            medians,
            width=width,
            color=COLORS[algorithm],
            label=LABELS[algorithm],
        )
    axis.set_xticks(positions, [value.capitalize() for value in types])
    axis.set_yscale("log")
    axis.set_ylabel("Tiempo mediano (ms)")
    axis.set_title(f"Efecto del tipo de matriz para n={selected_size}")
    axis.grid(True, axis="y", which="both", linestyle=":", alpha=0.45)
    axis.legend()
    fig.savefig(OUTPUT / "matrix_by_type.png", dpi=180)
    plt.close(fig)


def main() -> None:
    OUTPUT.mkdir(parents=True, exist_ok=True)
    rows = read_measurements()
    summary = aggregate(rows)
    write_summary(summary)
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
    matrix_type_plot(rows)
    print(f"Gráficos guardados en {OUTPUT}")


if __name__ == "__main__":
    main()
