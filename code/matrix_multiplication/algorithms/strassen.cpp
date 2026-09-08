/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Algoritmo de Strassen con siete multiplicaciones recursivas, relleno con
 * ceros para dimensiones no potencia de dos y caso base clásico de tamaño 64.
 * Referencia original: Volker Strassen, "Gaussian elimination is not
 * optimal", Numerische Mathematik 13, pp. 354-356, 1969.
 * https://doi.org/10.1007/BF02165411
 */

#include "matrix_algorithms.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

namespace {

constexpr std::size_t kNaiveCutoff = 64;

Matrix add(const Matrix& left, const Matrix& right) {
    Matrix result(left.size());
    for (std::size_t index = 0; index < result.values().size(); ++index) {
        result.values()[index] = left.values()[index] + right.values()[index];
    }
    return result;
}

Matrix subtract(const Matrix& left, const Matrix& right) {
    Matrix result(left.size());
    for (std::size_t index = 0; index < result.values().size(); ++index) {
        result.values()[index] = left.values()[index] - right.values()[index];
    }
    return result;
}

Matrix quadrant(
    const Matrix& source,
    const std::size_t rowOffset,
    const std::size_t columnOffset,
    const std::size_t size
) {
    Matrix result(size);
    for (std::size_t row = 0; row < size; ++row) {
        for (std::size_t column = 0; column < size; ++column) {
            result(row, column) = source(row + rowOffset, column + columnOffset);
        }
    }
    return result;
}

void placeQuadrant(
    Matrix& destination,
    const Matrix& source,
    const std::size_t rowOffset,
    const std::size_t columnOffset
) {
    for (std::size_t row = 0; row < source.size(); ++row) {
        for (std::size_t column = 0; column < source.size(); ++column) {
            destination(row + rowOffset, column + columnOffset) = source(row, column);
        }
    }
}

Matrix strassenRecursive(const Matrix& left, const Matrix& right) {
    const std::size_t size = left.size();
    if (size <= kNaiveCutoff) {
        return naiveMultiply(left, right);
    }

    const std::size_t half = size / 2;
    const Matrix a11 = quadrant(left, 0, 0, half);
    const Matrix a12 = quadrant(left, 0, half, half);
    const Matrix a21 = quadrant(left, half, 0, half);
    const Matrix a22 = quadrant(left, half, half, half);
    const Matrix b11 = quadrant(right, 0, 0, half);
    const Matrix b12 = quadrant(right, 0, half, half);
    const Matrix b21 = quadrant(right, half, 0, half);
    const Matrix b22 = quadrant(right, half, half, half);

    const Matrix m1 = strassenRecursive(add(a11, a22), add(b11, b22));
    const Matrix m2 = strassenRecursive(add(a21, a22), b11);
    const Matrix m3 = strassenRecursive(a11, subtract(b12, b22));
    const Matrix m4 = strassenRecursive(a22, subtract(b21, b11));
    const Matrix m5 = strassenRecursive(add(a11, a12), b22);
    const Matrix m6 = strassenRecursive(subtract(a21, a11), add(b11, b12));
    const Matrix m7 = strassenRecursive(subtract(a12, a22), add(b21, b22));

    const Matrix c11 = add(subtract(add(m1, m4), m5), m7);
    const Matrix c12 = add(m3, m5);
    const Matrix c21 = add(m2, m4);
    const Matrix c22 = add(add(subtract(m1, m2), m3), m6);

    Matrix result(size);
    placeQuadrant(result, c11, 0, 0);
    placeQuadrant(result, c12, 0, half);
    placeQuadrant(result, c21, half, 0);
    placeQuadrant(result, c22, half, half);
    return result;
}

std::size_t nextPowerOfTwo(const std::size_t value) {
    std::size_t power = 1;
    while (power < value) {
        power *= 2;
    }
    return power;
}

}  // namespace

Matrix strassenMultiply(const Matrix& left, const Matrix& right) {
    if (left.size() != right.size()) {
        throw std::invalid_argument("Las matrices deben tener igual dimensión.");
    }
    if (left.size() == 0) {
        return Matrix{};
    }

    const std::size_t originalSize = left.size();
    const std::size_t paddedSize = nextPowerOfTwo(originalSize);
    if (paddedSize == originalSize) {
        return strassenRecursive(left, right);
    }

    Matrix paddedLeft(paddedSize);
    Matrix paddedRight(paddedSize);
    for (std::size_t row = 0; row < originalSize; ++row) {
        for (std::size_t column = 0; column < originalSize; ++column) {
            paddedLeft(row, column) = left(row, column);
            paddedRight(row, column) = right(row, column);
        }
    }

    const Matrix paddedResult = strassenRecursive(paddedLeft, paddedRight);
    Matrix result(originalSize);
    for (std::size_t row = 0; row < originalSize; ++row) {
        for (std::size_t column = 0; column < originalSize; ++column) {
            result(row, column) = paddedResult(row, column);
        }
    }
    return result;
}
