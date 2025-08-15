add_executable(firecracker_test firecracker_test.cpp)
target_include_directories(firecracker_test PRIVATE
  /usr/local/cuda-12.6/include
  /usr/local/cuda-12.6/lib64/stubs
)
set_target_properties(firecracker_test PROPERTIES
    BUILD_RPATH "/usr/local/cuda-12.6/lib64/stubs"
)
target_link_libraries(firecracker_test PRIVATE cuda)
# Register the test (this will run the compiled executable)
add_test(NAME firecracker_test COMMAND firecracker_test)
set_tests_properties(firecracker_test
  PROPERTIES 
  ENVIRONMENT LD_PRELOAD=${CMAKE_SOURCE_DIR}/build/interception/interception.so
)