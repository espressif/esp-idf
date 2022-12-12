# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import print_function

import logging
import os.path
import sys
import time
import traceback
from unittest.mock import MagicMock

from .. import Env

_COLOR_CODES = {
    'white': u'\033[0m',
    'red':  u'\033[31m',
    'green': u'\033[32m',
    'orange': u'\033[33m',
    'blue': u'\033[34m',
    'purple': u'\033[35m',
    'W': u'\033[0m',
    'R': u'\033[31m',
    'G': u'\033[32m',
    'O': u'\033[33m',
    'B': u'\033[34m',
    'P': u'\033[35m'
}


def _get_log_file_name():
    if Env.Env.CURRENT_LOG_FOLDER:
        file_name = os.path.join(Env.Env.CURRENT_LOG_FOLDER, 'console.txt')
    else:
        raise OSError('env log folder does not exist, will not save to log file')
    return file_name


def format_timestamp():
    ts = time.time()
    return '{}:{}'.format(time.strftime('%m-%d %H:%M:%S', time.localtime(ts)), str(ts % 1)[2:5])


def console_log(data, color='white', end='\n'):
    """
    log data to console.
    (if not flush console log, Gitlab-CI won't update logs during job execution)

    :param data: data content
    :param color: color
    """
    if color not in _COLOR_CODES:
        color = 'white'
    color_codes = _COLOR_CODES[color]
    if isinstance(data, type(b'')):
        data = data.decode('utf-8', 'replace')
    print(color_codes + data, end=end)
    if color not in ['white', 'W']:
        # reset color to white for later logs
        print(_COLOR_CODES['white'] + u'\r')
    sys.stdout.flush()
    log_data = '[{}] '.format(format_timestamp()) + data
    try:
        log_file = _get_log_file_name()
        with open(log_file, 'a+') as f:
            f.write(log_data + end)
    except OSError:
        pass


__LOADED_MODULES = dict()  # type: ignore
# we should only load one module once.
# if we load one module twice,
# python will regard the same object loaded in the first time and second time as different objects.
# it will lead to strange errors like `isinstance(object, type_of_this_object)` return False


def load_source(path, mock_missing=False):
    """
    Dynamic loading python file. Note that this function SHOULD NOT be used to replace ``import``.
    It should only be used when the package path is only available in runtime.

    :param path: The path of python file
    :param mock_missing: If True, will mock the module if the module is not found.
    :return: Loaded object
    """
    path = os.path.realpath(path)
    # load name need to be unique, otherwise it will update the already loaded module
    load_name = str(len(__LOADED_MODULES))
    try:
        return __LOADED_MODULES[path]
    except KeyError:
        folder = os.path.dirname(path)
        sys.path.append(folder)
        from importlib.machinery import SourceFileLoader
        try:
            ret = SourceFileLoader(load_name, path).load_module()
        except ModuleNotFoundError as e:
            if not mock_missing:
                raise

            # mock the missing third-party libs. Don't use it when real-testing
            while True:
                sys.modules[e.name] = MagicMock()
                logging.warning('Mocking python module %s', e.name)
                try:
                    ret = SourceFileLoader(load_name, path).load_module()
                    break
                except ModuleNotFoundError as f:
                    e = f  # another round
        except SyntaxError:
            # well, let's ignore it... non of our business
            return None
        finally:
            sys.path.remove(folder)
        __LOADED_MODULES[path] = ret
        return ret


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
