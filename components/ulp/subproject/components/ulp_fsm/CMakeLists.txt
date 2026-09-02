# What a ULP FSM program is built against.
#
# An FSM program is assembled from the application's own sources, so this
# component carries no sources of its own: it publishes the headers the program
# is written against and the memory layout it is linked with.

idf_build_get_property(target IDF_TARGET)
get_filename_component(ulp_dir "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

idf_component_register(
    INCLUDE_DIRS "${ulp_dir}/ulp_common/include"
                 "${ulp_dir}/ulp_fsm/include"
                 "${ulp_dir}/ulp_fsm/include/${target}"
    REQUIRES soc)

# -D__ASSEMBLER__ keeps the SoC headers to their macro-only form.
set(ulp_ld_flags "-D__ASSEMBLER__ -I\"${CMAKE_CURRENT_SOURCE_DIR}/ld\"")

target_linker_script(${COMPONENT_LIB} INTERFACE "ld/ulp_fsm.ld.in" MEMORY
                     FLAGS "${ulp_ld_flags}")
target_link_options(${COMPONENT_LIB} INTERFACE "SHELL:-u entry")
