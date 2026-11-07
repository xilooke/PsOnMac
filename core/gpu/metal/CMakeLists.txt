add_library(gpu_metal STATIC
    src/MetalFormatMap.mm
    src/MetalBuffer.mm
    src/MetalTexture.mm
    src/MetalSampler.mm
    src/MetalShaderModule.mm
    src/MetalPipeline.mm
    src/MetalPipelineCache.mm
    src/MetalFence.mm
    src/MetalDescriptorTable.mm
    src/MetalCommandRecorder.mm
    src/MetalQueue.mm
    src/MetalSwapchain.mm
    src/MetalDevice.mm
)

target_include_directories(gpu_metal PUBLIC
    include
)

target_compile_features(gpu_metal PUBLIC cxx_std_20)

if(APPLE)
    target_compile_options(gpu_metal PRIVATE -fobjc-arc)
    target_link_libraries(gpu_metal PUBLIC
        gpu_rhi
        "-framework Metal"
        "-framework QuartzCore"
        "-framework Foundation"
        "-framework AppKit"
    )
endif()

if(BUILD_TESTING AND APPLE)
    add_test_executable(metal_device_tests tests/MetalDeviceTests.mm)
    target_compile_options(metal_device_tests PRIVATE -fobjc-arc)
    target_link_libraries(metal_device_tests PRIVATE gpu_metal)
    add_test(NAME metal_device COMMAND metal_device_tests)
    set_tests_properties(metal_device PROPERTIES TIMEOUT 30)

    add_test_executable(metal_resource_tests tests/MetalResourceTests.mm)
    target_compile_options(metal_resource_tests PRIVATE -fobjc-arc)
    target_link_libraries(metal_resource_tests PRIVATE gpu_metal)
    add_test(NAME metal_resource COMMAND metal_resource_tests)
    set_tests_properties(metal_resource PROPERTIES TIMEOUT 30)

    add_test_executable(metal_execution_tests tests/MetalExecutionTests.mm)
    target_compile_options(metal_execution_tests PRIVATE -fobjc-arc)
    target_link_libraries(metal_execution_tests PRIVATE gpu_metal)
    add_test(NAME metal_execution COMMAND metal_execution_tests)
    set_tests_properties(metal_execution PROPERTIES TIMEOUT 30)
endif()
