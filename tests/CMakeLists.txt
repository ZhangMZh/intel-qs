################################################################################

add_executable(hello_world.exe hello_world.cpp)
target_link_libraries(hello_world.exe PRIVATE iqs)

################################################################################

set_target_properties( hello_world.cpp.exe
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

################################################################################
