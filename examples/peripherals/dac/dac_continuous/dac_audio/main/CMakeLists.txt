idf_component_register(SRCS "dac_audio_example_main.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES esp_driver_dac)

# Generate the audio table header at build time
set(audio_header ${CMAKE_CURRENT_BINARY_DIR}/audio_example_file.h)
set(audio_gen_script ${CMAKE_CURRENT_SOURCE_DIR}/../tools/generate_audio_file.py)
set(audio_wav ${CMAKE_CURRENT_SOURCE_DIR}/../tools/${CONFIG_EXAMPLE_AUDIO_WAV_FILE})

idf_build_get_property(python PYTHON)
idf_build_get_property(project_dir PROJECT_DIR)

add_custom_command(
    OUTPUT ${audio_header}
    COMMAND ${python} ${audio_gen_script}
            --bitwidth ${CONFIG_EXAMPLE_DAC_AUDIO_BITWIDTH}
            -o ${audio_header}
            ${audio_wav}
    DEPENDS ${audio_gen_script} ${audio_wav} ${project_dir}/sdkconfig
    COMMENT "Generating DAC audio table from ${CONFIG_EXAMPLE_AUDIO_WAV_FILE}"
    VERBATIM
)

add_custom_target(dac_audio_header DEPENDS ${audio_header})
add_dependencies(${COMPONENT_LIB} dac_audio_header)
set_source_files_properties(dac_audio_example_main.c PROPERTIES OBJECT_DEPENDS ${audio_header})
target_include_directories(${COMPONENT_LIB} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
             APPEND PROPERTY ADDITIONAL_CLEAN_FILES ${audio_header})
