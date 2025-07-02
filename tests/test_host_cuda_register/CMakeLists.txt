# Specify CUDA standard
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# Add the CUDA test source
add_executable(test_host_cuda_register test_host_cuda_register.cpp)

# Optional: enable all CUDA warnings
target_compile_options(test_host_cuda_register PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>)
target_link_libraries(test_host_cuda_register PRIVATE cuda)

# Register the test (this will run the compiled executable)
add_test(NAME test_host_cuda_register COMMAND test_host_cuda_register)
# set_tests_properties(test_pinned_memory_driver
#   PROPERTIES 
#   ENVIRONMENT LD_PRELOAD=${BUILD_DIR}/interception/interception.so
# )