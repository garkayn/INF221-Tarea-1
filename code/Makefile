# INF-221 Algoritmos y Complejidad - 2026-2
# Tarea 1 - Matías Jorquera - Rol 202310127-3

.PHONY: all sorting matrix run plots benchmark clean

PYTHON ?= python3

all: sorting matrix

sorting:
	$(MAKE) -C sorting

matrix:
	$(MAKE) -C matrix_multiplication

run: all
	$(MAKE) -C sorting run
	$(MAKE) -C matrix_multiplication run

plots:
	$(PYTHON) sorting/scripts/plot_generator.py
	$(PYTHON) matrix_multiplication/scripts/plot_generator.py

benchmark: run plots

clean:
	$(MAKE) -C sorting clean
	$(MAKE) -C matrix_multiplication clean
