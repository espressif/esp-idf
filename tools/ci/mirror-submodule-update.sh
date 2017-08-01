#!/bin/bash
#
# Redirects git submodules to the specified local mirrors and updates these recursively.
#
# To revert the changed URLs use 'git submodule deinit .'
#

# -----------------------------------------------------------------------------
# Common bash

if [[ ! -z ${DEBUG} ]]
then
  set -x # Activate the expand mode if DEBUG is anything but empty.
fi

set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.
set -o nounset # Exit if variable not set.

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

# -----------------------------------------------------------------------------

[ -z ${GITLAB_SSH_SERVER:-} ] && die "Have to set up GITLAB_SSH_SERVER environment variable"

ERR_CANNOT_UPDATE=13

REPO_DIR=${1:-"${PWD}"}
REPO_DIR=$(readlink -f -- "${REPO_DIR}")

SCRIPT_DIR=$(dirname -- "${0}")
SCRIPT_DIR=$(readlink -f -- "${SCRIPT_DIR}")

SCRIPT_SH=$(readlink -f -- "${0}")

MIRRORLIST=${SCRIPT_DIR}/mirror-list.txt

[ -d "${REPO_DIR}" ] || die "${REPO_DIR} is not directory!"
[ -f "${SCRIPT_SH}" ] || die "${SCRIPT_SH} does not exist!"
[ -f "${MIRRORLIST}" ] || die "${MIRRORLIST} does not exist!"

pushd ${REPO_DIR} >/dev/null

# 0
[ -f ".gitmodules" ] || exit 0

# 1
git submodule init


# 2
# Replacing each submodule URL of the current repository
# according to the one found in the MIRRORLIST

#   SED parses the strings like:
#
#-b991c67c1d91574ef22336cc3a5944d1e63230c9 roms/ipxe
#b991c67c1d91574ef22336cc3a5944d1e63230c9 roms/ipxe (v1.0.0-2388-gb991c67)
#
for SUBPATH in $(git submodule status | sed -E 's/.*[[:space:]](.*)([[:space:]].*|$)/\1/')
do
    SUBMIRROR=$(join -o"2.2" <(echo ${SUBPATH}) <(sort ${MIRRORLIST}))
    [ ${SUBMIRROR} ] || continue
    SUBMIRROR=${SUBMIRROR//@GENERAL_MIRROR_SERVER@/${GITLAB_SSH_SERVER}}
    echo -e "[switch mirror] $SUBPATH \tto\t $SUBMIRROR"

    git config submodule.${SUBPATH}.url ${SUBMIRROR}
done

# 3
# Getting submodules of the current repository from the local mirrors
git submodule update || exit $ERR_CANNOT_UPDATE

# 4
# Replacing URLs for each sub-submodule.
# The script runs recursively
git submodule foreach "${SCRIPT_SH}" # No '--recursive'

popd >/dev/null
