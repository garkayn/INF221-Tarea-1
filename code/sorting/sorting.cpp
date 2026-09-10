/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Programa principal para ejecutar Merge Sort, Quick Sort, Patience Sort y
 * std::sort. Cada medición se realiza en un proceso independiente: el tiempo
 * considera únicamente el algoritmo y la memoria corresponde al máximo RSS
 * del proceso trabajador. Los resultados se guardan como CSV reproducible.
 *
 * Referencias:
 * - Cormen et al., Introduction to Algorithms, 4.a ed., MIT Press, 2022.
 *   https://mitpress.mit.edu/9780262046305/introduction-to-algorithms/
 * - Linux man-pages, wait4(2): https://man7.org/linux/man-pages/man2/wait4.2.html
 */

#include "../benchmark_process.hpp"
#include "algorithms/sorting_algorithms.hpp"

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
    fs::path inputPath;
    std::string stem;
    std::uint64_t declaredSize;
    std::string type;
    std::string domain;
    std::string sample;
};

struct WorkerResult {
    double elapsedMilliseconds{};
    std::uint64_t checksum{};
    bool sorted{};
    std::uint64_t elementCount{};
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

std::optional<Dataset> parseDataset(const fs::path& path) {
    if (!fs::is_regular_file(path) || path.extension() != ".txt") {
        return std::nullopt;
    }

    const std::string stem = path.stem().string();
    const auto fields = split(stem, '_');
    if (fields.size() != 4) {
        return std::nullopt;
    }

    try {
        const std::uint64_t size = std::stoull(fields[0]);
        return Dataset{path, stem, size, fields[1], fields[2], fields[3]};
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

std::vector<int> readArray(const fs::path& path) {
    std::ifstream input(path);
    if (!input) {
        throw std::runtime_error("No se pudo abrir " + path.string());
    }

    std::vector<int> values;
    int value = 0;
    while (input >> value) {
        values.push_back(value);
    }
    if (!input.eof()) {
        throw std::runtime_error("Dato inválido en " + path.string());
    }
    return values;
}

void writeArray(const fs::path& path, const std::vector<int>& values) {
    std::ofstream output(path);
    if (!output) {
        throw std::runtime_error("No se pudo crear " + path.string());
    }

    for (std::size_t index = 0; index < values.size(); ++index) {
        if (index != 0) {
            output << ' ';
        }
        output << values[index];
    }
    output << '\n';
}

std::uint64_t checksum(const std::vector<int>& values) {
    std::uint64_t hash = 1469598103934665603ULL;
    for (const int value : values) {
        hash ^= static_cast<std::uint32_t>(value);
        hash *= 1099511628211ULL;
    }
    return hash;
}

using SortFunction = void (*)(std::vector<int>&);

void standardSortAdapter(std::vector<int>& values) {
    values = sortArray(values);
}

SortFunction findAlgorithm(const std::string& name) {
    if (name == "merge_sort") {
        return mergeSort;
    }
    if (name == "quick_sort") {
        return quickSort;
    }
    if (name == "patience_sort") {
        return patienceSort;
    }
    if (name == "std_sort") {
        return standardSortAdapter;
    }
    throw std::invalid_argument("Algoritmo desconocido: " + name);
}

int runWorker(
    const std::string& algorithm,
    const fs::path& inputPath,
    const fs::path& resultPath,
    const std::string& outputArgument
) {
    auto values = readArray(inputPath);
    const SortFunction sort = findAlgorithm(algorithm);

    const auto start = std::chrono::steady_clock::now();
    sort(values);
    const auto finish = std::chrono::steady_clock::now();

    const bool sorted = std::is_sorted(values.begin(), values.end());
    const double elapsed = std::chrono::duration<double, std::milli>(
        finish - start
    ).count();

    if (outputArgument != "-") {
        writeArray(fs::path(outputArgument), values);
    }

    std::ofstream result(resultPath);
    if (!result) {
        throw std::runtime_error(
            "No se pudo crear el resultado temporal de la medición."
        );
    }
    result << std::setprecision(17) << elapsed << ' ' << checksum(values) << ' '
           << (sorted ? 1 : 0) << ' ' << values.size() << '\n';
    return sorted ? 0 : 2;
}

WorkerResult readWorkerResult(const fs::path& path) {
    std::ifstream input(path);
    WorkerResult result;
    int sorted = 0;
    if (!(input >> result.elapsedMilliseconds >> result.checksum >> sorted
                >> result.elementCount)) {
        throw std::runtime_error("Resultado temporal de medición inválido.");
    }
    result.sorted = sorted != 0;
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
    const fs::path inputDirectory = "data/array_input";
    const fs::path outputDirectory = "data/array_output";
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
        return left.inputPath.filename() < right.inputPath.filename();
    });
    if (datasets.empty()) {
        throw std::runtime_error("No se encontraron datasets de ordenamiento válidos.");
    }

    const std::vector<std::string> algorithms{
        "merge_sort", "quick_sort", "patience_sort", "std_sort"
    };
    const fs::path csvPath = measurementDirectory / "sorting_measurements.csv";
    std::ofstream csv(csvPath);
    if (!csv) {
        throw std::runtime_error("No se pudo crear " + csvPath.string());
    }
    csv << "algorithm,n,type,domain,sample,time_ms,peak_memory_mb,correct,input_file\n";
    csv << std::fixed << std::setprecision(6);

    for (const auto& dataset : datasets) {
        std::optional<std::uint64_t> referenceChecksum;
        for (std::size_t index = 0; index < algorithms.size(); ++index) {
            const std::string& algorithm = algorithms[index];
            const fs::path resultPath = measurementDirectory /
                (".sorting_worker_" + std::to_string(index) + ".tmp");
            const fs::path outputPath = outputDirectory /
                (dataset.stem + "_out.txt");
            std::error_code removeError;
            fs::remove(resultPath, removeError);

            const auto process = benchmark::runAndMeasure(
                executable.string(),
                {
                    "--worker", algorithm, dataset.inputPath.string(),
                    resultPath.string(), index == 0 ? outputPath.string() : "-"
                }
            );
            if (process.exitCode != 0) {
                throw std::runtime_error(
                    algorithm + " falló para " + dataset.inputPath.string()
                );
            }

            const WorkerResult worker = readWorkerResult(resultPath);
            fs::remove(resultPath, removeError);
            bool correct = worker.sorted && worker.elementCount == dataset.declaredSize;
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
                << csvField(dataset.inputPath.generic_string()) << '\n';
            csv.flush();

            std::cout << dataset.stem << " | " << algorithm << " | "
                      << worker.elapsedMilliseconds << " ms | " << peakMegabytes
                      << " MiB | " << (correct ? "OK" : "ERROR") << '\n';
            if (!correct) {
                throw std::runtime_error(
                    "La validación falló para " + algorithm + " en " + dataset.stem
                );
            }
        }
    }

    std::cout << "Mediciones guardadas en " << csvPath << '\n';
    return 0;
}

void printUsage(const char* executable) {
    std::cout << "Uso: " << executable << "\n"
              << "Ejecuta todos los datasets válidos de data/array_input.\n";
}

}

int main(int argc, char* argv[]) {
    try {
        if (argc >= 2 && std::string(argv[1]) == "--worker") {
            if (argc != 6) {
                throw std::invalid_argument("Argumentos inválidos para --worker.");
            }
            return runWorker(argv[2], argv[3], argv[4], argv[5]);
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
