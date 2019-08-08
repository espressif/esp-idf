#!/usr/bin/env python

# internal use only
# called by CI jobs when it uses a project related to IDF

import os
import json
import argparse
import subprocess
import re


IDF_GIT_DESCRIBE_PATTERN = re.compile(r"^v(\d)\.(\d)")


def target_branch_candidates(proj_name):
    """
    :return: a list of target branch candidates, from highest priority to lowest priority.
    """
    candidates = [
        # branch name (or tag name) of current IDF
        os.getenv("CI_COMMIT_REF_NAME"),
        # CI_MERGE_REQUEST_TARGET_BRANCH_NAME
        os.getenv("CI_MERGE_REQUEST_TARGET_BRANCH_NAME"),
    ]
    # revision defined in bot message
    customized_project_revisions = os.getenv("BOT_CUSTOMIZED_REVISION")
    if customized_project_revisions:
        customized_project_revisions = json.loads(customized_project_revisions)
    try:
        ref_to_use = customized_project_revisions[proj_name.lower()]
        # highest priority, insert to head of list
        candidates.insert(0, ref_to_use)
    except (KeyError, TypeError):
        pass
    # branch name read from IDF
    try:
        git_describe = subprocess.check_output(["git", "describe", "--tags", "HEAD"])
        match = IDF_GIT_DESCRIBE_PATTERN.search(git_describe.decode())
        if match:
            major_revision = match.group(1)
            minor_revision = match.group(2)
            # release branch
            candidates.append("release/v{}.{}".format(major_revision, minor_revision))
            # branch to match all major branches, like v3.x or v3
            candidates.append("release/v{}.x".format(major_revision))
            candidates.append("release/v{}".format(major_revision))
    except subprocess.CalledProcessError:
        # this should not happen as IDF should have describe message
        pass

    return [c for c in candidates if c]  # filter out null value


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("project",
                        help="the name of project")
    parser.add_argument("project_relative_path",
                        help="relative path of project to IDF repository directory")

    args = parser.parse_args()

    candidate_branches = target_branch_candidates(args.project)

    # change to project dir for checkout
    os.chdir(args.project_relative_path)

    for candidate in candidate_branches:
        try:
            subprocess.check_call(["git", "checkout", candidate])
            print("CI using ref {} for project {}".format(candidate, args.project))
            break
        except subprocess.CalledProcessError:
            pass
    else:
        print("using default branch")
