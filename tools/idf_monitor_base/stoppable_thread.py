# SPDX-FileCopyrightText: 2015-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import threading
from typing import Optional


class StoppableThread(object):
    """
    Provide a Thread-like class which can be 'cancelled' via a subclass-provided
    cancellation method.

    Can be started and stopped multiple times.

    Isn't an instance of type Thread because Python Thread objects can only be run once
    """

    def __init__(self):
        #  type: () -> None
        self._thread = None  # type: Optional[threading.Thread]

    @property
    def alive(self):
        #  type: () -> bool
        """
        Is 'alive' whenever the internal thread object exists
        """
        return self._thread is not None

    def start(self):
        #  type: () -> None
        if self._thread is None:
            self._thread = threading.Thread(target=self._run_outer)
            self._thread.start()

    def _cancel(self):
        #  type: () -> None
        pass  # override to provide cancellation functionality

    def run(self):
        #  type: () -> None
        pass  # override for the main thread behaviour

    def _run_outer(self):
        #  type: () -> None
        try:
            self.run()
        finally:
            self._thread = None

    def stop(self):
        #  type: () -> None
        if self._thread is not None:
            old_thread = self._thread
            self._thread = None
            self._cancel()
            old_thread.join()
