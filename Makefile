PWD := $(shell pwd)
INTERCEPT_TARGET := ./basic_vector_add.cu
CUDA_HOME := /usr/local/cuda/lib64

INTERCEPT := $(PWD)/interception

BUILD_DIR := $(PWD)/build
LOG_DIR := $(PWD)/logs

generate_ptx: kernel_sample.cu
	nvcc -ptx nvcc -ptx kernel_sample.cu -o vector_add.ptx

all: generate_ptx host_sample

generate_grpc: hvcomm.proto
	@protoc --proto_path=$(PWD) \
				 --cpp_out=$(PWD)/grpc_generated \
				 --grpc_out=$(PWD)/grpc_generated \
				 --plugin=protoc-gen-grpc=$(shell which grpc_cpp_plugin) \
				 hvcomm.proto


generate_interception: $(INTERCEPT)/interception.cpp $(INTERCEPT)/interception.h
	g++ -std=c++11 -I/usr/local/cuda/include -shared -fPIC -o interception.so $(INTERCEPT)/interception.cpp -ldl -lcuda

intercept:
	g++ -std=c++11 host_sample.cpp -o $(BUILD_DIR)/intercept_target -I/usr/local/cuda/include -lcuda
# nvcc basic_vector_add_runtime.cu -o intercept_target -lcudart
# nvcc -o intercept_target intercept_target.o -lcudart -L$(CUDA_HOME)/lib64 -lcuda

# @if [[ "$<" == *.cu ]]; then \
# 	echo "Compiling CUDA source with nvcc..."; \
# 	nvcc $< -o $(BUILD_DIR)/intercept_target -lcudart -L$(CUDA_HOME)/lib64 -lcuda; \
# else \
# 	echo "Compiling C++ source with g++..."; \
# 	g++ -std=c++11 $< -o $(BUILD_DIR)/intercept_target -I$(CUDA_HOME)/include -lcuda; \
# fi
	LD_PRELOAD=$(PWD)/interception.so $(BUILD_DIR)/intercept_target > $(LOG_DIR)/logfile.txt

full_intercept: generate_interception intercept