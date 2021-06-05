#
# Copyright 2021 Espressif Systems (Shanghai) CO., LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import logging
import re
import time

from pygdbmi.gdbcontroller import DEFAULT_GDB_TIMEOUT_SEC, GdbController

from . import ESPCoreDumpError


class EspGDB(object):
    def __init__(self, gdb_path, gdb_cmds, core_filename, prog_filename, timeout_sec=DEFAULT_GDB_TIMEOUT_SEC):

        """
        Start GDB and initialize a GdbController instance
        """
        gdb_args = ['--quiet',  # inhibit dumping info at start-up
                    '--nx',  # inhibit window interface
                    '--nw',  # ignore .gdbinit
                    '--interpreter=mi2',  # use GDB/MI v2
                    '--core=%s' % core_filename]  # core file
        for c in gdb_cmds:
            if c:
                gdb_args += ['-ex', c]
        gdb_args.append(prog_filename)
        self.p = GdbController(gdb_path=gdb_path, gdb_args=gdb_args)
        self.timeout = timeout_sec

        # Consume initial output by issuing a dummy command
        self._gdbmi_run_cmd_get_responses(cmd='-data-list-register-values x pc',
                                          resp_message=None, resp_type='console', multiple=True,
                                          done_message='done', done_type='result')

    def __del__(self):
        try:
            self.p.exit()
        except IndexError:
            logging.warning('Attempt to terminate the GDB process failed, because it is already terminated. Skip')

    def _gdbmi_run_cmd_get_responses(self, cmd, resp_message, resp_type, multiple=True,
                                     done_message=None, done_type=None):

        self.p.write(cmd, read_response=False)
        t_end = time.time() + self.timeout
        filtered_response_list = []
        all_responses = []
        while time.time() < t_end:
            more_responses = self.p.get_gdb_response(timeout_sec=0, raise_error_on_timeout=False)
            filtered_response_list += filter(lambda rsp: rsp['message'] == resp_message and rsp['type'] == resp_type,
                                             more_responses)
            all_responses += more_responses
            if filtered_response_list and not multiple:
                break
            if done_message and done_type and self._gdbmi_filter_responses(more_responses, done_message, done_type):
                break
        if not filtered_response_list and not multiple:
            raise ESPCoreDumpError("Couldn't find response with message '{}', type '{}' in responses '{}'".format(
                resp_message, resp_type, str(all_responses)
            ))
        return filtered_response_list

    def _gdbmi_run_cmd_get_one_response(self, cmd, resp_message, resp_type):

        return self._gdbmi_run_cmd_get_responses(cmd, resp_message, resp_type, multiple=False)[0]

    def _gdbmi_data_evaluate_expression(self, expr):
        """ Get the value of an expression, similar to the 'print' command """
        return self._gdbmi_run_cmd_get_one_response("-data-evaluate-expression \"%s\"" % expr,
                                                    'done', 'result')['payload']['value']

    def get_freertos_task_name(self, tcb_addr):
        """ Get FreeRTOS task name given the TCB address """
        try:
            val = self._gdbmi_data_evaluate_expression('(char*)((TCB_t *)0x%x)->pcTaskName' % tcb_addr)
        except (ESPCoreDumpError, KeyError):
            # KeyError is raised when "value" is not in "payload"
            return ''

        # Value is of form '0x12345678 "task_name"', extract the actual name
        result = re.search(r"\"([^']*)\"$", val)
        if result:
            return result.group(1)
        return ''

    def run_cmd(self, gdb_cmd):
        """ Execute a generic GDB console command via MI2
        """
        filtered_responses = self._gdbmi_run_cmd_get_responses(cmd="-interpreter-exec console \"%s\"" % gdb_cmd,
                                                               resp_message=None, resp_type='console', multiple=True,
                                                               done_message='done', done_type='result')
        return ''.join([x['payload'] for x in filtered_responses]) \
            .replace('\\n', '\n') \
            .replace('\\t', '\t') \
            .rstrip('\n')

    def get_thread_info(self):
        """ Get information about all threads known to GDB, and the current thread ID """
        result = self._gdbmi_run_cmd_get_one_response('-thread-info', 'done', 'result')['payload']
        current_thread_id = result['current-thread-id']
        threads = result['threads']
        return threads, current_thread_id

    def switch_thread(self, thr_id):
        """ Tell GDB to switch to a specific thread, given its ID """
        self._gdbmi_run_cmd_get_one_response('-thread-select %s' % thr_id, 'done', 'result')

    @staticmethod
    def _gdbmi_filter_responses(responses, resp_message, resp_type):
        return list(filter(lambda rsp: rsp['message'] == resp_message and rsp['type'] == resp_type, responses))

    @staticmethod
    def gdb2freertos_thread_id(gdb_target_id):
        """ Convert GDB 'target ID' to the FreeRTOS TCB address """
        return int(gdb_target_id.replace('process ', ''), 0)
