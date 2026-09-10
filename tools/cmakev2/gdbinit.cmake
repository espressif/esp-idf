# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

#[[
    __generate_gdbinit(<executable> <gdbinit_dir>
                       <out_symbols> <out_py_extensions> <out_connect>)

    *executable[in]*

        Executable target whose ELF the debugger loads the application symbols
        from. The ELF path is resolved from the target.

    *gdbinit_dir[in]*

        Directory into which the generated gdbinit files are written.

    *out_symbols[out]*, *out_py_extensions[out]*, *out_connect[out]*

        Names of variables set in the caller's scope to the paths of the
        generated ``symbols``, ``py_extensions`` and ``connect`` files. These
        paths are per-executable, so they are returned to the caller instead of
        stored in global build properties, which in a multi-executable build
        would only ever hold the last executable's paths.

    Prepare the gdbinit files (``symbols``, ``connect``, ``py_extensions`` and
    the combined ``gdbinit``) passed to the debugger. Taking the executable
    target and output directory as arguments lets a project that builds several
    executables produce a distinct set of gdbinit files per executable instead
    of overwriting a single shared directory.
#]]
function(__generate_gdbinit executable gdbinit_dir out_symbols out_py_extensions out_connect)
    if(NOT TARGET "${executable}")
        idf_die("The executable '${executable}' is not a cmake target")
    endif()

    # The output paths derive from gdbinit_dir alone, so return them on every
    # call, including the repeat calls that skip regeneration below.
    set(symbols_gdbinit_path "${gdbinit_dir}/symbols")
    set(py_extensions_gdbinit_path "${gdbinit_dir}/py_extensions")
    set(connect_gdbinit_path "${gdbinit_dir}/connect")
    set(${out_symbols} "${symbols_gdbinit_path}" PARENT_SCOPE)
    set(${out_py_extensions} "${py_extensions_gdbinit_path}" PARENT_SCOPE)
    set(${out_connect} "${connect_gdbinit_path}" PARENT_SCOPE)

    # CMake permits only one file(GENERATE) per output path, and the same
    # executable can be passed to idf_build_generate_metadata more than once
    # (e.g. as EXECUTABLE and again as its BINARY). Generate the gdbinit files
    # only the first time a directory is used, keyed by the hash of its absolute
    # path so that differently-spelled paths resolving to the same directory are
    # matched. A different executable reusing the same directory is a collision.
    get_filename_component(gdbinit_dir_abs "${gdbinit_dir}" ABSOLUTE
                           BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
    string(MD5 gdbinit_dir_key "${gdbinit_dir_abs}")
    idf_build_get_property(gdbinit_dir_owner __GDBINIT_OWNER_${gdbinit_dir_key})
    if(gdbinit_dir_owner)
        if(NOT gdbinit_dir_owner STREQUAL executable)
            idf_die("Executables '${gdbinit_dir_owner}' and '${executable}' both generate gdbinit "
                    "files into '${gdbinit_dir}'. Pass a distinct GDBINIT_DIR per executable.")
        endif()
        return()
    endif()
    idf_build_set_property(__GDBINIT_OWNER_${gdbinit_dir_key} "${executable}")

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(python PYTHON)
    file(MAKE_DIRECTORY "${gdbinit_dir}")

    # Resolve the ELF path from the target instead of reconstructing it from
    # OUTPUT_NAME and SUFFIX.
    set(application_elf "$<TARGET_FILE:${executable}>")

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
        "import sys\n"
        "try:\n"
        "    import freertos_gdb\n"
        "except ModuleNotFoundError:\n"
        "    print('warning: python extension \"freertos_gdb\" not found.', file=sys.stderr)\n"
        "try:\n"
        "    import idf_drivers_gdb\n"
        "except ModuleNotFoundError:\n"
        "    print('warning: python extension \"idf_drivers_gdb\" not found.', file=sys.stderr)\n"
        "end\n")

    # Get ROM ELFs gdbinit part
    if(CONFIG_IDF_TARGET_LINUX)
        set(rom_symbols)
    else()
        execute_process(
            COMMAND ${python} "${idf_path}/components/esp_rom/gen_gdbinit.py" ${IDF_TARGET}
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

    # application_elf is a generator expression, which configure_file() does not
    # expand, so the configured template is routed through file_generate.
    configure_file("${idf_path}/tools/cmake/symbols.gdbinit.in" "${symbols_gdbinit_path}.templ")
    file(READ "${symbols_gdbinit_path}.templ" symbols_gdbinit_templ)
    file(REMOVE "${symbols_gdbinit_path}.templ")
    file_generate("${symbols_gdbinit_path}" CONTENT "${symbols_gdbinit_templ}")
    file(WRITE "${py_extensions_gdbinit_path}" ${gdbinit_py_extensions})
    file(WRITE "${connect_gdbinit_path}" ${gdbinit_connect})

    file(WRITE "${gdbinit_dir}/gdbinit" "source ${symbols_gdbinit_path}\n")
    file(APPEND "${gdbinit_dir}/gdbinit" "source ${connect_gdbinit_path}\n")
endfunction()
