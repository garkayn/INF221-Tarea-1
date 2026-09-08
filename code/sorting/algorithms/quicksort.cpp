/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Quick Sort in-place con pivote mediana de tres, partición ternaria y
 * recursión sobre la partición menor para limitar la profundidad de la pila.
 * Referencia: Cormen, Leiserson, Rivest y Stein, Introduction to
 * Algorithms, 4.a edición, MIT Press, 2022, capítulo 7.
 * https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/
 */

#include "sorting_algorithms.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

int medianOfThree(const int first, const int middle, const int last) {
    if ((first < middle) != (first < last)) {
        return first;
    }
    if ((middle < first) != (middle < last)) {
        return middle;
    }
    return last;
}

void quickSortRange(
    std::vector<int>& values,
    std::ptrdiff_t left,
    std::ptrdiff_t right
) {
    while (left < right) {
        const std::ptrdiff_t middle = left + (right - left) / 2;
        const int pivot = medianOfThree(
            values[static_cast<std::size_t>(left)],
            values[static_cast<std::size_t>(middle)],
            values[static_cast<std::size_t>(right)]
        );

        std::ptrdiff_t less = left;
        std::ptrdiff_t current = left;
        std::ptrdiff_t greater = right;

        while (current <= greater) {
            int& value = values[static_cast<std::size_t>(current)];
            if (value < pivot) {
                std::swap(
                    values[static_cast<std::size_t>(less)],
                    values[static_cast<std::size_t>(current)]
                );
                ++less;
                ++current;
            } else if (value > pivot) {
                std::swap(
                    values[static_cast<std::size_t>(current)],
                    values[static_cast<std::size_t>(greater)]
                );
                --greater;
            } else {
                ++current;
            }
        }

        const std::ptrdiff_t leftSize = less - left;
        const std::ptrdiff_t rightSize = right - greater;
        if (leftSize < rightSize) {
            quickSortRange(values, left, less - 1);
            left = greater + 1;
        } else {
            quickSortRange(values, greater + 1, right);
            right = less - 1;
        }
    }
}

}  // namespace

void quickSort(std::vector<int>& values) {
    if (!values.empty()) {
        quickSortRange(
            values, 0, static_cast<std::ptrdiff_t>(values.size() - 1)
        );
    }
}
