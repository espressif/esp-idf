# target_bitscrambler_add_src
#
# Assemble BitScrambler sources and embed into the application.

# This info is not available within the target_bitscrambler_add_src
# function, so save it to a global.
set(esp_bitscrambler_driver_component_path ${CMAKE_CURRENT_LIST_DIR})

function(target_bitscrambler_add_src s_sources)
    if(NOT CMAKE_BUILD_EARLY_EXPANSION)
        idf_build_get_property(target IDF_TARGET)
        spaces2list(s_sources)
        foreach(source ${s_sources})
            get_filename_component(source ${source} ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
            get_filename_component(basename ${source} NAME_WE)
            set(ps_output ${CMAKE_CURRENT_BINARY_DIR}/${basename}.bsbin)
            idf_build_get_property(python PYTHON)
            idf_build_get_property(idf_path IDF_PATH)
            add_custom_command(OUTPUT ${ps_output} DEPENDS ${source}
                    COMMAND ${python} ${idf_path}/tools/bsasm.py ${source} ${ps_output}
                    "-c" ${esp_bitscrambler_driver_component_path}/bsasm_targets/${target}.json)
            target_add_binary_data(${COMPONENT_LIB} ${ps_output} BINARY RENAME_TO bitscrambler_program_${basename})
        endforeach()
    endif()
endfunction()
