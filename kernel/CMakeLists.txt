macro(compile_asm ASM_FILE OUTPUT_VAR)
    get_filename_component(ASM_FILE_BASE ${ASM_FILE} NAME)
    set(ASM_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${ASM_FILE_BASE}.o)

    add_custom_command(
        OUTPUT ${ASM_OUTPUT}
        COMMAND ${CMAKE_ASM_COMPILER} ${CMAKE_ASM_FLAGS} ${ASM_FILE} -o ${ASM_OUTPUT}
        DEPENDS ${ASM_FILE}
        COMMENT "Compiling ${ASM_FILE} to ${ASM_OUTPUT}"
    )

    set(${OUTPUT_VAR} ${ASM_OUTPUT})
endmacro()

execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtbegin.o OUTPUT_VARIABLE CRTBEGIN_O OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtend.o OUTPUT_VARIABLE CRTEND_O OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND ${CMAKE_CXX_COMPILER} --print-libgcc OUTPUT_VARIABLE LIBGCC_A OUTPUT_STRIP_TRAILING_WHITESPACE
)

compile_asm(${CMAKE_CURRENT_LIST_DIR}/boot/boot.s BOOT_O)
compile_asm(${CMAKE_CURRENT_LIST_DIR}/boot/crti.s CRTI_O)
compile_asm(${CMAKE_CURRENT_LIST_DIR}/boot/crtn.s CRTN_O)
add_custom_target(kernel_crt DEPENDS ${BOOT_O} ${CRTI_O} ${CRTN_O})

set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_COMPILER} -o <TARGET> ${BOOT_O} ${CRTI_O} ${CRTBEGIN_O} <OBJECTS> <LINK_LIBRARIES> ${LIBGCC_A} ${CRTEND_O} ${CRTN_O} ${CMAKE_EXE_LINKER_FLAGS}")

add_executable(kernel
    main.cpp

    lib/vga.cpp
    lib/io.cpp
    lib/alloc.cpp
    lib/gdt.cpp
    lib/idt.cpp
    lib/isr.cpp
    lib/keyboard.cpp
    lib/timer.cpp
    lib/page.cpp
    lib/task.cpp
    lib/task.s

    std/string.cpp
    std/icxxabi.cpp
)

add_dependencies(kernel kernel_crt)

target_include_directories(kernel PRIVATE .)

target_link_libraries(kernel PRIVATE printf)

set_source_files_properties(lib/isr.cpp PROPERTIES COMPILE_FLAGS -mgeneral-regs-only)
