#!/bin/bash
#
# Short script that is run as part of the CI environment
# in .gitlab-ci.yml
#
# Sets up submodules in the ESP-IDF source tree
# - Ideally, this just means doing a "git submodule update"
# - But if something goes wrong we re-clone the repo from scratch
#
# This is a "best of both worlds" for GIT_STRATEGY: fetch & GIT_STRATEGY: clone
#

# -----------------------------------------------------------------------------
# Common bash
if [[ ! -z ${DEBUG} ]]; then
  set -x # Activate the expand mode if DEBUG is anything but empty.
fi

set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.
set -o nounset # Exit if variable not set.

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit ${2:-1}
}
# -----------------------------------------------------------------------------

[ -z ${CI_PROJECT_DIR} ] && die "This internal script should only be run by a Gitlab CI runner."
[ -z ${GITLAB_SSH_SERVER} ] && die "GITLAB_SSH_SERVER should be defined to run mirror-submodule-update.sh"
[ -z ${CI_REPOSITORY_URL} ] && die "CI_REPOSITORY_URL should be defined to run mirror-submodule-update.sh"
[ -z ${CI_COMMIT_SHA} ] && die "CI_COMMIT_SHA should be defined to run mirror-submodule-update.sh"
[[ ( -z ${IS_PRIVATE} ) && ( -z ${IS_PUBLIC} ) ]] && die "IS_PRIVATE or IS_PUBLIC should be defined in the CI environment."

ERR_CANNOT_UPDATE=13

SCRIPT_DIR=$(dirname -- "${0}")
update_submodules() {
    if [ "${IS_PRIVATE}" ]; then
        ${SCRIPT_DIR}/mirror-submodule-update.sh || return $?
    else
        git submodule update --init --recursive
    fi
}

del_files() {
    DELETED_FILES=$(mktemp --tmpdir -d tmp_XXXX)
    # if non-empty
    [ "$(ls -A .)" ] && ( shopt -s dotglob; mv * "${DELETED_FILES}/" )
    trap 'del_files_rollback' ERR
}
del_files_confirm() {
    [ -d "${DELETED_FILES}" ] && rm -rf "${DELETED_FILES}"
    trap ERR
}
del_files_rollback() {
    [ "$(ls -A .)" ] && [ "$(ls -A ${DELETED_FILES}/)" ] && ( shopt -s dotglob; rm -rf * )
    [ "$(ls -A ${DELETED_FILES}/)" ] && ( shopt -s dotglob; mv "${DELETED_FILES}/"* . )
    [ -d "${DELETED_FILES}" ] && rmdir "${DELETED_FILES}"
    trap ERR
}

RETRIES=10
# we're in gitlab-ci's build phase, so GET_SOURCES_ATTEMPTS doesn't apply here...

# For the first time, we try the fastest way.
for try in `seq $RETRIES`; do
    echo "Trying to add submodules to existing repo..."
    update_submodules &&
        echo "Fetch strategy submodules succeeded" &&
        exit 0

    git submodule foreach "git reset --hard HEAD && git submodule deinit --force ."
    git submodule deinit --force .
done

# Then we use the clean way.
for try in `seq $RETRIES`; do
    cd ${CI_PROJECT_DIR}  # we are probably already here but pays to be certain
    echo "Trying a clean clone of IDF..."
    del_files
    git clone ${CI_REPOSITORY_URL} . &&
        git checkout ${CI_COMMIT_SHA} &&
        update_submodules &&
        echo "Clone strategy succeeded" &&
        del_files_confirm &&
        exit 0
    ERR_RES=$?
    del_files_rollback
    echo "Clean clone failed..."
    if [ $ERR_RES -eq $ERR_CANNOT_UPDATE ]; then
        echo "###"
        echo "### If you have updated one of the submodules,"
        echo "### you have to synchronize the local mirrors manually"
        echo "###"
        echo "###     https://gitlab.espressif.cn:6688/idf/esp-idf/wikis/ci-use-guide#submodule-mirroring-for-private-branches"
        echo "###"

        die "Failed to clone repo & submodules together" $ERR_RES
    fi
done

die "Failed to clone repo & submodules together"
