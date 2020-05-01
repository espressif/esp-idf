from __future__ import print_function
import os.path
import sys


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


__LOADED_MODULES = dict()
# we should only load one module once.
# if we load one module twice,
# python will regard the same object loaded in the first time and second time as different objects.
# it will lead to strange errors like `isinstance(object, type_of_this_object)` return False


def load_source(path):
    """
    Dynamic loading python file. Note that this function SHOULD NOT be used to replace ``import``.
    It should only be used when the package path is only available in runtime.

    :param path: The path of python file
    :return: Loaded object
    """
    path = os.path.realpath(path)
    # load name need to be unique, otherwise it will update the already loaded module
    load_name = str(len(__LOADED_MODULES))
    try:
        return __LOADED_MODULES[path]
    except KeyError:
        try:
            dir = os.path.dirname(path)
            sys.path.append(dir)
            from importlib.machinery import SourceFileLoader
            ret = SourceFileLoader(load_name, path).load_module()
        except ImportError:
            # importlib.machinery doesn't exists in Python 2 so we will use imp (deprecated in Python 3)
            import imp
            ret = imp.load_source(load_name, path)
        finally:
            sys.path.remove(dir)
        __LOADED_MODULES[path] = ret
        return ret
