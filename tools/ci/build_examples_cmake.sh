#!/bin/bash
#
# Build all examples from the examples directory, in BUILD_PATH to
# ensure they can run when copied to a new directory.
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
[ -z ${BUILD_PATH} ] && die "BUILD_PATH is not set"
[ -z ${IDF_TARGET} ] && die "IDF_TARGET is not set"
[ -d ${LOG_PATH} ] || mkdir -p ${LOG_PATH}
[ -d ${BUILD_PATH} ] || mkdir -p ${BUILD_PATH}

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

export REALPATH=realpath
if [ "$(uname -s)" = "Darwin" ]; then
    export REALPATH=grealpath
fi

# Convert LOG_PATH and BUILD_PATH to relative, to make the json file less verbose.
LOG_PATH=$(${REALPATH} --relative-to ${IDF_PATH} ${LOG_PATH})
BUILD_PATH=$(${REALPATH} --relative-to ${IDF_PATH} ${BUILD_PATH})

ALL_BUILD_LIST_JSON="${BUILD_PATH}/list.json"
JOB_BUILD_LIST_JSON="${BUILD_PATH}/list_job_${CI_NODE_INDEX}.json"
mkdir -p "${BUILD_PATH}/example_builds"

echo "build_examples running for target $IDF_TARGET"

cd ${IDF_PATH}

# This part of the script produces the same result for all the example build jobs. It may be moved to a separate stage
# (pre-build) later, then the build jobs will receive ${BUILD_LIST_JSON} file as an artifact.

# If changing the work-dir or build-dir format, remember to update the "artifacts" in gitlab-ci configs, and IDFApp.py.

${IDF_PATH}/tools/find_apps.py examples \
    -vv \
    --format json \
    --build-system cmake \
    --target ${IDF_TARGET} \
    --recursive \
    --exclude examples/build_system/idf_as_lib \
    --work-dir "${BUILD_PATH}/@f/@w/@t" \
    --build-dir build \
    --build-log "${LOG_PATH}/@f.txt" \
    --output ${ALL_BUILD_LIST_JSON} \
    --config 'sdkconfig.ci=default' \
    --config 'sdkconfig.ci.*=' \
    --config '=default' \

# --config rules above explained:
# 1. If sdkconfig.ci exists, use it build the example with configuration name "default"
# 2. If sdkconfig.ci.* exists, use it to build the "*" configuration
# 3. If none of the above exist, build the default configuration under the name "default"

# The part below is where the actual builds happen

${IDF_PATH}/tools/build_apps.py \
    -vv \
    --format json \
    --keep-going \
    --parallel-count ${CI_NODE_TOTAL} \
    --parallel-index ${CI_NODE_INDEX} \
    --output-build-list ${JOB_BUILD_LIST_JSON} \
    ${ALL_BUILD_LIST_JSON}\


# Check for build warnings
${IDF_PATH}/tools/ci/check_build_warnings.py -vv ${JOB_BUILD_LIST_JSON}
