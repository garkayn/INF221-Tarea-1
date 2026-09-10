/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Programa principal para medir multiplicación naive y Strassen. Cada
 * algoritmo se ejecuta en un proceso independiente; el tiempo excluye E/S y
 * la memoria informada es el máximo RSS del proceso trabajador.
 *
 * Referencias:
 * - Volker Strassen, "Gaussian elimination is not optimal", 1969.
 *   https://doi.org/10.1007/BF02165411
 * - Linux man-pages, wait4(2): https://man7.org/linux/man-pages/man2/wait4.2.html
 */

#include "../benchmark_process.hpp"
#include "algorithms/matrix_algorithms.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

struct Dataset {
    fs::path leftPath;
    fs::path rightPath;
    std::string stem;
    std::uint64_t declaredSize;
    std::string type;
    std::string domain;
    std::string sample;
};

struct WorkerResult {
    double elapsedMilliseconds{};
    std::uint64_t checksum{};
    std::uint64_t dimension{};
};

std::vector<std::string> split(const std::string& text, const char separator) {
    std::vector<std::string> fields;
    std::stringstream stream(text);
    std::string field;
    while (std::getline(stream, field, separator)) {
        fields.push_back(field);
    }
    return fields;
}

std::optional<Dataset> parseDataset(const fs::path& leftPath) {
    if (!fs::is_regular_file(leftPath) || leftPath.extension() != ".txt") {
        return std::nullopt;
    }

    const std::string inputStem = leftPath.stem().string();
    if (inputStem.size() < 3 || inputStem.substr(inputStem.size() - 2) != "_1") {
        return std::nullopt;
    }

    const std::string stem = inputStem.substr(0, inputStem.size() - 2);
    const auto fields = split(stem, '_');
    if (fields.size() != 4) {
        return std::nullopt;
    }

    const fs::path rightPath = leftPath.parent_path() / (stem + "_2.txt");
    if (!fs::is_regular_file(rightPath)) {
        throw std::runtime_error("Falta la matriz pareja " + rightPath.string());
    }

    try {
        const std::uint64_t size = std::stoull(fields[0]);
        return Dataset{
            leftPath, rightPath, stem, size, fields[1], fields[2], fields[3]
        };
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

Matrix readMatrix(const fs::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("No se pudo abrir " + path.string());
    }

    std::vector<MatrixValue> values;
    std::size_t rows = 0;
    std::size_t columns = 0;
    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream rowStream(line);
        std::vector<MatrixValue> row;
        MatrixValue value = 0;
        while (rowStream >> value) {
            row.push_back(value);
        }
        if (!rowStream.eof()) {
            throw std::runtime_error("Dato inválido en " + path.string());
        }
        if (columns == 0) {
            columns = row.size();
        } else if (row.size() != columns) {
            throw std::runtime_error("Filas con distinto largo en " + path.string());
        }
        values.insert(values.end(), row.begin(), row.end());
        ++rows;
    }

    if (rows == 0 || rows != columns) {
        throw std::runtime_error("La matriz no es cuadrada en " + path.string());
    }
    Matrix matrix(rows);
    matrix.values() = std::move(values);
    return matrix;
}

void writeMatrix(const fs::path& path, const Matrix& matrix) {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("No se pudo crear " + path.string());
    }

    for (std::size_t row = 0; row < matrix.size(); ++row) {
        for (std::size_t column = 0; column < matrix.size(); ++column) {
            if (column != 0) {
                output << ' ';
            }
            output << matrix(row, column);
        }
        output << '\n';
    }
}

std::uint64_t checksum(const Matrix& matrix) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (const MatrixValue value : matrix.values()) {
        hash ^= static_cast<std::uint64_t>(value);
        hash *= 1099511628211ULL;
    }
    return hash;
}

Matrix multiply(
    const std::string& algorithm,
    const Matrix& left,
    const Matrix& right
) {
    if (algorithm == "naive") {
        return naiveMultiply(left, right);
    }
    if (algorithm == "strassen") {
        return strassenMultiply(left, right);
    }
    throw std::invalid_argument("Algoritmo desconocido: " + algorithm);
}

int runWorker(
    const std::string& algorithm,
    const fs::path& leftPath,
    const fs::path& rightPath,
    const fs::path& resultPath,
    const std::string& outputArgument
) {
    const Matrix left = readMatrix(leftPath);
    const Matrix right = readMatrix(rightPath);
    if (left.size() != right.size()) {
        throw std::runtime_error("Las matrices de entrada tienen distinta dimensión.");
    }

    const auto start = std::chrono::steady_clock::now();
    const Matrix result = multiply(algorithm, left, right);
    const auto finish = std::chrono::steady_clock::now();
    const double elapsed = std::chrono::duration<double, std::milli>(
        finish - start
    ).count();

    if (outputArgument != "-") {
        writeMatrix(fs::path(outputArgument), result);
    }

    std::ofstream measurement(resultPath);
    if (!measurement) {
        throw std::runtime_error(
            "No se pudo crear el resultado temporal de la medición."
        );
    }
    measurement << std::setprecision(17) << elapsed << ' ' << checksum(result)
                << ' ' << result.size() << '\n';
    return 0;
}

WorkerResult readWorkerResult(const fs::path& path) {
    std::ifstream input(path);
    WorkerResult result;
    if (!(input >> result.elapsedMilliseconds >> result.checksum
                >> result.dimension)) {
        throw std::runtime_error("Resultado temporal de medición inválido.");
    }
    return result;
}

std::string csvField(const std::string& value) {
    std::string escaped = "\"";
    for (const char character : value) {
        escaped += character;
        if (character == '"') {
            escaped += '"';
        }
    }
    escaped += '"';
    return escaped;
}

int runBenchmark(const fs::path& executable) {
    const fs::path inputDirectory = "data/matrix_input";
    const fs::path outputDirectory = "data/matrix_output";
    const fs::path measurementDirectory = "data/measurements";

    if (!fs::is_directory(inputDirectory)) {
        throw std::runtime_error(
            "No existe el directorio " + inputDirectory.string()
        );
    }
    fs::create_directories(outputDirectory);
    fs::create_directories(measurementDirectory);

    std::vector<Dataset> datasets;
    for (const auto& entry : fs::directory_iterator(inputDirectory)) {
        if (const auto dataset = parseDataset(entry.path())) {
            datasets.push_back(*dataset);
        }
    }
    std::sort(datasets.begin(), datasets.end(), [](const auto& left, const auto& right) {
        return left.leftPath.filename() < right.leftPath.filename();
    });
    if (datasets.empty()) {
        throw std::runtime_error("No se encontraron pares de matrices válidos.");
    }

    const std::vector<std::string> algorithms{"naive", "strassen"};
    const fs::path csvPath = measurementDirectory / "matrix_measurements.csv";
    std::ofstream csv(csvPath);
    if (!csv) {
        throw std::runtime_error("No se pudo crear " + csvPath.string());
    }
    csv << "algorithm,n,type,domain,sample,time_ms,peak_memory_mb,correct,input_a,input_b\n";
    csv << std::fixed << std::setprecision(6);

    for (const auto& dataset : datasets) {
        std::optional<std::uint64_t> referenceChecksum;
        for (std::size_t index = 0; index < algorithms.size(); ++index) {
            const std::string& algorithm = algorithms[index];
            const fs::path resultPath = measurementDirectory /
                (".matrix_worker_" + std::to_string(index) + ".tmp");
            const fs::path outputPath = outputDirectory /
                (dataset.stem + "_out.txt");
            std::error_code removeError;
            fs::remove(resultPath, removeError);

            const auto process = benchmark::runAndMeasure(
                executable.string(),
                {
                    "--worker", algorithm, dataset.leftPath.string(),
                    dataset.rightPath.string(), resultPath.string(),
                    index == 0 ? outputPath.string() : "-"
                }
            );
            if (process.exitCode != 0) {
                throw std::runtime_error(
                    algorithm + " falló para " + dataset.stem
                );
            }

            const WorkerResult worker = readWorkerResult(resultPath);
            fs::remove(resultPath, removeError);
            bool correct = worker.dimension == dataset.declaredSize;
            if (!referenceChecksum) {
                referenceChecksum = worker.checksum;
            } else {
                correct = correct && worker.checksum == *referenceChecksum;
            }

            const double peakMegabytes = static_cast<double>(
                process.peakResidentBytes
            ) / (1024.0 * 1024.0);
            csv << algorithm << ',' << dataset.declaredSize << ','
                << dataset.type << ',' << dataset.domain << ',' << dataset.sample
                << ',' << worker.elapsedMilliseconds << ',' << peakMegabytes << ','
                << (correct ? 1 : 0) << ','
                << csvField(dataset.leftPath.generic_string()) << ','
                << csvField(dataset.rightPath.generic_string()) << '\n';
            csv.flush();

            std::cout << dataset.stem << " | " << algorithm << " | "
                      << worker.elapsedMilliseconds << " ms | " << peakMegabytes
                      << " MiB | " << (correct ? "OK" : "ERROR") << '\n';
            if (!correct) {
                throw std::runtime_error(
                    "Los algoritmos no coinciden para " + dataset.stem
                );
            }
        }
    }

    std::cout << "Mediciones guardadas en " << csvPath << '\n';
    return 0;
}

void printUsage(const char* executable) {
    std::cout << "Uso: " << executable << "\n"
              << "Ejecuta todos los pares válidos de data/matrix_input.\n";
}

}

int main(int argc, char* argv[]) {
    try {
        if (argc >= 2 && std::string(argv[1]) == "--worker") {
            if (argc != 7) {
                throw std::invalid_argument("Argumentos inválidos para --worker.");
            }
            return runWorker(argv[2], argv[3], argv[4], argv[5], argv[6]);
        }
        if (argc == 2 && std::string(argv[1]) == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        if (argc != 1) {
            printUsage(argv[0]);
            return 1;
        }

        return runBenchmark(fs::absolute(argv[0]));
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}
