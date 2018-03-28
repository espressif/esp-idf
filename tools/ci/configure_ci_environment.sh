# This file is sourced in to the CI environment
# in .gitlab-ci.yml
#

# Sets the error behaviour options for shell throughout the CI environment
#
set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.

# we can use the appropriate secret variable for debugging
[ ! -z $DEBUG_SHELL ] && set -x

[ -z $CI_COMMIT_REF_NAME ] && echo "This internal script should only be run by a Gitlab CI runner." && exit 1

# Sets IS_PUBLIC and IS_PRIVATE based on branch type
#
# Public branches are:
# release branches - start with release/
# release tags - look like vXX.YY or vXX.YY.ZZ with an optional dash followed by anything on the end
# master branch
#
# These POSIX REs are equivalent to the REs in some "only:" sections of the gitlab-ci.yml file
#
REF=$CI_COMMIT_REF_NAME
if [[ $REF = "master" || $REF =~ ^release/v || $REF =~ ^v[0-9]+\.[0-9]+(\.[0-9]+)?(-|$) ]]; then
    export IS_PRIVATE=
    export IS_PUBLIC=1
else
    export IS_PRIVATE=1
    export IS_PUBLIC=
fi
unset REF
