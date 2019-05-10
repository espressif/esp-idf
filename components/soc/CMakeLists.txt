idf_build_get_property(soc_name IDF_TARGET)

if(EXISTS "${COMPONENT_DIR}/${soc_name}")
    include(${COMPONENT_DIR}/${soc_name}/sources.cmake)

    spaces2list(SOC_SRCS)
    add_prefix(COMPONENT_SRCS "${soc_name}/" ${SOC_SRCS})
    set(COMPONENT_ADD_INCLUDEDIRS ${soc_name}/include)
endif()

list(APPEND COMPONENT_ADD_INCLUDEDIRS include)
list(APPEND COMPONENT_SRCS "src/memory_layout_utils.c src/lldesc.c src/hal/spi_hal.c src/hal/spi_hal_iram.c src/soc_include_legacy_warn.c")

set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

set(COMPONENT_REQUIRES)

register_component()
