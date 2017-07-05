#!/bin/bash
#
# Short script to verify behaviour of configure_ci_environment.sh
#
#
cd $(dirname $0)  # make dir

touch .gitmodules  # dummy file

# $1 - branch name
# $2 - 1 if public, empty if private
function assert_branch_public()
{
    (
        CI_COMMIT_REF_NAME=$1
        set -e
        source ./configure_ci_environment.sh
        [[ $IS_PUBLIC = $2 ]] || exit 1
    ) || ( echo "Expected $1 public=$2. Failing" && exit 1 )
}

assert_branch_public master 1
assert_branch_public release/v3.0 1
assert_branch_public release/invalid
assert_branch_public bugfix/invalid
assert_branch_public v1.0 1
assert_branch_public v1.0.0 1
assert_branch_public v50.50.50 1
assert_branch_public v1.2-rc77 1
assert_branch_public v1.2.3-rc1 1
assert_branch_public v1.2.3invalid

rm -f .gitmodules

