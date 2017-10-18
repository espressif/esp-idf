#!/usr/bin/env python

# internal use only
# called by CI jobs when it uses a project related to IDF

import os
import json
import argparse
import subprocess


def checkout_branch(proj_name, customized_revision, default_ref_name):
    try:
        ref_to_use = customized_revision[proj_name.lower()]
    except (KeyError, TypeError):
        ref_to_use = default_ref_name

    try:
        subprocess.check_call(["git", "checkout", ref_to_use])
        print("CI using ref {} for project {}".format(ref_to_use, proj_name))
    except subprocess.CalledProcessError:
        print("using default branch")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("project",
                        help="the name of project")

    args = parser.parse_args()
    project_name = args.project

    customized_project_revisions = os.getenv("BOT_CUSTOMIZED_REVISION")
    if customized_project_revisions:
        customized_project_revisions = json.loads(customized_project_revisions)
    ci_ref_name = os.getenv("CI_COMMIT_REF_NAME")

    checkout_branch(project_name, customized_project_revisions, ci_ref_name)
