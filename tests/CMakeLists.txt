add_executable(host_sample host_sample.cpp)
set_property(TARGET host_sample PROPERTY CUDA_STANDARD 14)
target_link_libraries(host_sample PRIVATE cuda)

add_test(host_sample_build
  "${CMAKE_COMMAND}"
  --build "${CMAKE_BINARY_DIR}"
  --config "$<CONFIG>"
  --target host_sample
)

add_test(NAME host_sample_test COMMAND host_sample)
add_test(NAME preload_host_sample_test COMMAND host_sample)
set_tests_properties(preload_host_sample_test PROPERTIES ENVIRONMENT LD_PRELOAD=${BUILD_DIR}/interception/interception.so)

set_tests_properties(host_sample_build        PROPERTIES FIXTURES_SETUP    test_fixture)
set_tests_properties(preload_host_sample_test PROPERTIES FIXTURES_REQUIRED test_fixture)

file(GLOB children RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *)
foreach(child ${children})
    if(IS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${child})
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${child}/CMakeLists.txt)
            add_subdirectory(${child})
        endif()
    endif()
endforeach()