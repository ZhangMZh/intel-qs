################################################################################

add_executable(hello_world.exe hello_world.cpp)
target_link_libraries(hello_world.exe PRIVATE iqs)

add_executable(test1qubitgate.exe test1qubitgate.cpp)
target_link_libraries(test1qubitgate.exe PRIVATE iqs)

################################################################################

set_target_properties( hello_world.exe
                       test1qubitgate.exe
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
)

################################################################################
