/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Interfaz común de los cuatro algoritmos de ordenamiento evaluados.
 */

#pragma once

#include <vector>

void mergeSort(std::vector<int>& values);
void quickSort(std::vector<int>& values);
void patienceSort(std::vector<int>& values);
std::vector<int> sortArray(std::vector<int>& values);
