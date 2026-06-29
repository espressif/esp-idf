idf_build_get_property(target IDF_TARGET)
if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs)
set(includes "include")
# Public clock HAL headers include soc/ and hal/ headers directly, and some
# target clk_tree_ll.h headers include hal/regi2c_ctrl.h.
set(requires soc hal esp_hal_regi2c)

# Target-specific include directory if present
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

# TODO(IDF-15947): potentially remove __ULP_BUILD handling from esp_hal_clock
if(__ULP_BUILD)
    # LP-core timer shared code includes hal/clk_tree_ll.h from
    # ulp/lp_core/shared/ulp_lp_core_lp_timer_shared.c. Some target
    # clk_tree_ll.h headers include hal/regi2c_ctrl.h.
    idf_component_register(INCLUDE_DIRS ${includes}
                           REQUIRES ${requires})
    return()
endif()

list(APPEND srcs "${target}/clk_tree_hal.c")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       REQUIRES ${requires})
