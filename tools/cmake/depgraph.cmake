# Component dependency graph generation helpers.
# To enable this functionality, add:
#   idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)
# in the project CMakeLists.txt file between include(project.cmake) and project(name) calls.
#
# Graphviz file component_deps.dot will be produced in the build directory.
# To change the template, see component_deps.dot.in.

# depgraph_add_edge
#
# @brief Record an edge in the component dependency graph (dependent -> dependency)
#
# @param[in] dep_from dependent name
# @param[in] dep_to dependency name
# @param[in, optional] REQUIRES if given, record this as "REQUIRES" (public/interface) dependency
# @param[in, optional] PRIV_REQUIRES if given, record this as "PRIV_REQUIRES" (private) dependency
#
function(depgraph_add_edge dep_from dep_to)
    cmake_parse_arguments(_ "REQUIRES;PRIV_REQUIRES" "" "" ${ARGN})
    idf_build_get_property(depgraph_enabled __BUILD_COMPONENT_DEPGRAPH_ENABLED)
    if(NOT depgraph_enabled)
        return()
    endif()
    idf_build_get_property(common_reqs __COMPONENT_REQUIRES_COMMON)

    set(attr)
    if(__REQUIRES)
        set(attr "[class=\"requires\"]")
    elseif(__PRIV_REQUIRES)
        set(attr "[class=\"priv_requires\" style=\"dotted\"]")
    endif()

    if(dep_to IN_LIST common_reqs)
        # Don't record graph edges leading to "common" components, since every component has these dependencies.
        # However, show which components are "common" by adding an edge from a node named "common".
        # If necessary, add a new build property to customize this behavior.
        if(NOT dep_from IN_LIST common_reqs)
            idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH "common -> ${dep_to}" APPEND)
        endif()
    else()
        idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH "${dep_from} -> ${dep_to} ${attr}" APPEND)
    endif()
endfunction()

# depgraph_generate
#
# @brief Write the collected component dependency graph to a file. The file is in Graphviz (.dot) format.
#
# @param[in] out_path  name of the output file
#
function(depgraph_generate out_path)
    idf_build_get_property(depgraph_enabled __BUILD_COMPONENT_DEPGRAPH_ENABLED)
    if(NOT depgraph_enabled)
        return()
    endif()
    idf_build_get_property(depgraph __BUILD_COMPONENT_DEPGRAPH)
    list(REMOVE_DUPLICATES depgraph)
    string(REPLACE ";" ";\n" depgraph "${depgraph}")
    configure_file("${IDF_PATH}/tools/cmake/component_deps.dot.in"
        "${out_path}")
endfunction()
