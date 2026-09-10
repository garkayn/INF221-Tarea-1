/*
 * INF-221 Algoritmos y Complejidad - 2026-2
 * Tarea 1 - Matías Jorquera - Rol 202310127-3
 *
 * Patience Sort mediante búsqueda binaria de pilas y mezcla con min-heap.
 * Las pilas se almacenan como nodos enlazados en un vector para evitar una
 * asignación dinámica independiente por cada pila.
 * Referencia original: C. L. Mallows, "Patience Sorting", SIAM Review,
 * vol. 4, n. 2, pp. 148-149, 1962. DOI: 10.1137/1004036.
 * https://doi.org/10.1137/1004036
 */

#include "sorting_algorithms.hpp"

#include <algorithm>
#include <climits>
#include <cstddef>
#include <queue>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

struct Node {
    int value;
    int previous;
};

struct NodeGreater {
    const std::vector<Node>* nodes;

    bool operator()(const int left, const int right) const {
        return (*nodes)[static_cast<std::size_t>(left)].value >
               (*nodes)[static_cast<std::size_t>(right)].value;
    }
};

}

void patienceSort(std::vector<int>& values) {
    if (values.size() > static_cast<std::size_t>(INT_MAX)) {
        throw std::length_error("Patience Sort excede el rango de índices.");
    }

    std::vector<Node> nodes;
    std::vector<int> pileTopValues;
    std::vector<int> pileTopNodes;
    nodes.reserve(values.size());

    for (const int value : values) {
        const auto position = std::lower_bound(
            pileTopValues.begin(), pileTopValues.end(), value
        );
        const std::size_t pile = static_cast<std::size_t>(
            std::distance(pileTopValues.begin(), position)
        );
        const int previous = pile == pileTopNodes.size()
            ? -1
            : pileTopNodes[pile];
        const int nodeIndex = static_cast<int>(nodes.size());
        nodes.push_back({value, previous});

        if (pile == pileTopValues.size()) {
            pileTopValues.push_back(value);
            pileTopNodes.push_back(nodeIndex);
        } else {
            pileTopValues[pile] = value;
            pileTopNodes[pile] = nodeIndex;
        }
    }

    std::vector<int> heapStorage = std::move(pileTopNodes);
    std::priority_queue<int, std::vector<int>, NodeGreater> minimums(
        NodeGreater{&nodes}, std::move(heapStorage)
    );
    std::vector<int>().swap(pileTopValues);

    std::size_t output = 0;
    while (!minimums.empty()) {
        const int nodeIndex = minimums.top();
        minimums.pop();

        const Node& node = nodes[static_cast<std::size_t>(nodeIndex)];
        values[output++] = node.value;
        if (node.previous >= 0) {
            minimums.push(node.previous);
        }
    }
}
