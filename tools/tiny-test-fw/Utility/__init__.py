import sys


_COLOR_CODES = {
    "white": '\033[0m',
    "red":  '\033[31m',
    "green": '\033[32m',
    "orange": '\033[33m',
    "blue": '\033[34m',
    "purple": '\033[35m',
    "W": '\033[0m',
    "R":  '\033[31m',
    "G": '\033[32m',
    "O": '\033[33m',
    "B": '\033[34m',
    "P": '\033[35m'
}


def console_log(data, color="white"):
    """
    log data to console.
    (if not flush console log, Gitlab-CI won't update logs during job execution)

    :param data: data content
    :param color: color
    """
    if color not in _COLOR_CODES:
        color = "white"
    color_codes = _COLOR_CODES[color]
    print(color_codes + data)
    if color not in ["white", "W"]:
        # reset color to white for later logs
        print(_COLOR_CODES["white"] + "\r")
    sys.stdout.flush()