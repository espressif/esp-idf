# Given a list of components in 'component_paths', filter only paths to the components
# mentioned in 'components' and return as a list in 'result_paths'
function(components_get_paths component_paths components result_paths)
    set(result "")
    foreach(path ${component_paths})
        get_filename_component(name "${path}" NAME)
        if("${name}" IN_LIST components)
            list(APPEND result "${name}")
        endif()
    endforeach()
    set("${result_path}" "${result}" PARENT_SCOPE)
endfunction()

# Add a component to the build, using the COMPONENT variables defined
# in the parent
#
function(register_component)
    get_filename_component(component_dir ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
    get_filename_component(component ${component_dir} NAME)

    spaces2list(COMPONENT_SRCDIRS)
    spaces2list(COMPONENT_ADD_INCLUDEDIRS)
    spaces2list(COMPONENT_SRCEXCLUDE)

    if(COMPONENT_SRCDIRS)
        # Warn user if both COMPONENT_SRCDIRS and COMPONENT_SRCS are set
        if(COMPONENT_SRCS)
            message(WARNING "COMPONENT_SRCDIRS and COMPONENT_SRCS are both set, COMPONENT_SRCS will be ignored")
        endif()

        set(COMPONENT_SRCS "")

        foreach(dir ${COMPONENT_SRCDIRS})
            get_filename_component(abs_dir ${dir} ABSOLUTE BASE_DIR ${component_dir})
            if(NOT IS_DIRECTORY ${abs_dir})
                message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: COMPONENT_SRCDIRS entry '${dir}' does not exist")
            endif()

            file(GLOB matches "${abs_dir}/*.c" "${abs_dir}/*.cpp" "${abs_dir}/*.S")
            if(matches)
                list(SORT matches)
                set(COMPONENT_SRCS "${COMPONENT_SRCS};${matches}")
            else()
                message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: COMPONENT_SRCDIRS entry '${dir}' has no source files")
            endif()
        endforeach()
    endif()

    # Remove COMPONENT_SRCEXCLUDE matches
    foreach(exclude ${COMPONENT_SRCEXCLUDE})
        get_filename_component(exclude "${exclude}" ABSOLUTE ${component_dir})
        foreach(src ${COMPONENT_SRCS})
            get_filename_component(abs_src "${src}" ABSOLUTE ${component_dir})
            if("${exclude}" STREQUAL "${abs_src}")  # compare as canonical paths
                list(REMOVE_ITEM COMPONENT_SRCS "${src}")
            endif()
        endforeach()
    endforeach()

    # add as a PUBLIC library (if there are source files) or INTERFACE (if header only)
    if(COMPONENT_SRCS OR embed_binaries)
        add_library(${component} STATIC ${COMPONENT_SRCS})
        set(include_type PUBLIC)
    else()
        add_library(${component} INTERFACE) # header-only component
        set(include_type INTERFACE)
    endif()

    # binaries to embed directly in library
    spaces2list(COMPONENT_EMBED_FILES)
    spaces2list(COMPONENT_EMBED_TXTFILES)
    foreach(embed_data ${COMPONENT_EMBED_FILES} ${COMPONENT_EMBED_TXTFILES})
        if(embed_data IN_LIST COMPONENT_EMBED_TXTFILES)
            set(embed_type "TEXT")
        else()
            set(embed_type "BINARY")
        endif()
        target_add_binary_data("${component}" "${embed_data}" "${embed_type}")
    endforeach()

    # add component public includes
    foreach(include_dir ${COMPONENT_ADD_INCLUDEDIRS})
        get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${component_dir})
        if(NOT IS_DIRECTORY ${abs_dir})
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: "
                "COMPONENT_ADD_INCLUDEDIRS entry '${include_dir}' not found")
        endif()
        target_include_directories(${component} ${include_type} ${abs_dir})
    endforeach()

    # add component private includes
    foreach(include_dir ${COMPONENT_PRIV_INCLUDEDIRS})
        if(${include_type} STREQUAL INTERFACE)
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE} "
                "sets no component source files but sets COMPONENT_PRIV_INCLUDEDIRS")
        endif()

        get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${component_dir})
        if(NOT IS_DIRECTORY ${abs_dir})
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: "
                "COMPONENT_PRIV_INCLUDEDIRS entry '${include_dir}' does not exist")
        endif()
        target_include_directories(${component} PRIVATE ${abs_dir})
    endforeach()
endfunction()

function(register_config_only_component)
    get_filename_component(component_dir ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
    get_filename_component(component ${component_dir} NAME)

    # No-op for now...
endfunction()

function(add_component_dependencies target dep dep_type)
    get_target_property(target_type ${target} TYPE)
    get_target_property(target_imported ${target} IMPORTED)

    if(${target_type} STREQUAL STATIC_LIBRARY OR ${target_type} STREQUAL EXECUTABLE)
        if(TARGET ${dep})
            # Add all compile options exported by dep into target
            target_include_directories(${target} ${dep_type}
                $<TARGET_PROPERTY:${dep},INTERFACE_INCLUDE_DIRECTORIES>)
            target_compile_definitions(${target} ${dep_type}
                $<TARGET_PROPERTY:${dep},INTERFACE_COMPILE_DEFINITIONS>)
            target_compile_options(${target} ${dep_type}
                $<TARGET_PROPERTY:${dep},INTERFACE_COMPILE_OPTIONS>)
        endif()
    endif()
endfunction()

function(components_finish_registration)

    # have the executable target depend on all components in the build
    set_target_properties(${CMAKE_PROJECT_NAME}.elf PROPERTIES INTERFACE_COMPONENT_REQUIRES "${BUILD_COMPONENTS}")

    spaces2list(COMPONENT_REQUIRES_COMMON)

    # each component should see the include directories of its requirements
    #
    # (we can't do this until all components are registered and targets exist in cmake, as we have
    # a circular requirements graph...)
    foreach(a ${BUILD_COMPONENTS})
        if(TARGET ${a})
            get_component_requirements("${a}" a_deps a_priv_deps)
            list(APPEND a_priv_deps ${COMPONENT_REQUIRES_COMMON})
            foreach(b ${a_deps})
                add_component_dependencies(${a} ${b} PUBLIC)
            endforeach()

            foreach(b ${a_priv_deps})
                add_component_dependencies(${a} ${b} PRIVATE)
            endforeach()

            get_target_property(a_type ${a} TYPE)
            if(${a_type} MATCHES .+_LIBRARY)
                set(COMPONENT_LIBRARIES "${COMPONENT_LIBRARIES};${a}")
            endif()
        endif()
    endforeach()

    # Add each component library's link-time dependencies (which are otherwise ignored) to the executable
    # LINK_DEPENDS in order to trigger a re-link when needed (on Ninja/Makefile generators at least).
    # (maybe this should probably be something CMake does, but it doesn't do it...)
    foreach(component ${BUILD_COMPONENTS})
        if(TARGET ${component})
            get_target_property(imported ${component} IMPORTED)
            get_target_property(type ${component} TYPE)
            if(NOT imported)
                if(${type} STREQUAL STATIC_LIBRARY OR ${type} STREQUAL EXECUTABLE)
                    get_target_property(link_depends "${component}" LINK_DEPENDS)
                    if(link_depends)
                        set_property(TARGET ${CMAKE_PROJECT_NAME}.elf APPEND PROPERTY LINK_DEPENDS "${link_depends}")
                    endif()
                endif()
            endif()
        endif()
    endforeach()

    target_link_libraries(${CMAKE_PROJECT_NAME}.elf ${COMPONENT_LIBRARIES})

    message(STATUS "Component libraries: ${COMPONENT_LIBRARIES}")

endfunction()
