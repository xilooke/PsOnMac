if(APPLE)
    enable_language(C)
    if(NOT CMAKE_OSX_SYSROOT)
        execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE MACOSX_SDK_PATH OUTPUT_STRIP_TRAILING_WHITESPACE)
    else()
        set(MACOSX_SDK_PATH "${CMAKE_OSX_SYSROOT}")
    endif()

    set(MACH_EXC_DEFS "${MACOSX_SDK_PATH}/usr/include/mach/mach_exc.defs")
    set(MACH_EXC_SERVER_C "${CMAKE_CURRENT_BINARY_DIR}/mach_excServer.c")
    set(MACH_EXC_H "${CMAKE_CURRENT_BINARY_DIR}/mach_exc.h")

    add_custom_command(
        OUTPUT "${MACH_EXC_SERVER_C}" "${MACH_EXC_H}"
        COMMAND mig -server "${MACH_EXC_SERVER_C}" -header "${MACH_EXC_H}" "${MACH_EXC_DEFS}"
        DEPENDS "${MACH_EXC_DEFS}"
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
        VERBATIM
    )

    set_source_files_properties("${MACH_EXC_SERVER_C}" PROPERTIES COMPILE_OPTIONS "-Wno-parentheses-equality;-Wno-nullability-completeness")
endif()

add_library(platform_macos STATIC
    src/MachVmMemory.cpp
    src/OsSyncWait.cpp
    src/QosThread.cpp
    src/MachExceptionHandler.cpp
    src/MetalSurface.mm
    ${MACH_EXC_SERVER_C}
)

target_include_directories(platform_macos PUBLIC
    include
)

target_include_directories(platform_macos PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/3rdparty/SDL2/include
)

target_compile_features(platform_macos PUBLIC cxx_std_20)

if(APPLE)
    target_compile_options(platform_macos PRIVATE -fobjc-arc)
    target_link_libraries(platform_macos PUBLIC
        "-framework Foundation"
        "-framework CoreFoundation"
        "-framework Metal"
        "-framework QuartzCore"
    )
endif()

if(BUILD_TESTING AND APPLE)
    add_test_executable(mach_vm_memory_tests tests/MachVmMemoryTests.cpp)
    target_link_libraries(mach_vm_memory_tests PRIVATE platform_macos)
    add_test(NAME mach_vm_memory COMMAND mach_vm_memory_tests)
    set_tests_properties(mach_vm_memory PROPERTIES TIMEOUT 20)

    add_test_executable(os_sync_wait_tests tests/OsSyncWaitTests.cpp)
    target_link_libraries(os_sync_wait_tests PRIVATE platform_macos)
    add_test(NAME os_sync_wait COMMAND os_sync_wait_tests)
    set_tests_properties(os_sync_wait PROPERTIES TIMEOUT 20)

    add_test_executable(qos_thread_tests tests/QosThreadTests.cpp)
    target_link_libraries(qos_thread_tests PRIVATE platform_macos)
    add_test(NAME qos_thread COMMAND qos_thread_tests)
    set_tests_properties(qos_thread PROPERTIES TIMEOUT 20)

    add_test_executable(mach_exception_handler_tests tests/MachExceptionHandlerTests.cpp)
    target_link_libraries(mach_exception_handler_tests PRIVATE platform_macos)
    add_test(NAME mach_exception_handler COMMAND mach_exception_handler_tests)
    set_tests_properties(mach_exception_handler PROPERTIES TIMEOUT 20)
endif()
