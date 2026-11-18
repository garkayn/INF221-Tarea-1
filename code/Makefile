# INF-221 Algoritmos y Complejidad - 2026-2
# Tarea 1 - Matías Jorquera - Rol 202310127-3

.PHONY: all benchmark sorting-experiment matrix-experiment \
	build sorting matrix generate inputs generate-sorting generate-matrix \
	measure run measure-sorting measure-matrix plots plots-sorting plots-matrix \
	execute clean

PYTHON ?= python3

all: benchmark

benchmark:
	$(MAKE) clean
	$(MAKE) inputs
	$(MAKE) build
	$(MAKE) measure
	$(MAKE) plots

sorting-experiment:
	$(MAKE) clean
	$(MAKE) generate-sorting
	$(MAKE) sorting
	$(MAKE) measure-sorting
	$(MAKE) plots-sorting

matrix-experiment:
	$(MAKE) clean
	$(MAKE) generate-matrix
	$(MAKE) matrix
	$(MAKE) measure-matrix
	$(MAKE) plots-matrix

build: sorting matrix

sorting:
	$(MAKE) -C sorting

matrix:
	$(MAKE) -C matrix_multiplication

generate: inputs

inputs: generate-sorting generate-matrix

generate-sorting:
	cd sorting/scripts && mkdir -p ../data/array_input && $(PYTHON) array_generator.py

generate-matrix:
	cd matrix_multiplication/scripts && mkdir -p ../data/matrix_input && $(PYTHON) matrix_generator.py

measure: measure-sorting measure-matrix

run: measure

measure-sorting:
	$(MAKE) -C sorting execute

measure-matrix:
	$(MAKE) -C matrix_multiplication execute

execute:
	$(MAKE) measure

plots: plots-sorting plots-matrix

plots-sorting:
	$(PYTHON) sorting/scripts/plot_generator.py

plots-matrix:
	$(PYTHON) matrix_multiplication/scripts/plot_generator.py


clean:
	$(MAKE) -C sorting clean
	$(MAKE) -C matrix_multiplication clean
