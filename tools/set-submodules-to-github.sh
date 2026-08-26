#!/usr/bin/env bash
#
# Explicitly switches the relative submodules locations on GitHub to the original public URLs
#
#       '../../group/repo.git' to 'https://github.com/group/repo.git'
#
# This can be useful for non-GitHub forks to automate getting of right submodules sources.
#

#
# It makes sense to do
#
#       git submodule deinit --force .
#       git submodule init
#
# before running this, and
#
#       git submodule update --recursive
#
# after that. These were not included over this script deliberately, to use the script flexibly
#

set -o errexit
set -o pipefail
set -o nounset

DEBUG_SHELL=${DEBUG_SHELL:-"0"}
[ "${DEBUG_SHELL}" = "1" ] && set -x

### '../../' relative locations

for LINE in $(git config -f .gitmodules --list | grep "\.url=../../[^.]")
do
    SUBPATH=$(echo "${LINE}" | sed "s|^submodule\.\([^.]*\)\.url.*$|\1|")
    LOCATION=$(echo "${LINE}" | sed 's|.*\.url=\.\./\.\./\(.*\)$|\1|')
    SUBURL="https://github.com/$LOCATION"

    git config submodule."${SUBPATH}".url "${SUBURL}"
done

git config --get-regexp '^submodule\..*\.url$'
