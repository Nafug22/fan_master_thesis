add_executable(cuda_server cuda_server.cpp gpu_instance.cpp)
target_link_libraries(cuda_server PRIVATE ${CUDA_DRIVER_LIB} /usr/local/cuda/lib64/libcudart.so)

add_library(interception SHARED interception.cpp interception.h)
target_link_libraries(interception PRIVATE dl ${CUDA_DRIVER_LIB})
set_target_properties(interception PROPERTIES
    PREFIX ""  # No "lib" prefix for the shared library
    OUTPUT_NAME interception  # Output file will be 'interception.so'
)

add_library(interception_runtime SHARED interception_runtime.c)
target_link_libraries(interception_runtime PRIVATE dl ${CUDA_DRIVER_LIB})
set_target_properties(interception_runtime PROPERTIES
    PREFIX ""  # No "lib" prefix for the shared library
    OUTPUT_NAME interception_runtime  # Output file will be 'interception.so'
)
# add_test(interception_build
#   "${CMAKE_COMMAND}"
#   --build "${CMAKE_BINARY_DIR}"
#   --config "$<CONFIG>"
#   --target interception
# )