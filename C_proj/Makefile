CXX = g++
CXXFLAGS = -Wall -W -pedantic -std=c++14 -O3

SRC_DIR = .
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGET = sat_solver.exe

all: $(TARGET)
	del /F /Q $(SRC_DIR)\*.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	del /F /Q $(SRC_DIR)\*.o $(TARGET)

.PHONY: all clean

bench: all
	powershell -ExecutionPolicy Bypass -File .\bench.ps1 -TimeoutSec 300