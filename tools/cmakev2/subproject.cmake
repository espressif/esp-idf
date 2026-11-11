# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(ExternalProject)

#[[api
.. cmakev2:function:: idf_subproject_add

    .. code-block:: cmake

        idf_subproject_add(<name> [SHARED_CONFIG | CONFIG_OPTIONS <option>...]
                           EXTERNALPROJECT_OPTIONS <externalproject_add options>...)

    *name[in]*

        Name of the target that builds the subproject.

    *SHARED_CONFIG[in,opt]*

        The subproject reads the main project's ``sdkconfig`` instead of keeping
        a configuration of its own. It cannot be given with ``CONFIG_OPTIONS``.

    *CONFIG_OPTIONS[in,opt]*

        Config options that the subproject takes from the main project, named
        as in the main project's ``sdkconfig``, with the ``CONFIG_`` prefix.

    *EXTERNALPROJECT_OPTIONS[in]*

        The options of ``externalproject_add``. Every argument after this
        keyword is passed to ``externalproject_add`` as given, so it must come
        last.

    Build an ESP-IDF subproject with ``externalproject_add``. By default the
    subproject has a configuration of its own. It resolves its config options
    from the components it discovers and stores them in an ``sdkconfig`` in its
    own project directory, instead of reading the main project's.

    With ``SHARED_CONFIG``, the subproject reads the main project's
    ``sdkconfig`` and does not write it. It is passed ``SDKCONFIG`` set to the
    main project's ``sdkconfig``, an empty ``SDKCONFIG_DEFAULTS``, because the
    main project has already applied its defaults files, and
    ``GENERATE_SDKCONFIG`` set to 0. Its components are built with the main
    project's config option values, and a config option that only the
    subproject declares gets its default value.

    The subproject is passed ``IDF_PATH``, ``IDF_TARGET``, ``PYTHON`` and
    ``IDF_BUILD_V2`` from the project that builds it, in addition to the
    ``CMAKE_ARGS`` given in ``EXTERNALPROJECT_OPTIONS``. Unless the caller gives
    them, the subproject is built in ``subprojects/<name>`` in the main
    project's build directory, it has no install step, ``INSTALL_COMMAND ""``,
    and it is built on every build of the main project, ``BUILD_ALWAYS 1``, so
    that a change in the main project's configuration reaches it.

    Each config option in ``CONFIG_OPTIONS``, and only these, gets the main
    project's value in the subproject. Every other config option keeps the value
    the subproject resolves, even if both projects declare it. Before kconfgen
    resolves the subproject's configuration, the build writes a copy of the
    subproject's ``sdkconfig`` to ``config/sdkconfig.imported`` in the
    subproject's build directory. In this copy, the lines of the listed config
    options are removed, and their lines from the main project's ``sdkconfig``
    are added at the end. kconfgen reads the copy instead of the subproject's
    ``sdkconfig`` and uses the last value it reads for a config option, so
    every file kconfgen generates for the subproject has the main project's
    value. A component of the subproject must declare the config option, as one
    of the ESP-IDF components does for their options.

    After kconfgen, the build stops with an error if a listed config option has
    a different value in the subproject than in the main project. This happens
    for a config option without a prompt, whose value kconfgen derives from
    other config options and does not take from a configuration file. It also
    happens for a config option whose dependencies are not met in only one of
    the projects, or that no component of the subproject declares. A bool
    option that is off and one that has no value are the same. A listed name
    that has no value in either project is reported with a warning, because it
    may be misspelled.

    .. code-block:: cmake

        idf_subproject_add(sensor
            CONFIG_OPTIONS CONFIG_FREERTOS_UNICORE CONFIG_LOG_DEFAULT_LEVEL_WARN
            EXTERNALPROJECT_OPTIONS
                SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sensor")
#]]
function(idf_subproject_add name)
    # PARSE_ARGV keeps an empty value, such as the one in 'INSTALL_COMMAND ""',
    # and externalproject_add keeps it too when the options are passed as one
    # quoted list.
    cmake_parse_arguments(PARSE_ARGV 1 ARG "SHARED_CONFIG" "" "CONFIG_OPTIONS;EXTERNALPROJECT_OPTIONS")
    if(ARG_UNPARSED_ARGUMENTS)
        list(GET ARG_UNPARSED_ARGUMENTS 0 unexpected)
        idf_die("idf_subproject_add: unexpected argument '${unexpected}'. The options of "
                "externalproject_add follow EXTERNALPROJECT_OPTIONS.")
    endif()
    if(NOT ARG_EXTERNALPROJECT_OPTIONS)
        idf_die("idf_subproject_add: the options of externalproject_add are required, after "
                "EXTERNALPROJECT_OPTIONS")
    endif()
    if(ARG_SHARED_CONFIG AND ARG_CONFIG_OPTIONS)
        idf_die("idf_subproject_add: CONFIG_OPTIONS cannot be given with SHARED_CONFIG. A "
                "subproject that shares the main project's configuration already reads every "
                "config option from the main project's sdkconfig.")
    endif()

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(main_sdkconfig SDKCONFIG)

    # The default install step of externalproject_add runs an install target
    # that an ESP-IDF project does not have. Without BUILD_ALWAYS, the subproject
    # is not configured again after a change in the main project's
    # configuration. externalproject_add joins the values of a repeated option,
    # so a default is added only if the caller does not give the option.
    set(options "${ARG_EXTERNALPROJECT_OPTIONS}")
    if(NOT "BINARY_DIR" IN_LIST options)
        list(APPEND options BINARY_DIR "${build_dir}/subprojects/${name}")
    endif()
    if(NOT "INSTALL_COMMAND" IN_LIST options)
        list(APPEND options INSTALL_COMMAND "")
    endif()
    if(NOT "BUILD_ALWAYS" IN_LIST options)
        list(APPEND options BUILD_ALWAYS 1)
    endif()

    # PARSE_ARGV escapes a semicolon inside one argument, so a list given as one
    # quoted argument is split here. A config option name has no semicolon.
    string(REPLACE "\\;" ";" config_options "${ARG_CONFIG_OPTIONS}")

    # A list in CMAKE_ARGS would be split into separate arguments.
    list(JOIN config_options "," config_options)

    # externalproject_add joins repeated CMAKE_ARGS. These come after the
    # caller's, so that the caller cannot set them to other values. Every
    # subproject is passed the same variables, and a subproject may not read
    # all of them, for example IDF_SUBPROJECT_OWN_CONFIG before it has an
    # sdkconfig, so CMake does not warn about the ones it does not read.
    list(APPEND options CMAKE_ARGS
                        --no-warn-unused-cli
                        -DIDF_PATH=${idf_path}
                        -DIDF_TARGET=${idf_target}
                        -DPYTHON=${python}
                        -DPYTHON_DEPS_CHECKED=1
                        -DIDF_BUILD_V2=1
                        -DIDF_SUBPROJECT=1)
    # The subproject's CMake cache keeps the values of an earlier configure, so
    # each kind of subproject is passed every variable the other kind sets. A
    # subproject that keeps a configuration of its own has the three
    # configuration variables removed before the caller's CMAKE_ARGS, so that
    # the caller can still set them.
    if(ARG_SHARED_CONFIG)
        list(APPEND options CMAKE_ARGS
                            -DSDKCONFIG=${main_sdkconfig}
                            -DSDKCONFIG_DEFAULTS=
                            -DGENERATE_SDKCONFIG=0
                            -DIDF_SUBPROJECT_OWN_CONFIG=0
                            -DIDF_SUBPROJECT_IMPORTED_CONFIG=
                            -DIDF_SUBPROJECT_MAIN_SDKCONFIG=)
    else()
        list(PREPEND options CMAKE_ARGS
                             -USDKCONFIG
                             -USDKCONFIG_DEFAULTS
                             -UGENERATE_SDKCONFIG)
        list(APPEND options CMAKE_ARGS
                            -DIDF_SUBPROJECT_OWN_CONFIG=1
                            -DIDF_SUBPROJECT_IMPORTED_CONFIG=${config_options}
                            -DIDF_SUBPROJECT_MAIN_SDKCONFIG=${main_sdkconfig})
    endif()
    externalproject_add(${name} "${options}")
endfunction()

#[[
    __subproject_import_config(INPUT <sdkconfig> OUTPUT <variable>)

    *INPUT[in]*

        The subproject's configuration that kconfgen would read. It does not
        exist before the subproject is configured for the first time.

    *OUTPUT[out]*

        Variable to store the path of the configuration kconfgen reads instead.

    In a subproject added with idf_subproject_add() with CONFIG_OPTIONS, write
    the configuration kconfgen reads. It is the subproject's own configuration
    without the imported config options, followed by the lines of the imported
    config options in the main project's sdkconfig, as kconfgen wrote them
    there. kconfgen uses the last value it reads for a config option, so the
    imported config options get the main project's values. The subproject's
    sdkconfig.defaults is still applied to the other config options.

    A value the subproject's own configuration sets differently, for example
    with menuconfig, is recorded, and __subproject_check_imported_config()
    reports it once the replacement has taken effect.
#]]
function(__subproject_import_config)
    cmake_parse_arguments(ARG "" "INPUT;OUTPUT" "" ${ARGN})

    set(own "")
    if(EXISTS "${ARG_INPUT}")
        file(READ "${ARG_INPUT}" own)
    endif()
    file(READ "${IDF_SUBPROJECT_MAIN_SDKCONFIG}" main)
    # A changed value in the main project configures the subproject again.
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${IDF_SUBPROJECT_MAIN_SDKCONFIG}")
    # A leading newline lets every line be matched as '\n<line>\n'.
    set(own "\n${own}")
    set(main "\n${main}")

    # The lines imported last time. A value the subproject's sdkconfig still
    # has from then was not set in the subproject, so it is not reported.
    # Without them, as in a new build directory, a value may come from another
    # build that uses the same sdkconfig, so nothing is reported.
    idf_build_get_property(config_dir CONFIG_DIR)
    set(output "${config_dir}/sdkconfig.imported")
    set(previous "")
    set(report_replaced FALSE)
    if(EXISTS "${output}")
        file(READ "${output}" previous)
        set(report_replaced TRUE)
    endif()
    set(previous_imported "\n")
    if(previous MATCHES "\n# Imported from the main project\n(.*)$")
        set(previous_imported "\n${CMAKE_MATCH_1}")
    endif()

    set(imported "")
    set(replaced_options "")
    string(REPLACE "," ";" options "${IDF_SUBPROJECT_IMPORTED_CONFIG}")
    foreach(option IN LISTS options)
        set(line_regex "(${option}=[^\n]*|# ${option} is not set)")
        if(NOT main MATCHES "\n${line_regex}\n")
            continue()
        endif()
        set(main_line "${CMAKE_MATCH_1}")
        string(APPEND imported "${main_line}\n")
        if(report_replaced AND own MATCHES "\n${line_regex}\n" AND NOT CMAKE_MATCH_1 STREQUAL main_line)
            set(own_line "${CMAKE_MATCH_1}")
            set(previous_line "")
            if(previous_imported MATCHES "\n${line_regex}\n")
                set(previous_line "${CMAKE_MATCH_1}")
            endif()
            if(NOT own_line STREQUAL previous_line)
                list(APPEND replaced_options "${option}")
                idf_build_set_property(__SUBPROJECT_REPLACED_${option} "${own_line}")
            endif()
        endif()
        # The '# default:' line above belongs to the value that is removed.
        string(REGEX REPLACE "\n(# default:\n)?${line_regex}\n" "\n" own "${own}")
    endforeach()
    string(REGEX REPLACE "^\n" "" own "${own}")

    set(content "${own}\n# Imported from the main project\n${imported}")
    if(NOT "${previous}" STREQUAL "${content}")
        file(WRITE "${output}" "${content}")
    endif()

    idf_build_set_property(__SUBPROJECT_REPLACED "${replaced_options}")

    set(${ARG_OUTPUT} "${output}" PARENT_SCOPE)
endfunction()

#[[
    __subproject_line_of(<option> <text> <variable>)

    Store in <variable> the line of <option> in the sdkconfig <text>, which
    starts with a newline, or nothing if it has none.
#]]
function(__subproject_line_of option text out_var)
    set(line "")
    if(text MATCHES "\n(${option}=[^\n]*|# ${option} is not set)\n")
        set(line "${CMAKE_MATCH_1}")
    endif()
    set(${out_var} "${line}" PARENT_SCOPE)
endfunction()

#[[
    __subproject_check_imported_config()

    Stop the build if an imported config option has a different value in the
    subproject's resolved configuration than in the main project's. kconfgen
    does not take a value for a config option without a prompt, for one whose
    dependencies are not met, or for one it does not know, so the import can
    fail. A bool option that is off and one that has no line are the same.

    Warn about a listed name that has no line in either configuration, because
    it may be misspelled, and about the values set in the subproject's
    configuration that the import replaced.

    Runs on every configure, also when kconfgen is not run again, so that a
    build that stopped stops again until the configuration is fixed.
#]]
function(__subproject_check_imported_config)
    idf_build_get_property(sdkconfig SDKCONFIG)
    file(READ "${IDF_SUBPROJECT_MAIN_SDKCONFIG}" main)
    file(READ "${sdkconfig}" own)
    set(main "\n${main}")
    set(own "\n${own}")
    idf_build_get_property(replaced_options __SUBPROJECT_REPLACED)

    set(unknown "")
    set(differ "")
    set(replaced "")
    string(REPLACE "," ";" options "${IDF_SUBPROJECT_IMPORTED_CONFIG}")
    foreach(option IN LISTS options)
        __subproject_line_of(${option} "${main}" main_line)
        __subproject_line_of(${option} "${own}" own_line)
        if(main_line STREQUAL "" AND own_line STREQUAL "")
            string(APPEND unknown "    ${option}\n")
            continue()
        endif()

        set(main_value "${main_line}")
        set(own_value "${own_line}")
        if(main_value STREQUAL "# ${option} is not set")
            set(main_value "")
        endif()
        if(own_value STREQUAL "# ${option} is not set")
            set(own_value "")
        endif()
        if(NOT main_value STREQUAL own_value)
            foreach(value IN ITEMS main_value own_value)
                if("${${value}}" STREQUAL "")
                    set(${value} "no value")
                endif()
            endforeach()
            string(APPEND differ "    ${option}: ${main_value} in the main project, "
                                 "${own_value} in the subproject\n")
        elseif(option IN_LIST replaced_options)
            idf_build_get_property(replaced_line __SUBPROJECT_REPLACED_${option})
            string(APPEND replaced "    ${replaced_line} is replaced by ${main_line}\n")
        endif()
    endforeach()

    if(unknown)
        idf_warn("These config options in CONFIG_OPTIONS have no value in the main project or "
                 "in this subproject. Check that the names are right:\n${unknown}")
    endif()
    if(replaced)
        idf_warn("These config options are imported from the main project, so the values set "
                 "in this subproject's configuration are replaced:\n${replaced}")
    endif()
    if(differ)
        idf_die("These config options are imported from the main project, but this subproject "
                "resolves them to other values:\n${differ}"
                "kconfgen does not take the value of a config option without a prompt, or of one "
                "whose dependencies are not met in this subproject, or of one that no component "
                "of this subproject declares. Import the config options that such an option "
                "depends on, and make sure a component of this subproject declares them.")
    endif()
endfunction()
