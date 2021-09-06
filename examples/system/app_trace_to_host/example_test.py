from __future__ import unicode_literals
import os
import re
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag="test_jtag_arm")
def test_examples_app_trace_to_host(env, extra_data):

    rel_project_path = os.path.join('examples', 'system', 'app_trace_to_host')
    dut = env.get_dut('app_trace_to_host', rel_project_path)
    idf_path = dut.app.get_sdk_path()
    proj_path = os.path.join(idf_path, rel_project_path)
    oocd_log_path = os.path.join(proj_path, 'openocd.log')

    with ttfw_idf.OCDProcess(oocd_log_path):
        with ttfw_idf.TelnetProcess(os.path.join(proj_path, 'telnet.log')) as telnet_p:
            dut.start_app()
            dut.expect_all('example: Enabling ADC1 on channel 6 / GPIO34.',
                           'example: Enabling CW generator on DAC channel 1',
                           'example: Custom divider of RTC 8 MHz clock has been set.',
                           'example: Sampling ADC and sending data to the host...',
                           re.compile(r'example: Collected \d+ samples in 20 ms.'),
                           'example: Sampling ADC and sending data to the UART...',
                           re.compile(r'example: Sample:\d, Value:\d+'),
                           re.compile(r'example: Collected \d+ samples in 20 ms.'),
                           timeout=20)

            telnet_p.pexpect_proc.sendline('esp apptrace start file://adc.log 0 9000 5 0 0')
            telnet_p.pexpect_proc.expect_exact('App trace params: from 2 cores, size 9000 bytes, '
                                               'stop_tmo 5 s, poll period 0 ms, wait_rst 0, skip 0 bytes')
            telnet_p.pexpect_proc.expect_exact('Targets connected.')
            telnet_p.pexpect_proc.expect_exact('Targets disconnected.')
            telnet_p.pexpect_proc.expect_exact('Tracing is STOPPED. Size is 9000 of 9000 @')
            telnet_p.pexpect_proc.expect_exact('Data: blocks incomplete 0, lost bytes: 0')

    with open(oocd_log_path) as oocd_log:
        cores = 1 if dut.app.get_sdkconfig().get('CONFIG_FREERTOS_UNICORE', '').replace('"','') == 'y' else 2
        params_str = 'App trace params: from {} cores'.format(cores)
        for line in oocd_log:
            if params_str in line:
                break
        else:
            raise RuntimeError('"{}" could not be found in {}'.format(params_str, oocd_log_path))

    with ttfw_idf.CustomProcess(' '.join([os.path.join(idf_path, 'tools/esp_app_trace/logtrace_proc.py'),
                                          'adc.log',
                                          os.path.join(dut.app.get_binary_path(rel_project_path),
                                                       'app_trace_to_host.elf')]),
                                logfile='logtrace_proc.log') as logtrace:
        logtrace.pexpect_proc.expect_exact('Parse trace file')
        logtrace.pexpect_proc.expect_exact('Parsing completed.')
        logtrace.pexpect_proc.expect_exact('====================================================================')
        logtrace.pexpect_proc.expect(re.compile(r'example: Sample:\d+, Value:\d+'))
        logtrace.pexpect_proc.expect_exact('====================================================================')
        logtrace.pexpect_proc.expect(re.compile(r'Log records count: \d+'))


if __name__ == '__main__':
    test_examples_app_trace_to_host()
