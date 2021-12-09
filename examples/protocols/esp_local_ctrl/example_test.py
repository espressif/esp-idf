from __future__ import unicode_literals
import os
import re
import sys
import ttfw_idf


@ttfw_idf.idf_example_test(env_tag='Example_WIFI')
def test_examples_esp_local_ctrl(env, extra_data):

    rel_project_path = os.path.join('examples', 'protocols', 'esp_local_ctrl')
    dut = env.get_dut('esp_local_ctrl', rel_project_path)
    idf_path = dut.app.get_sdk_path()
    dut.start_app()

    dut_ip = dut.expect(re.compile(r'esp_netif_handlers: sta ip: (\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3})'))[0]
    dut.expect('esp_https_server: Starting server')
    dut.expect('esp_https_server: Server listening on port 443')
    dut.expect('control: esp_local_ctrl service started with name : my_esp_ctrl_device')

    def dut_expect_read():
        dut.expect('control: Reading property : timestamp (us)')
        dut.expect('control: Reading property : property1')
        dut.expect('control: Reading property : property2')
        dut.expect('control: Reading property : property3')

    # Running mDNS services in docker is not a trivial task. Therefore, the script won't connect to the host name but
    # to IP address. However, the certificates were generated for the host name and will be rejected.
    cmd = ' '.join([sys.executable, os.path.join(idf_path, rel_project_path, 'scripts/esp_local_ctrl.py'),
                    '--sec_ver 0',
                    '--name', dut_ip,
                    '--dont-check-hostname'])  # don't reject the certificate because of the hostname
    esp_local_ctrl_log = os.path.join(idf_path, rel_project_path, 'esp_local_ctrl.log')
    with ttfw_idf.CustomProcess(cmd, esp_local_ctrl_log) as ctrl_py:

        def expect_properties(prop1, prop3):
            dut_expect_read()
            ctrl_py.pexpect_proc.expect_exact('==== Available Properties ====')
            ctrl_py.pexpect_proc.expect(re.compile(r'S.N. Name\s+Type\s+Flags\s+Value'))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 1\] timestamp \(us\)\s+TIME\(us\)\s+Read-Only\s+\d+'))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 2\] property1\s+INT32\s+{}'.format(prop1)))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 3\] property2\s+BOOLEAN\s+Read-Only\s+(True)|(False)'))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 4\] property3\s+STRING\s+{}'.format(prop3)))
            ctrl_py.pexpect_proc.expect_exact('Select properties to set (0 to re-read, \'q\' to quit) :')

        property1 = 123456789
        property3 = ''

        ctrl_py.pexpect_proc.expect_exact('Connecting to {}'.format(dut_ip))
        dut.expect('esp_https_server: performing session handshake', timeout=60)
        expect_properties(property1, property3)

        ctrl_py.pexpect_proc.sendline('1')
        ctrl_py.pexpect_proc.expect_exact('Enter value to set for property (timestamp (us)) :')
        ctrl_py.pexpect_proc.sendline('2')
        ctrl_py.pexpect_proc.expect_exact('Failed to set values!')
        dut.expect('control: timestamp (us) is read-only')
        expect_properties(property1, property3)

        property1 = 638
        ctrl_py.pexpect_proc.sendline('2')
        ctrl_py.pexpect_proc.expect_exact('Enter value to set for property (property1) :')
        ctrl_py.pexpect_proc.sendline(str(property1))
        dut.expect('control: Setting property1 value to {}'.format(property1))
        expect_properties(property1, property3)

        property3 = 'test'
        ctrl_py.pexpect_proc.sendline('4')
        ctrl_py.pexpect_proc.expect_exact('Enter value to set for property (property3) :')
        ctrl_py.pexpect_proc.sendline(property3)
        dut.expect('control: Setting property3 value to {}'.format(property3))
        expect_properties(property1, property3)

        ctrl_py.pexpect_proc.sendline('q')
        ctrl_py.pexpect_proc.expect_exact('Quitting...')


if __name__ == '__main__':
    test_examples_esp_local_ctrl()
