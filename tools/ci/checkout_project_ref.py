#!/usr/bin/env python

# internal use only
# called by CI jobs when it uses a project related to IDF

import argparse
import json
import os
import re
import subprocess

IDF_GIT_DESCRIBE_PATTERN = re.compile(r'^v(\d)\.(\d)')
RETRY_COUNT = 3


def get_customized_project_revision(proj_name):
    """
    get customized project revision defined in bot message
    """
    revision = ''
    customized_project_revisions = os.getenv('BOT_CUSTOMIZED_REVISION')
    if customized_project_revisions:
        customized_project_revisions = json.loads(customized_project_revisions)
    try:
        revision = customized_project_revisions[proj_name.lower()]
    except (KeyError, TypeError):
        pass
    return revision


def target_branch_candidates(proj_name):
    """
    :return: a list of target branch candidates, from highest priority to lowest priority.
    """
    candidates = [
        # branch name (or tag name) of current IDF
        os.getenv('CI_COMMIT_REF_NAME'),
        # CI_MERGE_REQUEST_TARGET_BRANCH_NAME
        os.getenv('CI_MERGE_REQUEST_TARGET_BRANCH_NAME'),
    ]
    customized_candidate = get_customized_project_revision(proj_name)
    if customized_candidate:
        # highest priority, insert to head of list
        candidates.insert(0, customized_candidate)

    # branch name read from IDF
    try:
        git_describe = subprocess.check_output(['git', 'describe', 'HEAD'])
        match = IDF_GIT_DESCRIBE_PATTERN.search(git_describe.decode())
        if match:
            major_revision = match.group(1)
            minor_revision = match.group(2)
            # release branch
            candidates.append('release/v{}.{}'.format(major_revision, minor_revision))
            # branch to match all major branches, like v3.x or v3
            candidates.append('release/v{}.x'.format(major_revision))
            candidates.append('release/v{}'.format(major_revision))
    except subprocess.CalledProcessError:
        # this should not happen as IDF should have describe message
        pass

    return [c for c in candidates if c]  # filter out null value


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('project',
                        help='the name of project')
    parser.add_argument('project_relative_path',
                        help='relative path of project to IDF repository directory')
    parser.add_argument('--customized_only', action='store_true',
                        help='Only to find customized revision')

    args = parser.parse_args()

    if args.customized_only:
        customized_revision = get_customized_project_revision(args.project)
        candidate_branches = [customized_revision] if customized_revision else []
    else:
        candidate_branches = target_branch_candidates(args.project)

    # change to project dir for checkout
    os.chdir(args.project_relative_path)

    ref_to_use = ''
    for candidate in candidate_branches:
        # check if the branch, tag or commit exists
        try:
            subprocess.check_call(['git', 'cat-file', '-t', 'origin/{}'.format(candidate)], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            ref_to_use = candidate
            break
        except subprocess.CalledProcessError:
            try:
                # For customized commits
                subprocess.check_call(['git', 'cat-file', '-t', candidate], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
                ref_to_use = candidate
                break
            except subprocess.CalledProcessError:
                pass
            continue

    if ref_to_use:
        for _ in range(RETRY_COUNT):
            # Add retry for projects with git-lfs
            try:
                subprocess.check_call(['git', 'checkout', '-f', ref_to_use], stdout=subprocess.PIPE)  # not print the stdout
                print('CI using ref {} for project {}'.format(ref_to_use, args.project))
                break
            except subprocess.CalledProcessError:
                pass
        else:
            print('Failed to use ref {} for project {}'.format(ref_to_use, args.project))
            exit(1)
    else:
        print('using default branch')
