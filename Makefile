# Compiler settings
CXX = mpic++  # Use MPI C++ compiler wrapper (change if using OpenMPI/MPICH)
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Iincludes -fopenmp -MMD -MP

# Target executable
TARGET = qr_demo

# Source and include directories
SRC_DIR = src
INC_DIR = includes

# Source files
SRCS = main.cpp $(SRC_DIR)/tsqr.cpp $(SRC_DIR)/qr.cpp

# Object files (.o)
OBJS = $(SRCS:.cpp=.o)

# Dependency files (.d)
DEPS = $(OBJS:.o=.d)

# Default target: build the program
all: $(TARGET)

# Link the object files to create the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Compile each .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Include dependency files (if they exist)
-include $(DEPS)

# Clean up object files, dependencies, and the target executable
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

# Phony targets
.PHONY: all clean
