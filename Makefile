CXX = g++
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Iincludes

TARGET = qr_demo

SRC_DIR = src
INC_DIR = includes

SRCS = main.cpp $(SRC_DIR)/tsqr.cpp $(SRC_DIR)/qr.cpp
# SRCS = main.cpp $(SRC_DIR)/qr.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
