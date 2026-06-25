# Helpers for link-time optimization (LTO) support.
#
# These helpers are shared between the legacy build system (tools/cmake) and the
# build system v2 (tools/cmakev2). They only rely on the component property API
# (idf_component_get_property) and the COMPONENT_DIR / LDFRAGMENTS component
# properties, which are available in both build systems.

include_guard(GLOBAL)

# __lto_collect_fragment_placed_components(<output_var> <component_name>...)
#
# Returns, in output_var, the subset of the given components whose object code is
# placed by an explicit "archive: libNAME.a" directive in some linker fragment
# (.lf) file - whether that fragment belongs to the component itself or to a
# different component.
#
# Such components rely on section placement that is matched by object file /
# archive name. LTO merges and renames the object files of a component, so the
# placement rules silently stop matching and the affected functions (for example
# the spi_flash / GDMA HAL routines that must execute from IRAM while the flash
# cache is disabled) end up in flash. Compiling these components without LTO keeps
# their placement intact.
#
# Wildcard directives ("archive: *") are intentionally ignored: they match by
# section name rather than by object file name and are therefore not affected by
# LTO.
function(__lto_collect_fragment_placed_components output_var)
    set(placed "")
    foreach(component_name ${ARGN})
        idf_component_get_property(component_dir ${component_name} COMPONENT_DIR)
        idf_component_get_property(ldfragments ${component_name} LDFRAGMENTS)
        foreach(ldfragment ${ldfragments})
            get_filename_component(ldfragment_abs "${ldfragment}" ABSOLUTE BASE_DIR "${component_dir}")
            if(EXISTS "${ldfragment_abs}")
                file(READ "${ldfragment_abs}" ldfragment_contents)
                string(REGEX MATCHALL "archive:[ \t]*lib[A-Za-z0-9_-]+\\.a"
                       archive_matches "${ldfragment_contents}")
                foreach(archive_match ${archive_matches})
                    string(REGEX REPLACE "archive:[ \t]*lib([A-Za-z0-9_-]+)\\.a" "\\1"
                           archive_name "${archive_match}")
                    list(APPEND placed ${archive_name})
                endforeach()
            endif()
        endforeach()
    endforeach()
    if(placed)
        list(REMOVE_DUPLICATES placed)
    endif()
    set(${output_var} "${placed}" PARENT_SCOPE)
endfunction()
