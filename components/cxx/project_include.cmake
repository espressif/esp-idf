# Sub-projects with a custom toolchain (e.g. ULP) do not use the IDF
# toolchain response file machinery. Skip flag manipulation.
if(IDF_CUSTOM_TOOLCHAIN)
    return()
endif()

if(NOT CONFIG_IDF_TARGET_LINUX)
    if(CONFIG_COMPILER_CXX_ATOMIC_LOCK_POLICY)
        idf_toolchain_add_flags(CXX_COMPILE_OPTIONS "-D_GLIBCXX_HAVE_ATOMIC_LOCK_POLICY=1")
    else()
        idf_toolchain_remove_flags(CXX_COMPILE_OPTIONS "-D_GLIBCXX_HAVE_ATOMIC_LOCK_POLICY=1")
    endif()
endif()
