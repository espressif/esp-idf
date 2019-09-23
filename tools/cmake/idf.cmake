get_property(__idf_env_set GLOBAL PROPERTY __IDF_ENV_SET)
if(NOT __idf_env_set)
    # Infer an IDF_PATH relative to the tools/cmake directory
    get_filename_component(_idf_path "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
    file(TO_CMAKE_PATH "${_idf_path}" _idf_path)

    # Get the path set in environment
    set(idf_path $ENV{IDF_PATH})
    file(TO_CMAKE_PATH "${idf_path}" idf_path)

    # Environment IDF_PATH should match the inferred IDF_PATH. If not, warn the user.
    if(idf_path)
        if(NOT idf_path STREQUAL _idf_path)
            message(WARNING "IDF_PATH environment variable is different from inferred IDF_PATH.
                            Check if your project's top-level CMakeLists.txt includes the right
                            CMake files. Environment IDF_PATH will be used for the build.")
        endif()
    else()
        message(WARNING "IDF_PATH environment variable not found. Setting IDF_PATH to '${_idf_path}'.")
        set(idf_path ${_idf_path})
        set(ENV{IDF_PATH} ${_idf_path})
    endif()

    # Include other CMake modules required
    set(CMAKE_MODULE_PATH
        "${idf_path}/tools/cmake"
        "${idf_path}/tools/cmake/third_party"
        ${CMAKE_MODULE_PATH})
    include(build)

    set(IDF_PATH ${idf_path})

    include(GetGitRevisionDescription)
    include(git_submodules)
    include(crosstool_version_check)
    include(kconfig)
    include(component)
    include(utilities)
    include(targets)
    include(ldgen)
    include(version)

    __build_init("${idf_path}")

    set_property(GLOBAL PROPERTY __IDF_ENV_SET 1)
endif()