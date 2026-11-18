# INF-221 Algoritmos y Complejidad - 2026-2
# Tarea 1 - Matías Jorquera - Rol 202310127-3

CXX      ?= g++
PYTHON   ?= python3
CPPFLAGS  = -Ialgorithms -I..
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -pedantic
DEPFLAGS  = -MMD -MP

SRC = sorting.cpp \
      algorithms/mergesort.cpp \
      algorithms/quicksort.cpp \
      algorithms/patiencesort.cpp \
      algorithms/sort.cpp
OBJ = $(SRC:.cpp=.o)
DEP = $(OBJ:.o=.d)

TARGET  = sorting
RUN     = ./$(TARGET)

.PHONY: all run execute plots benchmark clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(DEPFLAGS) -c -o $@ $<

run:
	$(MAKE) clean
	$(MAKE) all
	$(MAKE) execute

execute: $(TARGET)
	$(RUN)

plots:
	$(PYTHON) scripts/plot_generator.py

benchmark: run plots

clean:
	rm -f $(OBJ) $(DEP) $(TARGET)

-include $(DEP)
