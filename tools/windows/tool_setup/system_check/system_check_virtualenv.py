#!/usr/bin/env python

import sys

expected_executable = sys.argv[1]
active_executable = sys.executable
if expected_executable != active_executable:
    print('Failure. Expected executable does not match current executable.')
    print('Expected:', expected_executable)
    print('Active: ', active_executable)
    sys.exit(1)
