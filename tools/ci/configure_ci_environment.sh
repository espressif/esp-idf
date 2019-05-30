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
export PEDANTIC_CFLAGS="-DIDF_CI_BUILD -Werror -Werror=deprecated-declarations -Werror=unused-variable -Werror=unused-but-set-variable -Werror=unused-function"
