from __future__ import unicode_literals
from pexpect import TIMEOUT
import os
import time

from ttfw_idf import Utility
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag="test_jtag_arm")
def test_examples_gcov(env, extra_data):

    rel_project_path = os.path.join('examples', 'system', 'gcov')
    dut = env.get_dut('gcov', rel_project_path)
    idf_path = dut.app.get_sdk_path()
    proj_path = os.path.join(idf_path, rel_project_path)

    with ttfw_idf.OCDProcess(os.path.join(proj_path, 'openocd.log')):
        with ttfw_idf.TelnetProcess(os.path.join(proj_path, 'telnet.log')) as telnet_p:
            dut.start_app()

            def expect_counter_output(loop, timeout=10):
                dut.expect_all('blink_dummy_func: Counter = {}'.format(loop),
                               'some_dummy_func: Counter = {}'.format(loop * 2),
                               timeout=timeout)

            expect_counter_output(0, timeout=20)
            dut.expect('Ready to dump GCOV data...', timeout=5)

            def dump_coverage(cmd):
                try:
                    telnet_p.pexpect_proc.sendline(cmd)
                    telnet_p.pexpect_proc.expect_exact('Targets connected.')
                    telnet_p.pexpect_proc.expect_exact('gcov_example_main.c.gcda')
                    telnet_p.pexpect_proc.expect_exact('gcov_example_func.c.gcda')
                    telnet_p.pexpect_proc.expect_exact('some_funcs.c.gcda')
                    telnet_p.pexpect_proc.expect_exact('Targets disconnected.')
                except TIMEOUT:
                    # Print what is happening with DUT. Limit the size if it is in loop and generating output.
                    Utility.console_log(dut.read(size=1000))
                    raise

            dump_coverage('esp gcov dump')
            dut.expect('GCOV data have been dumped.', timeout=5)
            expect_counter_output(1)
            dut.expect('Ready to dump GCOV data...', timeout=5)
            dump_coverage('esp gcov dump')
            dut.expect('GCOV data have been dumped.', timeout=5)

            for i in range(2, 6):
                expect_counter_output(i)

            for _ in range(3):
                time.sleep(1)
                # Test instant run-time dump
                dump_coverage('esp gcov')


if __name__ == '__main__':
    test_examples_gcov()
