# Utilities for supporting linker script generation in the build system

# __ldgen_add_fragment_files
#
# Add one or more linker fragment files, and append it to the list of fragment
# files found so far.
function(__ldgen_add_fragment_files fragment_files)
    spaces2list(fragment_files)

    foreach(fragment_file ${fragment_files})
        get_filename_component(abs_path ${fragment_file} ABSOLUTE)
        list(APPEND _fragment_files ${abs_path})
    endforeach()

    idf_build_set_property(__LDGEN_FRAGMENT_FILES "${_fragment_files}" APPEND)
endfunction()

# __ldgen_process_template
#
# Passes a linker script template to the linker script generation tool for
# processing. This internal function is called from the public "target_linker_script" function.
# It simply records the template and output file names. The actual linker script generation
# target is created later by the __ldgen_create_target function.
#
function(__ldgen_process_template template output)
    idf_build_set_property(__LDGEN_TEMPLATES "${template}" APPEND)
    idf_build_set_property(__LDGEN_OUTPUTS "${output}" APPEND)
endfunction()

# __ldgen_get_lib_deps_of_target
#
# Helper function to get the list of libraries that a target depends on,
# recursively, and append it to the out_list. The 'target' argument is
# typically an executable.
#
# TODO: In CMake 4.1 and later, LINK_LIBRARIES property is evaluated recursively,
# so this logic can be skipped if we are running a recent-enough CMake.
# https://gitlab.kitware.com/cmake/cmake/-/commit/b3da9c6d6054711c5075cc3405fb95aa065ea014
#
function(__ldgen_get_lib_deps_of_target target out_list_var)
    set(out_list ${${out_list_var}})
    if(target IN_LIST out_list)
        # This target is already in the list, meaning that it has been
        # processed already. Bail out.
        return()
    endif()

    # The target is not in the list yet. Add it, and then add all its dependencies.
    list(APPEND out_list ${target})
    set(${out_list_var} ${out_list} PARENT_SCOPE)

    get_target_property(lib_type ${target} TYPE)
    set(library_prop_name "LINK_LIBRARIES")
    if(lib_type STREQUAL "INTERFACE_LIBRARY")
        # CMake didn't allow setting or querying non-whitelisted properties on
        # INTERFACE_LIBRARY targets until version 3.19:
        # https://gitlab.kitware.com/cmake/cmake/-/commit/afb998704e67d3d3ce5b24c112cb06e770fca78d
        set(library_prop_name "INTERFACE_LINK_LIBRARIES")
    endif()

    get_target_property(target_deps ${target} ${library_prop_name})
    if(NOT target_deps)
        # This target has no dependencies, nothing to do.
        return()
    endif()

    foreach(dep ${target_deps})
        if(dep IN_LIST out_list)
            # This dependency has already been processed, skip it.
            continue()
        endif()

        if(NOT TARGET ${dep})
            # LINK_LIBRARIES may contain various non-library-related linker flags
            # (-u, -L, -l, etc.), skip them.
            if(dep MATCHES "^-" OR dep MATCHES "^\:\:")
                continue()
            endif()

            # If the dependency is not a target, it may be a library. Add it to the list.
            list(APPEND out_list ${dep})
        else()
            # Recursively add the dependencies of this target.
            __ldgen_get_lib_deps_of_target(${dep} out_list)
        endif()
    endforeach()

    set(${out_list_var} ${out_list} PARENT_SCOPE)
endfunction()

# __ldgen_get_mutable_libs
#
# Helper function to get the list of library file name generator expressions
# for project components. These libraries are placed by ldgen into a separate
# location in the linker script, allowing the fast reflashing feature.
#
function(__ldgen_get_mutable_libs out_list_var)
    set(mutable_libs)
    idf_build_get_property(build_component_targets __BUILD_COMPONENT_TARGETS)
    foreach(component_target ${build_component_targets})
        __component_get_property(component_source ${component_target} COMPONENT_SOURCE)
        __component_get_property(component_lib ${component_target} COMPONENT_LIB)
        __component_get_property(component_type ${component_target} COMPONENT_TYPE)
        if("${component_type}" STREQUAL "CONFIG_ONLY")
            # Configuration only component interface target without a library.
            continue()
        endif()
        if(NOT "${component_source}" STREQUAL "project_components")
            # Add only project components as mutable.
            continue()
        endif()
        set(unstable_lib "$<TARGET_LINKER_FILE_NAME:${component_lib}>")
        list(APPEND mutable_libs "${unstable_lib}")
    endforeach()
    set(${out_list_var} ${mutable_libs} PARENT_SCOPE)
endfunction()

# __ldgen_create_target
#
# Internal function which creates a custom target for the linker script generation tool.
# This function is called from the public idf_build_executable function.
#
function(__ldgen_create_target exe_target)
    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(templates __LDGEN_TEMPLATES)
    idf_build_get_property(outputs __LDGEN_OUTPUTS)

    if(NOT templates)
        # No templates were passed to ldgen, nothing to do.
        return()
    endif()

    list(LENGTH templates num_templates)
    if(NOT num_templates EQUAL 1)
        # This limitation can be removed, if necessary, by looping over the list of templates
        # and creating a target for each one of them.
        # However, this is not needed in IDF for now, hence the simpler implementation.
        message(FATAL_ERROR "Only one template file can be passed to __ldgen_process, "
                            "got ${num_templates}: ${templates}")
    endif()

    list(GET templates 0 template)
    list(GET outputs 0 output)

    # Collect all the libraries that the executable depends on, recursively.
    # This is needed to pass the list of libraries to the linker script generator tool.
    set(ldgen_libraries)
    __ldgen_get_lib_deps_of_target(${exe_target} ldgen_libraries)
    list(REMOVE_ITEM ldgen_libraries ${exe_target})
    set(ldgen_deps)
    foreach(lib ${ldgen_libraries})
        if(TARGET ${lib})
            get_target_property(lib_type ${lib} TYPE)
            if(lib_type STREQUAL "INTERFACE_LIBRARY")
                continue()
            endif()
            list(APPEND ldgen_libraries_expr "$<TARGET_FILE:${lib}>")
            list(APPEND ldgen_deps ${lib})
        else()
            # Here we have two cases:
            #
            # 1. ${lib} is actually a target, but the target is outside the current scope.
            #    This is the case for imported library targets which are not defined as GLOBAL.
            #    Libraries added using `add_prebuilt_library` are marked as GLOBAL, so they should
            #    be handled in the if block above.
            #
            # 2. ${lib} is the name of a library, which the linker can find in its built-in
            #    or specified search paths.
            #    This is the case for toolchain libraries (m, c, gcc, stdc++), as well as
            #    any prebuilt libraries which may have been added using `-lname -Lpath`
            #    style flags.
            #
            # If we can successfully find the absolute path of each such library, this
            # will allow us to pass them to ldgen, enabling us to place functions from
            # prebuilt libraries to specific sections in the linker script (IDF-12049)
        endif()
    endforeach()
    list(JOIN ldgen_libraries_expr "\n" ldgen_libraries_str)

    idf_build_get_property(build_dir BUILD_DIR)
    file(WRITE ${build_dir}/ldgen_libraries.in "${ldgen_libraries_str}")
    file(GENERATE OUTPUT ${build_dir}/ldgen_libraries INPUT ${build_dir}/ldgen_libraries.in)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        "${build_dir}/ldgen_libraries.in"
        "${build_dir}/ldgen_libraries")

    idf_build_get_property(ldgen_fragment_files __LDGEN_FRAGMENT_FILES GENERATOR_EXPRESSION)

    # Create command to invoke the linker script generator tool.
    idf_build_get_property(sdkconfig SDKCONFIG)
    idf_build_get_property(root_kconfig __ROOT_KCONFIG)

    idf_build_get_property(python PYTHON)

    idf_build_get_property(config_env_path CONFIG_ENV_PATH)

    if($ENV{LDGEN_CHECK_MAPPING})
        set(ldgen_check "--check-mapping"
            "--check-mapping-exceptions" "${idf_path}/tools/ci/check_ldgen_mapping_exceptions.txt")
        message(STATUS "Mapping check enabled in ldgen")
    endif()

    if(CONFIG_ESPTOOLPY_FAST_REFLASHING)
        # Create a file containing a list of mutable libraries used by ldgen
        # for fast reflashing.
        set(mutable_libs_path "${build_dir}/ldgen_mutable_libraries")
        __ldgen_get_mutable_libs(mutable_libs)
        list(JOIN mutable_libs "\n" mutable_libs_str)
        file(GENERATE OUTPUT "${mutable_libs_path}"
            CONTENT "${mutable_libs_str}")
        set(mutable_libs_option "--mutable-libraries-file" "${mutable_libs_path}")
    endif()

    add_custom_command(
        OUTPUT ${output}
        COMMAND ${python} "${idf_path}/tools/ldgen/ldgen.py"
        --config    "${sdkconfig}"
        --fragments-list "${ldgen_fragment_files}"
        --input     "${template}"
        --output    "${output}"
        --kconfig   "${root_kconfig}"
        --env-file  "${config_env_path}"
        --libraries-file "${build_dir}/ldgen_libraries"
        --objdump   "${CMAKE_OBJDUMP}"
        ${ldgen_check}
        ${mutable_libs_option}
        DEPENDS     ${template} ${ldgen_fragment_files} ${ldgen_deps} ${SDKCONFIG}
        VERBATIM
    )

    # The executable depends on the output of the ldgen command.
    get_filename_component(_name ${output} NAME)
    add_custom_target(__ldgen_output_${_name} DEPENDS ${output})
    add_dependencies(${exe_target} __ldgen_output_${_name})

    # Add the output of the ldgen command to the list of link dependencies.
    idf_build_set_property(__LINK_DEPENDS ${output} APPEND)
endfunction()
