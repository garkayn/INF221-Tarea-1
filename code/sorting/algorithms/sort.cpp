/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Adaptador para std::sort de la biblioteca estándar de C++.
 * Referencia: ISO/IEC 14882:2024, Programming languages - C++.
 * https://committee.iso.org/standard/83626.html
 */

#include "sorting_algorithms.hpp"

#include <algorithm>
#include <vector>

void standardSort(std::vector<int>& values) {
    std::sort(values.begin(), values.end());
}
