#!/usr/bin/env python

# internal use only
# check if expire time is set for all artifacts

import os

import yaml

try:
    from yaml import CLoader as Loader
except ImportError:
    from yaml import Loader as Loader

IDF_PATH = os.getenv("IDF_PATH")
if not IDF_PATH:
    print("Please set IDF_PATH before running this script")
    raise SystemExit(-1)

GITLAB_CONFIG_FILE = os.path.join(os.getenv("IDF_PATH"), ".gitlab-ci.yml")


def check_artifacts_expire_time():
    with open(GITLAB_CONFIG_FILE, "r") as f:
        config = yaml.load(f, Loader=Loader)

    errors = []

    print("expire time for jobs:")

    job_names = list(config.keys())
    job_names.sort()

    for job_name in job_names:

        if job_name.startswith("."):
            # skip ignored jobs
            continue

        try:
            if "expire_in" not in config[job_name]["artifacts"]:
                errors.append(job_name)
            else:
                print("{}: {}".format(job_name, config[job_name]["artifacts"]["expire_in"]))
        except (KeyError, TypeError):
            # this is not job, or the job does not have artifacts
            pass

    if errors:
        print("\n\nThe following jobs did not set expire time for its artifacts")
        for error in errors:
            print(error)
        raise SystemExit(-2)


if __name__ == '__main__':
    check_artifacts_expire_time()
