#
# Warn if the toolchain version doesn't match
#
get_expected_ctng_version(expected_toolchain expected_gcc)
gcc_version_check("${expected_gcc}")
crosstool_version_check("${expected_toolchain}")
