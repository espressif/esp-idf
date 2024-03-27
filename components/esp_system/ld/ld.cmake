idf_build_get_property(target IDF_TARGET)
idf_build_get_property(sdkconfig_header SDKCONFIG_HEADER)
idf_build_get_property(config_dir CONFIG_DIR)

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/ld")

# Cmake script that generates linker script from "*.ld.in" scripts using compiler preprocessor
set(linker_script_generator "${CMAKE_CURRENT_BINARY_DIR}/ld/linker_script_generator.cmake")
file(WRITE ${linker_script_generator}
[=[
execute_process(COMMAND "${CC}" "-C" "-P" "-x" "c" "-E" "-I" "${CONFIG_DIR}" "-I" "${LD_DIR}" "${SOURCE}"
                RESULT_VARIABLE RET_CODE
                OUTPUT_VARIABLE PREPROCESSED_LINKER_SCRIPT
                ERROR_VARIABLE ERROR_VAR)
if(RET_CODE AND NOT RET_CODE EQUAL 0)
    message(FATAL_ERROR "Can't generate ${TARGET}\nRET_CODE: ${RET_CODE}\nERROR_MESSAGE: ${ERROR_VAR}")
endif()
string(REPLACE "\\n" "\n" TEXT "${PREPROCESSED_LINKER_SCRIPT}")
file(WRITE "${TARGET}" "${TEXT}")
]=])

if(CONFIG_IDF_TARGET_ESP32C5_BETA3_VERSION)
    set(target_folder "esp32c5/beta3")
elseif(CONFIG_IDF_TARGET_ESP32C5_MP_VERSION)
    set(target_folder "esp32c5/mp")
else()
    set(target_folder "${target}")
endif()

function(preprocess_linker_file name_in name_out out_path)
    set(script_in "${CMAKE_CURRENT_LIST_DIR}/${target_folder}/${name_in}")
    set(script_out "${CMAKE_CURRENT_BINARY_DIR}/ld/${name_out}")
    set(${out_path} ${script_out} PARENT_SCOPE)

    add_custom_command(
        OUTPUT ${script_out}
        COMMAND ${CMAKE_COMMAND}
            "-DCC=${CMAKE_C_COMPILER}"
            "-DSOURCE=${script_in}"
            "-DTARGET=${script_out}"
            "-DCONFIG_DIR=${config_dir}"
            "-DLD_DIR=${CMAKE_CURRENT_LIST_DIR}"
            -P "${linker_script_generator}"
        MAIN_DEPENDENCY ${script_in}
        DEPENDS ${sdkconfig_header}
        COMMENT "Generating ${script_out} linker script..."
        VERBATIM)
    add_custom_target("${name_out}" DEPENDS "${script_out}")
    add_dependencies(${COMPONENT_LIB} "${name_out}")
endfunction()

# Generage memory.ld
preprocess_linker_file("memory.ld.in" "memory.ld" ld_out_path)
target_linker_script(${COMPONENT_LIB} INTERFACE "${ld_out_path}")

# Generage sections.ld.in and pass it through linker script generator
preprocess_linker_file("sections.ld.in" "sections.ld.in" ld_out_path)
target_linker_script(${COMPONENT_LIB} INTERFACE "${ld_out_path}"
                    PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ld/sections.ld")
