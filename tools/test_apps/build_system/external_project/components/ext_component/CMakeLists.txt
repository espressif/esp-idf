idf_component_register()

include(ExternalProject)
externalproject_add(ext_subproject
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/subproject
    CMAKE_ARGS
        -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
        -DCMAKE_C_FLAGS=-DEXTERNAL_PROJECT_FLAG=1
        -DCMAKE_CXX_FLAGS=-DEXTERNAL_PROJECT_CXX_FLAG=1
        -DCMAKE_ASM_FLAGS=-DEXTERNAL_PROJECT_ASM_FLAG=1
        -DCMAKE_EXE_LINKER_FLAGS=-Wl,--defsym=EXTERNAL_PROJECT_LINKER_CHECK=1
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/subproject"
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/subproject/libext_lib.a"
)

add_prebuilt_library(ext_lib "${CMAKE_CURRENT_BINARY_DIR}/subproject/libext_lib.a")

target_link_libraries(${COMPONENT_LIB} INTERFACE ext_lib)
