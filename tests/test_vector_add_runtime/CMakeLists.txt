# Specify CUDA standard
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

find_package(CUDAToolkit REQUIRED)
# Add the CUDA test source
add_executable(test_vector_add_runtime test_vector_add_runtime.cu)
target_link_libraries(test_vector_add_runtime PRIVATE CUDA::cudart)
# Optional: enable all CUDA warnings
target_compile_options(test_vector_add_runtime PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>)

# Register the test (this will run the compiled executable)
add_test(NAME test_vector_add_runtime COMMAND test_vector_add_runtime)
set_tests_properties(test_vector_add_runtime
  PROPERTIES 
  ENVIRONMENT LD_PRELOAD=/home/ubuntu/fan_thesis/fan_master_thesis/build/interception/interception_runtime.so
)
