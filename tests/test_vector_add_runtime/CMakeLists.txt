# Specify CUDA standard
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# Add the CUDA test source
add_executable(test_vector_add_runtime test_vector_add_runtime.cu)

# Optional: enable all CUDA warnings
target_compile_options(test_vector_add_runtime PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>)

# Register the test (this will run the compiled executable)
add_test(NAME test_vector_add_runtime COMMAND test_vector_add_runtime)
set_tests_properties(test_vector_add_runtime
  PROPERTIES 
  ENVIRONMENT LD_PRELOAD=${BUILD_DIR}/interception/interception.so
)
