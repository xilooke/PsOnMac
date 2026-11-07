add_library(gpu_rhi STATIC
    src/GpuCommandStream.cpp
)

target_include_directories(gpu_rhi PUBLIC
    include
)

target_compile_features(gpu_rhi PUBLIC cxx_std_20)

if(BUILD_TESTING)
    add_test_executable(rhi_types_tests tests/RhiTypesTests.cpp)
    target_link_libraries(rhi_types_tests PRIVATE gpu_rhi)
    add_test(NAME rhi_types COMMAND rhi_types_tests)
    set_tests_properties(rhi_types PROPERTIES TIMEOUT 20)

    add_test_executable(rhi_interface_tests tests/RhiInterfaceTests.cpp)
    target_link_libraries(rhi_interface_tests PRIVATE gpu_rhi)
    add_test(NAME rhi_interface COMMAND rhi_interface_tests)
    set_tests_properties(rhi_interface PROPERTIES TIMEOUT 20)

    add_test_executable(gpu_command_tests tests/GpuCommandTests.cpp)
    target_link_libraries(gpu_command_tests PRIVATE gpu_rhi)
    add_test(NAME gpu_command COMMAND gpu_command_tests)
    set_tests_properties(gpu_command PROPERTIES TIMEOUT 20)
endif()
