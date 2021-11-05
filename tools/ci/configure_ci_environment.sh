# This file is sourced in to the CI environment
# in .gitlab-ci.yml
#

# Sets the error behaviour options for shell throughout the CI environment
#
set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.

# We can use the appropriate CI variable for debugging
DEBUG_SHELL=${DEBUG_SHELL:-"0"}
[ "${DEBUG_SHELL}" = "1" ] && set -x

[ -z $CI_COMMIT_REF_NAME ] && echo "This internal script should only be run by a Gitlab CI runner." && exit 1

# Compiler flags to thoroughly check the IDF code in some CI jobs
# (Depends on default options '-Wno-error=XXX' used in the IDF build system)

PEDANTIC_FLAGS="-DIDF_CI_BUILD -Werror -Werror=deprecated-declarations -Werror=unused-variable -Werror=unused-but-set-variable -Werror=unused-function"
export PEDANTIC_CFLAGS="${PEDANTIC_FLAGS} -Wstrict-prototypes"
export PEDANTIC_CXXFLAGS="${PEDANTIC_FLAGS}"

# ccache related settings.
# IDF_CCACHE_ENABLE may be enabled at job level (see build.yml). However it is possible to override it
# globally via CI_CCACHE_DISABLE, in case there are any ccache related issues.
if [ "$CI_CCACHE_DISABLE" = 1 ]; then
    export IDF_CCACHE_ENABLE=0
    echo "INFO: ccache disabled globally using CI_CCACHE_DISABLE=0"
fi

# Set ccache base directory to the project checkout path, to cancel out differences between runners
export CCACHE_BASEDIR="${CI_PROJECT_DIR}"

# In tools/ci/find_apps_build_apps.sh, we use --work-dir argument to copy apps to a separate location
# before building them. This results in cache misses, even though the same code is compiled.
# To solve this issue, we can disable 'hash_dir' option of ccache by setting CCACHE_NOHASHDIR env variable.
# Note, this can result in issues with debug information, see:
#   https://ccache.dev/manual/4.5.html#_compiling_in_different_directories
#
# 'CI_CCACHE_DISABLE_NOHASHDIR' variable can be used at project level to revert to hash_dir=true, in
# case we start seeing failures due to false cache hits.
if [ "${CI_CCACHE_DISABLE_NOHASHDIR}" != "1" ]; then
    export CCACHE_NOHASHDIR="1"
    echo "INFO: ccache CCACHE_NOHASHDIR option is enabled"
fi

# If 'REDIS_CACHE' variable is set at runner (or project) level, use that as secondary ccache storage.
# This can be disabled at project level using 'CI_CCACHE_DISABLE_SECONDARY', in case of issues.
if [ "${CI_CCACHE_DISABLE_SECONDARY}" != "1" ] && [ -n "${REDIS_CACHE}" ]; then
    export CCACHE_SECONDARY_STORAGE="redis://${REDIS_CACHE}"
    echo "INFO: Using CCACHE_SECONDARY_STORAGE=${CCACHE_SECONDARY_STORAGE}"
fi
