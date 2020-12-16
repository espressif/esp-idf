set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_ASM_COMPILER gcc)

set(CMAKE_C_FLAGS "-Wno-frame-address" CACHE STRING "C Compiler Base Flags")
set(CMAKE_CXX_FLAGS "-Wno-frame-address" CACHE STRING "C++ Compiler Base Flags")
