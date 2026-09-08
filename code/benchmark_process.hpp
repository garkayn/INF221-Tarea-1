/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Ejecución aislada de un algoritmo y medición de su memoria residente
 * máxima en Linux. wait4(2) entrega el uso máximo de memoria del proceso hijo
 * mediante ru_maxrss, expresado en KiB en Linux.
 *
 * Referencia:
 * - Linux man-pages, wait4(2):
 *   https://man7.org/linux/man-pages/man2/wait4.2.html
 */

#pragma once

#if !defined(__linux__)
#error "Esta tarea requiere Linux."
#endif

// wait4(2) se declara en glibc cuando están habilitadas sus extensiones.
#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace benchmark {

struct ProcessResult {
    int exitCode{};
    std::uint64_t peakResidentBytes{};
};

inline ProcessResult runAndMeasure(
    const std::string& executable,
    const std::vector<std::string>& arguments
) {
    const pid_t child = fork();
    if (child < 0) {
        throw std::runtime_error("No se pudo crear el proceso de medición.");
    }

    if (child == 0) {
        std::vector<std::string> storage;
        storage.reserve(arguments.size() + 1);
        storage.push_back(executable);
        storage.insert(storage.end(), arguments.begin(), arguments.end());

        std::vector<char*> argv;
        argv.reserve(storage.size() + 1);
        for (auto& item : storage) {
            argv.push_back(item.data());
        }
        argv.push_back(nullptr);

        execv(executable.c_str(), argv.data());
        _exit(127);
    }

    int status = 0;
    struct rusage usage {};
    if (wait4(child, &status, 0, &usage) < 0) {
        throw std::runtime_error("No se pudo esperar el proceso de medición.");
    }

    int exitCode = 1;
    if (WIFEXITED(status)) {
        exitCode = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        exitCode = 128 + WTERMSIG(status);
    }

    const std::uint64_t peakBytes =
        static_cast<std::uint64_t>(usage.ru_maxrss) * 1024ULL;
    return {exitCode, peakBytes};
}

}  // namespace benchmark
