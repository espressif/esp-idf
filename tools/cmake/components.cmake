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

    spaces2list(COMPONENT_SRCS)
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
        add_library(${COMPONENT_TARGET} STATIC ${COMPONENT_SRCS})
        set(include_type PUBLIC)

        set_property(TARGET ${COMPONENT_TARGET} PROPERTY OUTPUT_NAME ${COMPONENT_NAME})

        ldgen_generate_sections_info(${COMPONENT_TARGET})
    else()
        add_library(${COMPONENT_TARGET} INTERFACE) # header-only component
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
        target_add_binary_data("${COMPONENT_TARGET}" "${embed_data}" "${embed_type}")
    endforeach()

    # add component public includes
    foreach(include_dir ${COMPONENT_ADD_INCLUDEDIRS})
        get_filename_component(abs_dir ${include_dir} ABSOLUTE BASE_DIR ${component_dir})
        if(NOT IS_DIRECTORY ${abs_dir})
            message(FATAL_ERROR "${CMAKE_CURRENT_LIST_FILE}: "
                "COMPONENT_ADD_INCLUDEDIRS entry '${include_dir}' not found")
        endif()
        target_include_directories(${COMPONENT_TARGET} ${include_type} ${abs_dir})
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
        target_include_directories(${COMPONENT_TARGET} PRIVATE ${abs_dir})
    endforeach()

    if(${COMPONENT_NAME} IN_LIST BUILD_TEST_COMPONENTS)
        target_link_libraries(${COMPONENT_TARGET} "-L${CMAKE_CURRENT_BINARY_DIR}")
        target_link_libraries(${COMPONENT_TARGET} "-Wl,--whole-archive -l${COMPONENT_NAME} -Wl,--no-whole-archive")
    endif()

    if(COMPONENT_SRCS OR embed_binaries)
        target_include_directories(${COMPONENT_TARGET} PUBLIC ${IDF_INCLUDE_DIRECTORIES})
        target_compile_options(${COMPONENT_TARGET} PUBLIC ${IDF_COMPILE_OPTIONS})
        target_compile_options(${COMPONENT_TARGET} PUBLIC $<$<COMPILE_LANGUAGE:C>:${IDF_C_COMPILE_OPTIONS}>)
        target_compile_options(${COMPONENT_TARGET} PUBLIC $<$<COMPILE_LANGUAGE:CXX>:${IDF_CXX_COMPILE_OPTIONS}>)
        target_compile_definitions(${COMPONENT_TARGET} PUBLIC ${IDF_COMPILE_DEFINITIONS})
    endif()

    if(COMPONENT_ADD_LDFRAGMENTS)
        spaces2list(COMPONENT_ADD_LDFRAGMENTS)
        ldgen_add_fragment_files(${COMPONENT_TARGET} "${COMPONENT_ADD_LDFRAGMENTS}")
    endif()
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

function(require_idf_targets)
    if(NOT ${IDF_TARGET} IN_LIST ARGN)
        message(FATAL_ERROR "Component ${COMPONENT_NAME} only supports targets: ${ARGN}")
    endif()
endfunction()

# component_compile_options
#
# Wrapper around target_compile_options that passes the component name
function(component_compile_options)
    target_compile_options(${COMPONENT_TARGET} PRIVATE ${ARGV})
endfunction()

# component_compile_definitions
#
# Wrapper around target_compile_definitions that passes the component name
function(component_compile_definitions)
    target_compile_definitions(${COMPONENT_TARGET} PRIVATE ${ARGV})
endfunction()

# component_get_target
#
# Get the library target created for the given component
function(component_get_target var component)
    get_property(prefix GLOBAL PROPERTY __IDF_COMPONENTS_PREFIX)
    set(${var} ${prefix}_${component} PARENT_SCOPE)
endfunction()
