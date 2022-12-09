#!/bin/bash
#
# Build unit test app
#
# Runs as part of CI process.
#

# -----------------------------------------------------------------------------
# Safety settings (see https://gist.github.com/ilg-ul/383869cbb01f61a51c4d).

if [[ ! -z ${DEBUG_SHELL} ]]
then
  set -x # Activate the expand mode if DEBUG is anything but empty.
fi

set -o errexit # Exit if command failed.
set -o pipefail # Exit if pipe failed.

export PATH="$IDF_PATH/tools/ci:$IDF_PATH/tools:$PATH"

# -----------------------------------------------------------------------------

die() {
    echo "${1:-"Unknown Error"}" 1>&2
    exit 1
}

[ -z ${IDF_PATH} ] && die "IDF_PATH is not set"
[ -z ${LOG_PATH} ] && die "LOG_PATH is not set"
[ -z ${IDF_TARGET} ] && die "IDF_TARGET is not set"
[ -d ${LOG_PATH} ] || mkdir -p ${LOG_PATH}

# Relative to IDF_PATH
# If changing the BUILD_PATH, remember to update the "artifacts" in gitlab-ci configs, and IDFApp.py.
BUILD_PATH=${IDF_PATH}/tools/unit-test-app/builds
OUTPUT_PATH=${IDF_PATH}/tools/unit-test-app/output
mkdir -p ${BUILD_PATH}/${IDF_TARGET}
mkdir -p ${OUTPUT_PATH}/${IDF_TARGET}

if [ -z ${CI_NODE_TOTAL} ]; then
    CI_NODE_TOTAL=1
    echo "Assuming CI_NODE_TOTAL=${CI_NODE_TOTAL}"
fi
if [ -z ${CI_NODE_INDEX} ]; then
    # Gitlab uses a 1-based index
    CI_NODE_INDEX=1
    echo "Assuming CI_NODE_INDEX=${CI_NODE_INDEX}"
fi


set -o nounset # Exit if variable not set.

# Convert LOG_PATH to relative, to make the json file less verbose.
LOG_PATH=$(realpath --relative-to ${IDF_PATH} ${LOG_PATH})

ALL_BUILD_LIST_JSON="${BUILD_PATH}/${IDF_TARGET}/list.json"
JOB_BUILD_LIST_JSON="${BUILD_PATH}/${IDF_TARGET}/list_job_${CI_NODE_INDEX}.json"

echo "build_unit_test running for target $IDF_TARGET"

cd ${IDF_PATH}

# This part of the script produces the same result for all the unit test app build jobs. It may be moved to a separate stage
# (pre-build) later, then the build jobs will receive ${BUILD_LIST_JSON} file as an artifact.

${IDF_PATH}/tools/find_apps.py tools/unit-test-app \
    -vv \
    --format json \
    --build-system cmake \
    --target ${IDF_TARGET} \
    --recursive \
    --build-dir "builds/@t/@w" \
    --build-log "${LOG_PATH}/@w.txt" \
    --output ${ALL_BUILD_LIST_JSON} \
    --config 'configs/*='

# The part below is where the actual builds happen

${IDF_PATH}/tools/build_apps.py \
    -vv \
    --format json \
    --keep-going \
    --parallel-count ${CI_NODE_TOTAL} \
    --parallel-index ${CI_NODE_INDEX} \
    --output-build-list ${JOB_BUILD_LIST_JSON} \
    ${ALL_BUILD_LIST_JSON}\

# Copy build artifacts to output directory
build_names=$(cd ${BUILD_PATH}/${IDF_TARGET}; find . -maxdepth 1 \! -name . -prune -type d | cut -c 3-)
for build_name in $build_names; do
    src=${BUILD_PATH}/${IDF_TARGET}/${build_name}
    dst=${OUTPUT_PATH}/${IDF_TARGET}/${build_name}
    echo "Copying artifacts from ${src} to ${dst}"

    rm -rf ${dst}
    mkdir -p ${dst}
    cp ${src}/{*.bin,*.elf,*.map,sdkconfig,flasher_args.json} ${dst}/

    mkdir -p ${dst}/bootloader
    cp ${src}/bootloader/*.bin ${dst}/bootloader/

    mkdir -p ${dst}/partition_table
    cp ${src}/partition_table/*.bin ${dst}/partition_table/
done

# Check for build warnings
${IDF_PATH}/tools/ci/check_build_warnings.py -vv ${JOB_BUILD_LIST_JSON}
