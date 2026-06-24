idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs "")
set(includes "include")

# Add target-specific include directory if it exists
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# Per-target regi2c implementation sources (replaces ROM implementation when needed)
if(NOT CONFIG_ESP_ROM_HAS_REGI2C_IMPL)
    list(APPEND srcs "${target}/regi2c_impl.c")
endif()

idf_component_register(
    SRCS ${srcs}
    INCLUDE_DIRS ${includes}
    REQUIRES soc hal
    LDFRAGMENTS linker.lf
)

# Link the ROM REGI2C HAL implementation linker script on specific targets
if(CONFIG_ESP_ROM_HAS_REGI2C_IMPL)
    target_linker_script(${COMPONENT_LIB} INTERFACE "${target}/rom.regi2c.ld")
endif()
