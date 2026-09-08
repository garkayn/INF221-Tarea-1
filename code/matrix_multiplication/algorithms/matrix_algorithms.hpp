/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Tipo de matriz e interfaz común de los algoritmos de multiplicación.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

using MatrixValue = std::int64_t;

class Matrix {
public:
    Matrix() = default;
    explicit Matrix(const std::size_t size, const MatrixValue initial = 0)
        : size_(size), values_(size * size, initial) {}

    std::size_t size() const noexcept {
        return size_;
    }

    MatrixValue& operator()(const std::size_t row, const std::size_t column) {
        return values_[row * size_ + column];
    }

    const MatrixValue& operator()(
        const std::size_t row,
        const std::size_t column
    ) const {
        return values_[row * size_ + column];
    }

    std::vector<MatrixValue>& values() noexcept {
        return values_;
    }

    const std::vector<MatrixValue>& values() const noexcept {
        return values_;
    }

private:
    std::size_t size_{};
    std::vector<MatrixValue> values_;
};

Matrix naiveMultiply(const Matrix& left, const Matrix& right);
Matrix strassenMultiply(const Matrix& left, const Matrix& right);
