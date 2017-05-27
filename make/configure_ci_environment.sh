#!/bin/bash
#
# Short script that is sourced in to the CI environment
# in .gitlab-ci.yml
#
# Sets IS_PUBLIC and IS_PRIVATE based on branch type
#
# Tweaks .gitmodules file for private builds

[ -z $CI_BUILD_REF ] && echo "This internal script should only be run by a Gitlab CI runner." && exit 1

REF=$CI_BUILD_REF

# Public branches are:
# release branches - start with release/
# release tags - look like vXX.YY or vXX.YY.ZZ with an optional dash followed by anything on the end
# master branch
#
# These POSIX REs are equivalent to the REs in some "only:" sections of the gitlab-ci.yml file
#
if [[ $REF = "master" || $REF =~ ^release/v || $REF =~ ^v[0-9]+\.[0-9]+(\.[0-9]+)?(-|$) ]]; then
    export IS_PUBLIC=1
else
    export IS_PRIVATE=1
fi

unset REF

set -e

if [[ $IS_PRIVATE ]]; then
   # Redirect git submodules from public github to our private gitlab server
   sed -i "s%https://github.com/espressif/esp32-wifi-lib%${GITLAB_SSH_SERVER}/idf/esp32-wifi-lib%" .gitmodules
   sed -i "s%https://github.com/espressif/esp32-bt-lib%${GITLAB_SSH_SERVER}/idf/esp32-bt-lib%" .gitmodules
fi
