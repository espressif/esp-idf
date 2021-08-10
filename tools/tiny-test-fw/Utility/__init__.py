from __future__ import print_function

import sys
import traceback

_COLOR_CODES = {
    "white": u'\033[0m',
    "red":  u'\033[31m',
    "green": u'\033[32m',
    "orange": u'\033[33m',
    "blue": u'\033[34m',
    "purple": u'\033[35m',
    "W": u'\033[0m',
    "R": u'\033[31m',
    "G": u'\033[32m',
    "O": u'\033[33m',
    "B": u'\033[34m',
    "P": u'\033[35m'
}


def console_log(data, color="white", end="\n"):
    """
    log data to console.
    (if not flush console log, Gitlab-CI won't update logs during job execution)

    :param data: data content
    :param color: color
    """
    if color not in _COLOR_CODES:
        color = "white"
    color_codes = _COLOR_CODES[color]
    if isinstance(data, type(b'')):
        data = data.decode('utf-8', 'replace')
    print(color_codes + data, end=end)
    if color not in ["white", "W"]:
        # reset color to white for later logs
        print(_COLOR_CODES["white"] + u"\r")
    sys.stdout.flush()


def load_source(name, path):
    try:
        from importlib.machinery import SourceFileLoader
        return SourceFileLoader(name, path).load_module()
    except ImportError:
        # importlib.machinery doesn't exists in Python 2 so we will use imp (deprecated in Python 3)
        import imp
        return imp.load_source(name, path)


def handle_unexpected_exception(junit_test_case, exception):
    """
    Helper to log & add junit result details for an unexpected exception encountered
    when running a test case.

    Should always be called from inside an except: block
    """
    traceback.print_exc()
    # AssertionError caused by an 'assert' statement has an empty string as its 'str' form
    e_str = str(exception) if str(exception) else repr(exception)
    junit_test_case.add_failure_info('Unexpected exception: {}\n{}'.format(e_str, traceback.format_exc()))


def format_case_id(case_name, target='esp32', config='default'):
    return '{}.{}.{}'.format(target, config, case_name)
