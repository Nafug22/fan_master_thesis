PWD := $(shell pwd)
INTERCEPT_TARGET := ./basic_vector_add.cu
CUDA_HOME := /usr/local/cuda

CXX_FLAGS := -std=c++17

INTERCEPT := $(PWD)/interception

BUILD_DIR := $(PWD)/build
LOG_DIR := $(PWD)/logs
GRPC_DIR := $(PWD)/grpc_generated
VCPKG_DIR := /home/ubuntu/fan_thesis/vcpkg
PROTOC_GEN_GRPC := /home/ubuntu/fan_thesis/vcpkg/installed/x64-linux/tools/grpc/grpc_cpp_plugin

PROTOC := $(VCPKG_DIR)/installed/x64-linux/tools/protobuf/protoc
generate_ptx: kernel_sample.cu
	nvcc -ptx kernel_sample.cu -o vector_add.ptx

all: generate_ptx host_sample

generate_grpc: hvcomm.proto
	@$(PROTOC) --proto_path=$(PWD) \
					--cpp_out=$(PWD)/grpc_generated \
					--grpc_out=$(PWD)/grpc_generated \
					--plugin=protoc-gen-grpc=$(PROTOC_GEN_GRPC) \
					hvcomm.proto

clean_grpc:
	rm -r $(GRPC_DIR)/*

generate_interception: $(INTERCEPT)/interception.cpp $(INTERCEPT)/interception.h $(INTERCEPT)/compiler.h
	g++ $(CXX_FLAGS) -I/usr/local/cuda/include -shared -fPIC -o interception.so $(INTERCEPT)/interception.cpp -ldl -lcuda

intercept:
	g++ $(CXX_FLAGS) host_sample.cpp -o $(BUILD_DIR)/intercept_target -I/usr/local/cuda/include -lcuda
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