get_property(__idf_env_set GLOBAL PROPERTY __IDF_ENV_SET)
if(NOT __idf_env_set)
    # Infer an IDF_PATH relative to the tools/cmake directory
    get_filename_component(_idf_path "${CMAKE_CURRENT_LIST_DIR}/../.." REALPATH)
    file(TO_CMAKE_PATH "${_idf_path}" _idf_path)

    # Get the path set in environment
    set(idf_path $ENV{IDF_PATH})

    # Environment IDF_PATH should match the inferred IDF_PATH. If not, warn the user.
    # (Note: REALPATH is needed in both above steps to account for case on case
    # insensitive filesystems, or relative paths)
    if(idf_path)
        get_filename_component(idf_path "${idf_path}" REALPATH)
        file(TO_CMAKE_PATH "${idf_path}" idf_path)

        if(NOT idf_path STREQUAL _idf_path)
            message(WARNING "IDF_PATH environment variable is different from inferred IDF_PATH.
                            Check if your project's top-level CMakeLists.txt includes the right
                            CMake files. Environment IDF_PATH will be used for the build:
                            ${idf_path}")
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
    include(CheckCCompilerFlag)
    include(CheckCXXCompilerFlag)
    include(git_submodules)
    include(tool_version_check)
    include(kconfig)
    include(component)
    include(utilities)
    include(depgraph)
    include(targets)
    include(ldgen)
    include(dfu)
    include(version)
    include(prefix_map)

    __build_init("${idf_path}")

    # Check if IDF_ENV_FPGA environment is set
    set(env_idf_env_fpga $ENV{IDF_ENV_FPGA})
    if(${env_idf_env_fpga})
        idf_build_set_property(__IDF_ENV_FPGA "y")
        message(NOTICE "IDF_ENV_FPGA is set, building for FPGA environment")
    endif()

    set_property(GLOBAL PROPERTY __IDF_ENV_SET 1)
endif()
