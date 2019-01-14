#!/usr/bin/env python
#
# A script similar to GNU envsubst, but filters out
# some CI related variables.

import os
import sys


def main():
    # Sanitize environment variables
    vars_to_remove = []
    for var_name in os.environ.keys():
        if var_name.startswith('CI_'):
            vars_to_remove.append(var_name)
    for var_name in vars_to_remove:
        del os.environ[var_name]

    for line in sys.stdin:
        if not line:
            break
        sys.stdout.write(os.path.expandvars(line))
    sys.stdout.flush()


if __name__ == '__main__':
    main()
