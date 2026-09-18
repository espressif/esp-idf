# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(ExternalProject)

#[[api
.. cmakev2:function:: idf_subproject_add

    .. code-block:: cmake

        idf_subproject_add(<name>
                           EXTERNALPROJECT_OPTIONS <externalproject_add options>...)

    *name[in]*

        Name of the target that builds the subproject.

    *EXTERNALPROJECT_OPTIONS[in]*

        The options of ``externalproject_add``. Every argument after this
        keyword is passed to ``externalproject_add`` as given, so it must come
        last.

    Build an ESP-IDF subproject with ``externalproject_add``, with a
    configuration of its own. The subproject resolves its config options from
    the components it discovers and stores them in an ``sdkconfig`` in its own
    project directory, instead of reading the main project's.

    The subproject is passed ``IDF_PATH``, ``IDF_TARGET``, ``PYTHON`` and
    ``IDF_BUILD_V2`` from the project that builds it, in addition to the
    ``CMAKE_ARGS`` given in ``EXTERNALPROJECT_OPTIONS``. Unless the caller gives
    them, the subproject is built in ``subprojects/<name>`` in the main
    project's build directory, it has no install step, ``INSTALL_COMMAND ""``,
    and it is built on every build of the main project, ``BUILD_ALWAYS 1``, so
    that a change in the main project's configuration reaches it.

    .. code-block:: cmake

        idf_subproject_add(sensor
            EXTERNALPROJECT_OPTIONS
                SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/sensor")
#]]
function(idf_subproject_add name)
    # PARSE_ARGV keeps an empty value, such as the one in 'INSTALL_COMMAND ""',
    # and externalproject_add keeps it too when the options are passed as one
    # quoted list.
    cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "EXTERNALPROJECT_OPTIONS")
    if(ARG_UNPARSED_ARGUMENTS)
        list(GET ARG_UNPARSED_ARGUMENTS 0 unexpected)
        idf_die("idf_subproject_add: unexpected argument '${unexpected}'. The options of "
                "externalproject_add follow EXTERNALPROJECT_OPTIONS.")
    endif()
    if(NOT ARG_EXTERNALPROJECT_OPTIONS)
        idf_die("idf_subproject_add: the options of externalproject_add are required, after "
                "EXTERNALPROJECT_OPTIONS")
    endif()

    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(idf_target IDF_TARGET)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(build_dir BUILD_DIR)

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
                        -DIDF_SUBPROJECT=1
                        -DIDF_SUBPROJECT_OWN_CONFIG=1)
    externalproject_add(${name} "${options}")
endfunction()
