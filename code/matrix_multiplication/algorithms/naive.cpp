/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Multiplicación naive de matrices con tres ciclos anidados.
 * Referencia: Cormen, Leiserson, Rivest y Stein, Introduction to
 * Algorithms, 4.a edición, MIT Press, 2022, sección 4.1.
 * https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/
 */

#include "matrix_algorithms.hpp"

#include <cstddef>
#include <stdexcept>

Matrix naiveMultiply(const Matrix& left, const Matrix& right) {
    if (left.size() != right.size()) {
        throw std::invalid_argument("Las matrices deben tener igual dimensión.");
    }

    const std::size_t size = left.size();
    Matrix result(size);
    for (std::size_t row = 0; row < size; ++row) {
        for (std::size_t inner = 0; inner < size; ++inner) {
            const MatrixValue leftValue = left(row, inner);
            for (std::size_t column = 0; column < size; ++column) {
                result(row, column) += leftValue * right(inner, column);
            }
        }
    }
    return result;
}
