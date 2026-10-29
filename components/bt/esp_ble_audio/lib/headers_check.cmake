# Binds the prebuilt BLE Audio lib (lib/lib/<target>/[log_compressed/]libble_audio.a)
# to the IDF headers it was compiled against. Nothing checks struct layouts at run
# time, so a mismatch must stop the build.
#
# What is checked, and how
#
#   1. abi_check.c, next to the lib. register_ble_audio_libs compiles it into bt; it
#      generates no code. It holds static assertions on what the lib uses from the
#      headers under
#          components/bt/esp_ble_iso/include
#          components/bt/esp_ble_iso/host/utils/include
#          components/bt/esp_ble_audio/include
#          components/bt/esp_ble_audio/host/services
#          components/bt/esp_ble_audio/lib/include
#      - each struct, union and enum the lib uses: its size, the offset of each
#        field and each enumerator value;
#      - each macro the lib expands: its full expansion must be the same text.
#      A mismatch fails the compilation with "libble_audio.a was built with another
#      layout" or "... another definition of <macro>".
#
#   2. idf_headers.sha256, next to the lib, checked by this function at configure
#      time:
#      - lib_abi.h, the function tables between the lib and IDF, compared by content
#        with comments and whitespace ignored;
#      - libble_audio.a and abi_check.c, so that neither is replaced without the
#        other.
#      A mismatch fails the configuration and lists the changed or missing files.
#
# Rules for the headers above
#
#   - Allowed without a new lib: comments, documentation, formatting, new
#     declarations, macros and types, and changes to anything the lib does not use.
#   - Not allowed: changing a struct, union or enum the lib uses (fields, their order,
#     types or sizes, enumerator values), the definition of a macro the lib uses, or
#     lib_abi.h. Some of these are beyond abi_check.c, so keep them unchanged too:
#     function signatures in the callback structs, bit-field widths and the bodies of
#     inline functions the lib uses.
#   - Do not edit or replace anything in lib/lib; keep the submodule at the commit
#     this ESP-IDF points to (git submodule update --init).
#   - When a check fails, restore the header to the version shipped with this
#     ESP-IDF, or update the submodule. Changing what the lib uses needs a new lib
#     built against the changed headers.
function(ble_audio_lib_headers_check lib)
    get_filename_component(lib_dir "${lib}" DIRECTORY)
    set(manifest "${lib_dir}/idf_headers.sha256")
    if(NOT EXISTS "${manifest}")
        message(FATAL_ERROR "BLE Audio: ${manifest} is missing. Run `git submodule update` "
                            "for components/bt/esp_ble_audio/lib/lib so it matches this ESP-IDF.")
    endif()

    file(STRINGS "${manifest}" entries REGEX "^[0-9a-f]+  ")
    if(NOT entries)
        message(FATAL_ERROR "BLE Audio: ${manifest} lists no headers.")
    endif()
    foreach(name libble_audio.a abi_check.c)
        if(NOT ";${entries};" MATCHES "  ${name};")
            message(FATAL_ERROR "BLE Audio: ${manifest} has no hash of ${lib_dir}/${name}.")
        endif()
    endforeach()

    # Stands for "*/" so that block comments need no recursive regex
    string(ASCII 1 comment_end)

    set(mismatch "")
    set(paths "")
    foreach(entry IN LISTS entries)
        # Copied out: every MATCHES below resets CMAKE_MATCH_<n>
        string(REGEX MATCH "^([0-9a-f]+)  (.+)$" _ "${entry}")
        set(expected "${CMAKE_MATCH_1}")
        set(name "${CMAKE_MATCH_2}")
        # Bare names are the files shipped next to the manifest
        if(name MATCHES "^components/bt/")
            # From the bt component being built, which may override the one in IDF_PATH
            string(REGEX REPLACE "^components/bt/" "" bt_name "${name}")
            get_filename_component(path "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../../${bt_name}" ABSOLUTE)
            set(hint "")
        elseif(name MATCHES "/")
            set(path "${IDF_PATH}/${name}")
            set(hint "")
        else()
            set(path "${lib_dir}/${name}")
            set(hint " (copied without its manifest?)")
        endif()
        list(APPEND paths "${path}")
        if(NOT EXISTS "${path}")
            list(APPEND mismatch "  missing: ${path}")
            continue()
        endif()
        if(path MATCHES "\\.a$")
            file(SHA256 "${path}" sha)
        else()
            # Same as normalize() in the lib's idf_headers.py. CR stripped: a Windows
            # checkout with core.autocrlf has CRLF line endings.
            file(READ "${path}" content)
            string(REPLACE "\r" "" content "${content}")
            string(REPLACE "*/" "${comment_end}" content "${content}")
            string(REGEX REPLACE "/\\*[^${comment_end}]*${comment_end}|//[^\n]*" " " content "${content}")
            string(REGEX REPLACE "[ \t]+" " " content "${content}")
            string(REGEX REPLACE " ?\n ?" "\n" content "${content}")
            string(REGEX REPLACE "\n+" "\n" content "${content}")
            string(STRIP "${content}" content)
            string(SHA256 sha "${content}")
        endif()
        if(NOT sha STREQUAL expected)
            list(APPEND mismatch "  changed: ${path}${hint}")
        endif()
    endforeach()

    if(mismatch)
        string(REPLACE ";" "\n" mismatch "${mismatch}")
        message(FATAL_ERROR
            "BLE Audio: ${lib} was built against other versions of these headers:\n"
            "${mismatch}\n"
            "Restore them to the versions shipped with this ESP-IDF; the prebuilt lib depends on them.")
    endif()

    # Re-run the check when the manifest or a listed file changes
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${manifest}" ${paths})
endfunction()
