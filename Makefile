# Compilers
CXX   =g++-11 -m64
NVCC  =/usr/local/cuda/bin/nvcc


# Flags
CXXFLAGS =-O3 -Wall -g -Iincludes
NVCCFLAGS =-O3 -m64 --gpu-architecture compute_61 -ccbin /usr/bin/g++-11  -Iincludes

# If you need specific GPU arch, uncomment and change:
# NVCCFLAGS += -gencode arch=compute_70,code=sm_70

# CUDA paths (adjust if needed)
CUDA_PATH   ?= /usr/local/cuda
CUDA_INC    = $(CUDA_PATH)/include
CUDA_LIB    = $(CUDA_PATH)/lib64

# Target
TARGET = qr_cuda

# Directories
SRC_DIR = src
INC_DIR = includes

# Sources
CPP_SRCS = main.cpp 
CU_SRCS  =  $(SRC_DIR)/tsqr_cuda.cu   # <-- add your .cu files here

# Objects
CPP_OBJS = $(CPP_SRCS:.cpp=.o)
CU_OBJS  = $(CU_SRCS:.cu=.o)
OBJS     = $(CPP_OBJS) $(CU_OBJS)

# Default rule
all: $(TARGET)

# Link
$(TARGET): $(OBJS)
	$(NVCC) -o $(TARGET) $(OBJS) -L$(CUDA_LIB) -lcudart

# Compile C++
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(CUDA_INC) -c $< -o $@

# Compile CUDA
%.o: %.cu
	$(NVCC) $(NVCCFLAGS) -I$(CUDA_INC) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

###########################################################

# EXECUTABLE := qr_cuda
# LDFLAGS=-L/usr/local/cuda-11.7/lib64/ -lcudart
# CU_FILES   := cudaRenderer.cu
# CU_DEPS    :=
# CC_FILES   := main.cpp display.cpp benchmark.cpp refRenderer.cpp \
#               noise.cpp ppm.cpp sceneLoader.cpp
# LOGS	   := logs

# all: $(EXECUTABLE)

# ###########################################################

# ARCH=$(shell uname | sed -e 's/-.*//g')
# OBJDIR=objs
# CXX=g++-11 -m64
# CXXFLAGS=-O3 -Wall -g
# HOSTNAME=$(shell hostname)

# LIBS       :=
# FRAMEWORKS :=

# NVCCFLAGS=-O3 -m64 --gpu-architecture compute_61 -ccbin /usr/bin/g++-11
# LIBS += GL glut cudart

# LDLIBS  := $(addprefix -l, $(LIBS))
# LDFRAMEWORKS := $(addprefix -framework , $(FRAMEWORKS))

# NVCC=/usr/local/cuda/bin/nvcc

# OBJS=$(OBJDIR)/main.o $(OBJDIR)/display.o $(OBJDIR)/benchmark.o $(OBJDIR)/refRenderer.o \
#      $(OBJDIR)/cudaRenderer.o $(OBJDIR)/noise.o $(OBJDIR)/ppm.o $(OBJDIR)/sceneLoader.o


# .PHONY: dirs clean

# default: $(EXECUTABLE)

# dirs:
# 		mkdir -p $(OBJDIR)/

# clean:
# 		rm -rf $(OBJDIR) *~ $(EXECUTABLE) $(LOGS) *.ppm

# check:	default
# 		./checker.pl

# export: $(EXFILES)
# 	cp -p $(EXFILES) $(STARTER)


# $(EXECUTABLE): dirs $(OBJS)
# 		$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS) $(LDLIBS) $(LDFRAMEWORKS)




# $(OBJDIR)/%.o: %.cpp
# 		$(CXX) $< $(CXXFLAGS) -c -o $@

# $(OBJDIR)/%.o: %.cu
# 		$(NVCC) $< $(NVCCFLAGS) -c -o $@