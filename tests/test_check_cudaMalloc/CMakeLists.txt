# Specify CUDA standard
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# Add the CUDA test source
add_executable(test_check_cudaMalloc check_cudaMalloc.cu)

# Optional: enable all CUDA warnings
target_compile_options(test_check_cudaMalloc PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>)

# Register the test (this will run the compiled executable)
add_test(NAME test_check_cudaMalloc COMMAND test_check_cudaMalloc)
set_tests_properties(test_check_cudaMalloc
  PROPERTIES 
  ENVIRONMENT LD_PRELOAD=${BUILD_DIR}/interception/interception.so
)
