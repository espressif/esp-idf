# check_cxx_atomic_policy.cmake
#
# Post-build check that verifies the linked executable uses a single libstdc++
# atomic lock policy (__gnu_cxx::_Lock_policy).
#
# libstdc++ selects the policy used for std::shared_ptr reference counting based
# on _GLIBCXX_HAVE_ATOMIC_LOCK_POLICY. Mixing object files or prebuilt libraries
# that were compiled with different settings results in different _Lock_policy
# template instantiations being linked into the same image, which is an ODR
# violation and can cause undefined behavior at runtime.
#
# The policy is encoded in mangled symbol names, e.g. the substring
# "_Lock_policyE2" (atomic) or "_Lock_policyE1" (mutex). If the symbol table
# contains more than one distinct value, the policies are mixed.
#
# Invoked via "cmake -P" with:
#   ELF_FILE      - path to the linked executable
#   CMAKE_OBJDUMP - objdump executable to inspect the symbol table

if(NOT DEFINED ELF_FILE)
    message(FATAL_ERROR "check_cxx_atomic_policy.cmake: ELF_FILE is not set")
endif()

if(NOT CMAKE_OBJDUMP)
    message(WARNING "check_cxx_atomic_policy.cmake: CMAKE_OBJDUMP is not set, skipping check")
    return()
endif()

execute_process(
    COMMAND "${CMAKE_OBJDUMP}" -t "${ELF_FILE}"
    OUTPUT_VARIABLE objdump_output
    ERROR_VARIABLE objdump_error
    RESULT_VARIABLE objdump_result
)

if(NOT objdump_result EQUAL 0)
    message(FATAL_ERROR
        "check_cxx_atomic_policy.cmake: failed to run objdump on '${ELF_FILE}': ${objdump_error}")
endif()

string(REGEX MATCHALL "_Lock_policyE[0-9]" lock_policy_matches "${objdump_output}")

set(lock_policies "")
foreach(match IN LISTS lock_policy_matches)
    string(REGEX REPLACE "^.*_Lock_policyE([0-9])$" "\\1" policy "${match}")
    list(APPEND lock_policies "${policy}")
endforeach()

if(lock_policies)
    list(REMOVE_DUPLICATES lock_policies)
    list(SORT lock_policies)
endif()
list(LENGTH lock_policies num_policies)

if(num_policies GREATER 1)
    string(REPLACE ";" ", " policies_str "${lock_policies}")
    message(FATAL_ERROR
        "Mixed libstdc++ atomic lock policies detected in '${ELF_FILE}' "
        "(__gnu_cxx::_Lock_policy values: ${policies_str}).")
endif()
