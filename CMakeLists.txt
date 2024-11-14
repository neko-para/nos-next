cmake_minimum_required(VERSION 3.22)

project(nos C CXX ASM)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(third_party)

add_subdirectory(kernel)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/compile_commands.json
    COMMAND bash ${CMAKE_SOURCE_DIR}/tools/update_compile_commands.sh
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json)

add_custom_target(UpdateCompileCommands ALL
    DEPENDS ${CMAKE_SOURCE_DIR}/compile_commands.json
)
