set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR x86)

set(tools "/home/nekosu/x-tools/i386-elf")
set(CMAKE_C_COMPILER ${tools}/bin/i386-elf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/i386-elf-g++)
set(CMAKE_ASM_COMPILER ${tools}/bin/i386-elf-as)

set(CMAKE_C_FLAGS "-ffreestanding")
set(CMAKE_CXX_FLAGS "-ffreestanding")
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -T ${CMAKE_CURRENT_LIST_DIR}/linker.ld")
set(CMAKE_EXECUTABLE_SUFFIX ".bin")
