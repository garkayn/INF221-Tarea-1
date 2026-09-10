/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Merge Sort estable con un único arreglo auxiliar reutilizable.
 * Referencia: Cormen, Leiserson, Rivest y Stein, Introduction to
 * Algorithms, 4.a edición, MIT Press, 2022, capítulo 2.
 * https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/
 */

#include "sorting_algorithms.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

namespace {

void mergeRanges(
    std::vector<int>& values,
    std::vector<int>& auxiliary,
    const std::size_t begin,
    const std::size_t middle,
    const std::size_t end
) {
    std::size_t left = begin;
    std::size_t right = middle;
    std::size_t output = begin;

    while (left < middle && right < end) {
        if (values[left] <= values[right]) {
            auxiliary[output++] = values[left++];
        } else {
            auxiliary[output++] = values[right++];
        }
    }

    while (left < middle) {
        auxiliary[output++] = values[left++];
    }
    while (right < end) {
        auxiliary[output++] = values[right++];
    }

    std::copy(
        auxiliary.begin() + static_cast<std::ptrdiff_t>(begin),
        auxiliary.begin() + static_cast<std::ptrdiff_t>(end),
        values.begin() + static_cast<std::ptrdiff_t>(begin)
    );
}

void mergeSortRange(
    std::vector<int>& values,
    std::vector<int>& auxiliary,
    const std::size_t begin,
    const std::size_t end
) {
    if (end - begin <= 1) {
        return;
    }

    const std::size_t middle = begin + (end - begin) / 2;
    mergeSortRange(values, auxiliary, begin, middle);
    mergeSortRange(values, auxiliary, middle, end);

    if (values[middle - 1] <= values[middle]) {
        return;
    }
    mergeRanges(values, auxiliary, begin, middle, end);
}

}

void mergeSort(std::vector<int>& values) {
    std::vector<int> auxiliary(values.size());
    mergeSortRange(values, auxiliary, 0, values.size());
}
