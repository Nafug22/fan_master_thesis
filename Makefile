PWD := $(shell pwd)
INTERCEPT_TARGET := ./basic_vector_add.cu
CUDA_HOME := /usr/local/cuda/lib64

BUILD_DIR := $(PWD)/build
LOG_DIR := $(PWD)/logs

generate_ptx: kernel_sample.cu
	nvcc -ptx nvcc -ptx kernel_sample.cu -o vector_add.ptx

# host_sample: host_sample.cpp
# g++ -I/usr/local/cuda/include host_sample.cpp -o host_sample -L/usr/local/cuda/lib64 -lcuda

all: generate_ptx host_sample

generate_grpc: hvcomm.proto
	@protoc --proto_path=$(PWD) \
				 --cpp_out=$(PWD)/grpc_generated \
				 --grpc_out=$(PWD)/grpc_generated \
				 --plugin=protoc-gen-grpc=$(shell which grpc_cpp_plugin) \
				 hvcomm.proto


generate_interception: interception.cpp interception.h
	g++ -std=c++11 -I/usr/local/cuda/include -shared -fPIC -o interception.so interception.cpp -ldl -lcuda

intercept:
# nvcc basic_vector_add_runtime.cu -o intercept_target -lcudart
	g++ -std=c++11 host_sample.cpp -o $(BUILD_DIR)/intercept_target -I/usr/local/cuda/include -lcuda
# nvcc -o intercept_target intercept_target.o -lcudart -L$(CUDA_HOME)/lib64 -lcuda

	LD_PRELOAD=$(PWD)/interception.so $(BUILD_DIR)/intercept_target > $(LOG_DIR)/logfile.txt

full_intercept: generate_interception intercept