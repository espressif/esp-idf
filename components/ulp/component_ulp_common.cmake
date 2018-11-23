if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    spaces2list(ULP_S_SOURCES)
    foreach(ulp_s_source ${ULP_S_SOURCES})
        get_filename_component(ulp_s_source ${ulp_s_source} ABSOLUTE)
        list(APPEND ulp_s_sources ${ulp_s_source})
    endforeach()

    foreach(ulp_s_source ${ulp_s_sources})
        get_filename_component(ulp_ps_source ${ulp_s_source} NAME_WE)
        set(ulp_ps_output ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}/${ulp_ps_source}.ulp.S)
        list(APPEND ulp_ps_sources ${ulp_ps_output})
    endforeach()

    set(ulp_artifacts_prefix ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}/${ULP_APP_NAME})

    set(ulp_artifacts ${ulp_artifacts_prefix}.bin
                        ${ulp_artifacts_prefix}.ld
                        ${ulp_artifacts_prefix}.h)

    set(ulp_artifacts_extras ${ulp_artifacts_prefix}.map
                        ${ulp_artifacts_prefix}.sym
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}/esp32.ulp.ld)

    # Replace the separator for the list of ULP source files that will be passed to
    # the external ULP project. This is a workaround to the bug https://public.kitware.com/Bug/view.php?id=16137.
    string(REPLACE ";" "|" ulp_s_sources "${ulp_s_sources}")

    externalproject_add(${ULP_APP_NAME}
        SOURCE_DIR ${IDF_PATH}/components/ulp/cmake
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}
        INSTALL_COMMAND ""
        CMAKE_ARGS  -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                    -DCMAKE_TOOLCHAIN_FILE=${IDF_PATH}/components/ulp/cmake/toolchain-ulp.cmake
                    -DULP_S_SOURCES=${ulp_s_sources} -DULP_APP_NAME=${ULP_APP_NAME}
                    -DCOMPONENT_PATH=${COMPONENT_PATH}
                    # Even though this resolves to a ';' separated list, this is fine. This must be special behavior
                    # for generator expressions.
                    -DCOMPONENT_INCLUDES=$<TARGET_PROPERTY:${COMPONENT_NAME},INTERFACE_INCLUDE_DIRECTORIES>
                    -DIDF_PATH=${IDF_PATH}
                    -DSDKCONFIG=${SDKCONFIG_HEADER}
        BUILD_COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME} --target build
        BUILD_BYPRODUCTS ${ulp_artifacts} ${ulp_artifacts_extras} ${ulp_ps_sources}
                        ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}/${ULP_APP_NAME}
        BUILD_ALWAYS 1
        LIST_SEPARATOR |
        )

    spaces2list(ULP_EXP_DEP_SRCS)
    set_source_files_properties(${ULP_EXP_DEP_SRCS} PROPERTIES OBJECT_DEPENDS ${ulp_artifacts})

    include_directories(${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME})

    add_custom_target(${ULP_APP_NAME}_artifacts DEPENDS ${ULP_APP_NAME})

    add_dependencies(${COMPONENT_NAME} ${ULP_APP_NAME}_artifacts)

    target_linker_script(${COMPONENT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}/${ULP_APP_NAME}.ld)
    target_add_binary_data(${COMPONENT_NAME} ${CMAKE_CURRENT_BINARY_DIR}/${ULP_APP_NAME}/${ULP_APP_NAME}.bin BINARY)
endif()