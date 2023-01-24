# SPDX-FileCopyrightText: 2020-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import unicode_literals

import logging
from io import open

import pexpect
from tiny_test_fw import Utility

try:
    import debug_backend
except ImportError:
    # Exception is ignored so the package is not required for those who don't use debug utils.
    err_msg = 'Please install py_debug_backend for debug utils to work properly!'

    class debug_backend(object):  # type: ignore
        @staticmethod
        def create_oocd(*args, **kwargs):
            raise RuntimeError(err_msg)

        @staticmethod
        def create_gdb(*args, **kwargs):
            raise RuntimeError(err_msg)

try:
    from debug_backend.defs import NoGdbProcessError
except ImportError:
    # fallback for pygdbmi<0.10.0.0 which is used in the previous version of debug_backend
    try:
        from pygdbmi.gdbcontroller import NoGdbProcessError
    except ImportError:
        # If debug_backend is not installed, the exception is ignored.
        class NoGdbProcessError(ValueError):  # type: ignore
            pass


class CustomProcess(object):
    def __init__(self, cmd, logfile, verbose=True):
        self.verbose = verbose
        self.f = open(logfile, 'w')
        if self.verbose:
            Utility.console_log('Starting {} > {}'.format(cmd, self.f.name))
        self.pexpect_proc = pexpect.spawn(cmd, timeout=60, logfile=self.f, encoding='utf-8', codec_errors='ignore')

    def __enter__(self):
        return self

    def close(self):
        self.pexpect_proc.terminate(force=True)

    def __exit__(self, type, value, traceback):
        self.close()
        self.f.close()


class OCDBackend(object):
    def __init__(self, logfile_path, target, cfg_cmds=[], extra_args=[]):
        # TODO Use configuration file implied by the test environment (board)
        self.oocd = debug_backend.create_oocd(chip_name=target,
                                              oocd_exec='openocd',
                                              oocd_scripts=None,
                                              oocd_cfg_files=['board/esp32-wrover-kit-3.3v.cfg'],
                                              oocd_cfg_cmds=cfg_cmds,
                                              oocd_debug=2,
                                              oocd_args=extra_args,
                                              host='localhost',
                                              log_level=logging.DEBUG,
                                              log_stream_handler=None,
                                              log_file_handler=logging.FileHandler(logfile_path, 'w'),
                                              scope=None)
        self.oocd.start()

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        self.oocd.stop()

    def cmd_exec(self, cmd):
        return self.oocd.cmd_exec(cmd)

    def apptrace_start(self, trace_args):
        self.oocd.apptrace_start(trace_args)

    def apptrace_stop(self):
        self.oocd.apptrace_stop()

    def apptrace_wait_stop(self, tmo=10):
        self.oocd.apptrace_wait_stop(tmo=tmo)


class GDBBackend(object):
    def __init__(self, logfile_path, elffile_path, target, gdbinit_path=None, working_dir=None):
        self.gdb = debug_backend.create_gdb(chip_name=target,
                                            gdb_path='xtensa-{}-elf-gdb'.format(target),
                                            remote_target=None,
                                            extended_remote_mode=False,
                                            gdb_log_file=logfile_path,
                                            log_level=None,
                                            log_stream_handler=None,
                                            log_file_handler=None,
                                            scope=None)
        if working_dir:
            self.gdb.console_cmd_run('directory {}'.format(working_dir))
        self.gdb.exec_file_set(elffile_path)
        if gdbinit_path:
            try:
                self.gdb.console_cmd_run('source {}'.format(gdbinit_path))
            except debug_backend.defs.DebuggerTargetStateTimeoutError:
                # The internal timeout is not enough on RPI for more time consuming operations, e.g. "load".
                # So lets try to apply the commands one-by-one:
                with open(gdbinit_path, 'r') as f:
                    for line in f:
                        line = line.strip()
                        if len(line) > 0 and not line.startswith('#'):
                            self.gdb.console_cmd_run(line)
                            # Note that some commands cannot be applied with console_cmd_run, e.g. "commands"

    def __enter__(self):
        return self

    def __exit__(self, type, value, traceback):
        try:
            self.gdb.gdb_exit()
        except NoGdbProcessError as e:
            #  the debug backend can fail on gdb exit
            #  when it tries to read the response after issuing the exit command.
            Utility.console_log('Ignoring exception: {}'.format(e), 'O')
        except debug_backend.defs.DebuggerTargetStateTimeoutError:
            Utility.console_log('Ignoring timeout exception for GDB exit', 'O')
