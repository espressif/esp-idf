include($ENV{IDF_PATH}/tools/cmake/utilities.cmake)

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER xtensa-esp32-elf-gcc)
set(CMAKE_CXX_COMPILER xtensa-esp32-elf-g++)
set(CMAKE_ASM_COMPILER xtensa-esp32-elf-gcc)
set(_CMAKE_TOOLCHAIN_PREFIX xtensa-esp32-elf-)

remove_duplicated_flags("-mlongcalls -Wno-frame-address \
                         -fno-builtin-memcpy -fno-builtin-memset -fno-builtin-bzero \
                         -fno-builtin-stpcpy -fno-builtin-strncpy \
                         ${CMAKE_C_FLAGS}" UNIQ_CMAKE_C_FLAGS)
set(CMAKE_C_FLAGS "${UNIQ_CMAKE_C_FLAGS}" CACHE STRING "C Compiler Base Flags" FORCE)
remove_duplicated_flags("-mlongcalls -Wno-frame-address \
                         -fno-builtin-memcpy -fno-builtin-memset -fno-builtin-bzero \
                         -fno-builtin-stpcpy -fno-builtin-strncpy \
                         ${CMAKE_CXX_FLAGS}" UNIQ_CMAKE_CXX_FLAGS)
set(CMAKE_CXX_FLAGS "${UNIQ_CMAKE_CXX_FLAGS}" CACHE STRING "C++ Compiler Base Flags" FORCE)
remove_duplicated_flags("-mlongcalls ${CMAKE_ASM_FLAGS}" UNIQ_CMAKE_ASM_FLAGS)
set(CMAKE_ASM_FLAGS "${UNIQ_CMAKE_ASM_FLAGS}" CACHE STRING "ASM Compiler Base Flags" FORCE)
