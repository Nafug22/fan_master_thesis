find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(NAME test_basic_pytorch COMMAND conda run -n torch-env ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/pytorch_test.py)
set_tests_properties(test_basic_pytorch
  PROPERTIES 
  ENVIRONMENT LD_PRELOAD=${BUILD_DIR}/interception/interception.so
  ENVIRONMENT LD_LIBRARY_PATH=/usr/lib/gcc/x86_64-linux-gnu/12
)
