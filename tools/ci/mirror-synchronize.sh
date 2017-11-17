#!/bin/bash
#
# Script for automate mirroring
# You can run this script manually
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
# Recursion
git submodule update --init
git submodule foreach "${SCRIPT_SH}" # No '--recursive'

# 2
git submodule deinit --force .
git submodule update --init


# 3
# Mirroring from original URLs to ones listed in the MIRRORLIST

#   SED parses the strings like:
#
#-b991c67c1d91574ef22336cc3a5944d1e63230c9 roms/ipxe
#b991c67c1d91574ef22336cc3a5944d1e63230c9 roms/ipxe (v1.0.0-2388-gb991c67)
#
for SUBPATH in $(git submodule status | sed -E 's/.*[[:space:]](.*)([[:space:]].*|$)/\1/')
do
    SUBURL=$(git config -f .gitmodules --get submodule.$SUBPATH.url)
    SUBMIRROR=$(join -o"2.2" <(echo ${SUBPATH}) <(sort ${MIRRORLIST}))
    [ ${SUBMIRROR} ] || continue
    SUBMIRROR=${SUBMIRROR//@GENERAL_MIRROR_SERVER@/${GITLAB_SSH_SERVER}}

    ALLOW_TO_SYNC=$(join -o"2.3" <(echo ${SUBPATH}) <(sort ${MIRRORLIST}))
    if [ "${ALLOW_TO_SYNC}" != "ALLOW_TO_SYNC_FROM_PUBLIC" ]
    then
        echo "[should not to sync mirror] ${SUBMIRROR}"
        continue
    fi

    echo -e "[mirror sync] ${SUBURL} \tto\t ${SUBMIRROR}"

    pushd ${SUBPATH} >/dev/null
        git fetch --prune
        git push --prune ${SUBMIRROR} +refs/remotes/origin/*:refs/heads/* +refs/tags/*:refs/tags/*
    popd >/dev/null
done

popd >/dev/null
