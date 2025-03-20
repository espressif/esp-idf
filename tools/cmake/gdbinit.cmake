# __generate_gdbinit
# Prepares gdbinit files to pass into the debugger.

function(__generate_gdbinit)
    # Define static gdbinit commands
    if(CONFIG_IDF_TARGET_LINUX)
        set(gdbinit_connect
            "# Run the application and stop on app_main()\n"
            "break app_main\n"
            "run\n")
    else()
        set(gdbinit_connect
            "# Connect to the default openocd-esp port and stop on app_main()\n"
            "set remotetimeout 10\n"
            "target remote :3333\n"
            "monitor reset halt\n"
            "maintenance flush register-cache\n"
            "thbreak app_main\n"
            "continue\n")
    endif()

    set(gdbinit_py_extensions
        "# Add Python GDB extensions\n"
        "python\n"
        "try:\n"
        "    import freertos_gdb\n"
        "except ModuleNotFoundError:\n"
        "    print('warning: python extension \"freertos_gdb\" not found.')\n"
        "end\n")

    # Define paths
    set(gdbinit_dir ${BUILD_DIR}/gdbinit)
    set(symbols_gdbinit_path ${gdbinit_dir}/symbols)
    set(py_extensions_gdbinit_path ${gdbinit_dir}/py_extensions)
    set(connect_gdbinit_path ${gdbinit_dir}/connect)
    idf_build_get_property(PROJECT_EXECUTABLE EXECUTABLE)
    set(application_elf ${BUILD_DIR}/${PROJECT_EXECUTABLE})

    file(MAKE_DIRECTORY ${gdbinit_dir})

    # Get ROM ELFs gdbinit part
    if(CONFIG_IDF_TARGET_LINUX)
        set(rom_symbols)
    else()
        execute_process(
            COMMAND python "${idf_path}/components/esp_rom/gen_gdbinit.py" ${IDF_TARGET}
            OUTPUT_VARIABLE rom_symbols
            RESULT_VARIABLE result
        )
        if(NOT result EQUAL 0)
            set(rom_symbols)
            message(WARNING "Error while generating esp_rom gdbinit")
        endif()
    endif()

    # Check if bootloader ELF is defined and set symbol-file accordingly
    if(DEFINED BOOTLOADER_ELF_FILE)
        set(add_bootloader_symbols "  add-symbol-file ${BOOTLOADER_ELF_FILE}")
    else()
        set(add_bootloader_symbols "  # Bootloader elf was not found")
    endif()

    # Configure and generate files
    configure_file(${idf_path}/tools/cmake/symbols.gdbinit.in ${symbols_gdbinit_path})
    file(WRITE ${py_extensions_gdbinit_path} ${gdbinit_py_extensions})
    file(WRITE ${connect_gdbinit_path} ${gdbinit_connect})

    # TODO IDF-11667
    file(WRITE ${gdbinit_dir}/gdbinit "source ${symbols_gdbinit_path}\n")
    file(APPEND ${gdbinit_dir}/gdbinit "source ${connect_gdbinit_path}\n")

    # NOTE: prefix_map gbinit file generates by prefix_map.cmake file
    idf_build_set_property(GDBINIT_FILES_SYMBOLS ${symbols_gdbinit_path})
    idf_build_set_property(GDBINIT_FILES_PY_EXTENSIONS ${py_extensions_gdbinit_path})
    idf_build_set_property(GDBINIT_FILES_CONNECT ${connect_gdbinit_path})
endfunction()
