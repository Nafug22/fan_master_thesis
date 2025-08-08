# --- PREDEFINE CUDA::nvToolsExt so Torch's cmake doesn't break ---
find_library(NVTOOLSEXT_LIBRARY nvToolsExt
    PATHS /usr/local/cuda/lib64 /usr/lib/x86_64-linux-gnu
)
# if (NVTOOLSEXT_LIBRARY)
#     message(STATUS "Found nvToolsExt at ${NVTOOLSEXT_LIBRARY}")
#     add_library(CUDA::nvToolsExt UNKNOWN IMPORTED)
#     set_target_properties(CUDA::nvToolsExt PROPERTIES
#         IMPORTED_LOCATION "${NVTOOLSEXT_LIBRARY}"
#         INTERFACE_INCLUDE_DIRECTORIES "/usr/local/cuda/include"
#     )
# else()
#     message(FATAL_ERROR "nvToolsExt not found — check your CUDA installation.")
# endif()

# Path to your CUDA-enabled LibTorch
set(CMAKE_PREFIX_PATH "/home/ubuntu/libtorch")

# Now find Torch — safe because CUDA::nvToolsExt exists
find_package(Torch REQUIRED)

# Create your executable
add_executable(torch_test torch_test.cpp)
target_link_libraries(torch_test "${TORCH_LIBRARIES}")

# ABI setting for LibTorch compatibility
target_compile_definitions(torch_test PRIVATE -D_GLIBCXX_USE_CXX11_ABI=1)

# Optional: embed RPATH to avoid LD_LIBRARY_PATH
set_target_properties(torch_test PROPERTIES
    BUILD_RPATH "${CMAKE_PREFIX_PATH}/lib"
    INSTALL_RPATH "${CMAKE_PREFIX_PATH}/lib"
)
