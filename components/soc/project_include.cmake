if(CONFIG_IDF_TOOLCHAIN_GCC)
    # Common flags
    idf_toolchain_add_flags(LINK_OPTIONS "-nostartfiles")

    # Target-specific flags
    if(CONFIG_IDF_TARGET_ESP32)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-Wno-frame-address")
    elseif(CONFIG_IDF_TARGET_ESP32C2 OR
           CONFIG_IDF_TARGET_ESP32C3)
        set(_march "rv32imc_zicsr_zifencei")
    elseif(CONFIG_IDF_TARGET_ESP32C5 OR
           CONFIG_IDF_TARGET_ESP32C6 OR
           CONFIG_IDF_TARGET_ESP32C61 OR
           CONFIG_IDF_TARGET_ESP32H2 OR
           CONFIG_IDF_TARGET_ESP32H21)
        set(_march "rv32imac_zicsr_zifencei_zaamo_zalrsc")
    elseif(CONFIG_IDF_TARGET_ESP32H4)
        set(_march "rv32imafcb_zicsr_zifencei_zaamo_zalrsc")
    elseif(CONFIG_IDF_TARGET_ESP32P4 OR CONFIG_IDF_TARGET_ESP32S31)
        set(_march "rv32imafc_zicsr_zifencei_zaamo_zalrsc")
    elseif(NOT(CONFIG_IDF_TARGET_ESP32S2 OR CONFIG_IDF_TARGET_ESP32S3))
        message(FATAL_ERROR "Unknown Espressif target: ${CONFIG_IDF_TARGET}")
    endif()

    # Architecture-specific flags
    if(CONFIG_IDF_TARGET_ARCH_XTENSA)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-mlongcalls"
                                                "-fno-builtin-memcpy"
                                                "-fno-builtin-memset"
                                                "-fno-builtin-bzero")
    elseif(CONFIG_IDF_TARGET_ARCH_RISCV)
        if(NOT DEFINED _march)
            message(FATAL_ERROR "-march option is not defined for target ${CONFIG_IDF_TARGET}!")
        endif()

        # Clean compile options that were added by previous configurations and may be outdated
        idf_toolchain_remove_flags(COMPILE_OPTIONS "-march="
                                                   "-mno-cm-push-reverse"
                                                   "-mno-cm-popret")

        if(CONFIG_SOC_CPU_HAS_ZB_EXTENSIONS)
            set(_march "${_march}_zba_zbb_zbs")
        endif()

        if((CONFIG_SOC_CPU_HAS_ZC_EXTENSIONS AND NOT CONFIG_SOC_CPU_ZCMP_WORKAROUND) OR
           CONFIG_COMPILER_ENABLE_RISCV_ZCMP)
            if(NOT CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
                set(_march "${_march}_zcb_zcmp_zcmt")

                if(CONFIG_SOC_CPU_ZCMP_PUSH_REVERSED)
                    idf_toolchain_add_flags(COMPILE_OPTIONS "-mno-cm-push-reverse")
                endif()
                if(CONFIG_SOC_CPU_ZCMP_POPRET_ISSUE)
                    idf_toolchain_add_flags(COMPILE_OPTIONS "-mno-cm-popret")
                endif()
            endif()
        endif()

        if(CONFIG_SOC_CPU_HAS_HWLOOP)
            set(_march "${_march}_xesploop")
        endif()

        if(CONFIG_SOC_CPU_HAS_PIE)
            set(_march "${_march}_xespv")
            if(CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
                set(_march "${_march}2p1")
            endif()
        endif()

        if(CONFIG_SOC_CPU_HAS_DSP)
            set(_march "${_march}_xespdsp")
        endif()

        # Set ABI and ARCH options
        if(CONFIG_SOC_CPU_HAS_FPU)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-mabi=ilp32f")
        endif()
        idf_toolchain_add_flags(COMPILE_OPTIONS "-march=${_march}")

        if(NOT CONFIG_SOC_CPU_MISALIGNED_ACCESS_ON_PMP_MISMATCH_ISSUE)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-mtune=esp-base")
        endif()
    else()
        message(FATAL_ERROR "Unknown Espressif architecture: ${CONFIG_IDF_TARGET_ARCH}")
    endif()

    # Workaround: Re-run CMake compiler ABI detection after ABI flags are set.
    #
    # Problem: CMake performs compiler checks at an early stage during
    # toolchain.cmake processing. At this early stage, response files are not yet
    # ready, which causes CMake paths (e.g., CMAKE_<lang>_IMPLICIT_LINK_DIRECTORIES)
    # to be incorrectly determined.
    #
    # Solution: Re-run the ABI detection after ABI flags are present to correctly
    # determine these paths.
    #
    # Note: If the CMake API changes, this solution may need to be revised.
    set(lang_ext_pairs "C|c" "CXX|cpp")
    include(${CMAKE_ROOT}/Modules/CMakeDetermineCompilerABI.cmake)
    foreach(lang_ext ${lang_ext_pairs})
        string(REPLACE "|" ";" lang_ext_parts ${lang_ext})
        list(GET lang_ext_parts 0 lang)
        list(GET lang_ext_parts 1 ext)
        if(DEFINED CMAKE_${lang}_ABI_COMPILED)
            unset(CMAKE_${lang}_ABI_COMPILED)
            cmake_determine_compiler_abi(${lang} ${CMAKE_ROOT}/Modules/CMake${lang}CompilerABI.${ext})
        endif()
    endforeach()
endif()
