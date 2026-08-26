idf_component_register()

include(ExternalProject)
externalproject_add(subproject
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/subproject
    CMAKE_ARGS -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/subproject"
    INSTALL_COMMAND ""
    BUILD_BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/subproject/libprebuilt.a"
)

add_prebuilt_library(prebuilt "${CMAKE_CURRENT_BINARY_DIR}/subproject/libprebuilt.a")

target_link_libraries(${COMPONENT_LIB} INTERFACE prebuilt)
