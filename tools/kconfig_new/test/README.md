# KConfig Tests

## confserver.py tests

Install pexpect (`pip install pexpect`).

Then run the tests manually like this:

```
./test_confserver.py --logfile tests.log
```

If a weird error message comes up from the test, check the log file (`tests.log`) which has the full interaction session (input and output) from confserver.py - sometimes the test suite misinterprets some JSON-like content in a Python error message as JSON content.

Note: confserver.py prints its error messages on stderr, to avoid overlap with JSON content on stdout. However pexpect uses a pty (virtual terminal) which can't distinguish stderr and stdout.

Test cases apply to `KConfig` config schema. Cases are listed in `testcases.txt` and are each of this form:

```
* Set TEST_BOOL, showing child items
> { "TEST_BOOL" : true }
< { "values" : { "TEST_BOOL" : true, "TEST_CHILD_STR" : "OHAI!", "TEST_CHILD_BOOL" : true }, "ranges": {"TEST_CONDITIONAL_RANGES": [0, 100]}, "visible": {"TEST_CHILD_BOOL" : true, "TEST_CHILD_STR" : true} }

```

* First line (`*`) is description
* Second line (`>`) is changes to send
* Third line (`<`) is response to expect back
* (Blank line between cases)

Test cases are run in sequence, so any test case depends on the state changes caused by all items above it.

