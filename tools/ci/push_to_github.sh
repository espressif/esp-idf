#!/bin/bash

# gitlab-ci script to push current tested revision (tag or branch) to github

set -ex

if [ -n "${CI_COMMIT_TAG}" ]; then
    # for tags
    git push github "${CI_COMMIT_TAG}"
else
    # for branches
    git push github "${CI_COMMIT_SHA}:refs/heads/${CI_COMMIT_REF_NAME}"
fi

