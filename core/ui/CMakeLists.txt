add_executable(anyps5-ui
    src/Main.mm
    src/LauncherUi.mm
)

target_include_directories(anyps5-ui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(anyps5-ui PRIVATE
    -fobjc-arc
)

target_link_libraries(anyps5-ui PRIVATE
    "-framework Cocoa"
    "-framework Metal"
    "-framework QuartzCore"
    "-framework GameController"
)

set_target_properties(anyps5-ui PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

add_custom_command(TARGET anyps5-ui POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink
        $<TARGET_FILE:anyps5-ui>
        ${CMAKE_BINARY_DIR}/anyps5-ui
)
