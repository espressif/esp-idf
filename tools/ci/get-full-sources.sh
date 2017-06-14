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

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

[ -z ${CI_PROJECT_DIR} ] && die "This internal script should only be run by a Gitlab CI runner."

DELETED_FILES=$(mktemp --tmpdir -d tmp_XXXX)
del_files() {
    # if non-empty
    [ "$(ls -A .)" ] && ( shopt -s dotglob; mv * "${DELETED_FILES}/" )
}
del_files_confirm() {
    rm -rf "${DELETED_FILES}"
}

RETRIES=10
# we're in gitlab-ci's build phase, so GET_SOURCES_ATTEMPTS doesn't apply here...

# For the first time, we try the fastest way.
for try in `seq $RETRIES`; do
    echo "Trying to add submodules to existing repo..."
    git submodule update --init --recursive &&
        echo "Fetch strategy submodules succeeded" &&
        exit 0
done

# Then we use the clean way.
for try in `seq $RETRIES`; do
    cd ${CI_PROJECT_DIR}  # we are probably already here but pays to be certain
    echo "Trying a clean clone of IDF..."
    del_files
    git clone ${CI_REPOSITORY_URL} . &&
        git checkout ${CI_COMMIT_SHA} &&
        git submodule update --init --recursive &&
        echo "Clone strategy succeeded" &&
        del_files_confirm &&
        exit 0

    echo "Clean clone failed..."
done

echo "Failed to clone repo & submodules together"

exit 1
