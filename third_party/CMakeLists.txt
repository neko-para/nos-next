add_library(printf STATIC printf/printf.c)
target_compile_definitions(printf PRIVATE PRINTF_INCLUDE_CONFIG_H)
target_include_directories(printf INTERFACE ${CMAKE_CURRENT_LIST_DIR} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/config)
