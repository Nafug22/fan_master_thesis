# Specify CUDA standard
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
# Add the CUDA test source
add_executable(test_pinned_memory_driver test_pinned_memory_driver.cpp)

# Optional: enable all CUDA warnings
target_compile_options(test_pinned_memory_driver PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>)
target_link_libraries(test_pinned_memory_driver PRIVATE cuda)

# Register the test (this will run the compiled executable)
add_test(NAME test_pinned_memory_driver COMMAND test_pinned_memory_driver)
# set_tests_properties(test_pinned_memory_driver
#   PROPERTIES 
#   ENVIRONMENT LD_PRELOAD=${BUILD_DIR}/interception/interception.so
# )