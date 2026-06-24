# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from esp_pylib.errors import FatalError

__all__ = [
    'FatalError',
    'InconsistentFATAttributes',
    'LowerCaseException',
    'NoFreeClusterException',
    'NotInitialized',
    'TooLongNameException',
    'WLNotInitialized',
    'WriteDirectoryException',
]


class WriteDirectoryException(Exception):
    """
    Exception is raised when the user tries to write the content into the directory instead of file
    """

    pass


class NoFreeClusterException(Exception):
    """
    Exception is raised when the user tries allocate cluster but no free one is available
    """

    pass


class LowerCaseException(Exception):
    """
    Exception is raised when the user tries to write file or directory with lower case
    """

    pass


class TooLongNameException(Exception):
    """
    Exception is raised when long name support is not enabled and user tries to write file longer then allowed
    """

    pass


class NotInitialized(Exception):
    """
    Exception is raised when the user tries to access not initialized property
    """

    pass


class WLNotInitialized(Exception):
    """
    Exception is raised when the user tries to write fatfs not initialized with wear levelling
    """

    pass


class InconsistentFATAttributes(Exception):
    """
    Caused by e.g. wrong number of clusters for given FAT type
    """

    pass
