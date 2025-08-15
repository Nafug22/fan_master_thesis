add_executable(firecracker_test firecracker_test.cpp)
target_include_directories(interception PRIVATE
  /usr/local/cuda-12.6/include
)
target_include_directories(interception PRIVATE
  /usr/local/cuda-12.6/lib64/stubs
)
target_link_libraries(interception PRIVATE 
  dl cuda
)
# Register the test (this will run the compiled executable)
add_test(NAME firecracker_test COMMAND firecracker_test)
set_tests_properties(firecracker_test
  PROPERTIES 
  ENVIRONMENT LD_PRELOAD=${CMAKE_SOURCE_DIR}/build/interception/interception.so
)