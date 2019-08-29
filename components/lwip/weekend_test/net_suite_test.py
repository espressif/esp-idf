import re
import os
import sys
import socket
from threading import Thread, Event
import subprocess
import time
from shutil import copyfile

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

import DUT
import Utility

stop_sock_listener = Event()
stop_io_listener = Event()
sock = None
client_address = None
manual_test = False


def io_listener(dut1):
    global sock
    global client_address
    data = b''
    while not stop_io_listener.is_set():
        try:
            data = dut1.expect(re.compile(r"PacketOut:\[([a-fA-F0-9]+)\]"), timeout=5)
        except DUT.ExpectTimeout:
            continue
        if data != () and data[0] != b'':
            packet_data = data[0]
            print("Packet_data>{}<".format(packet_data))
            response = bytearray.fromhex(packet_data.decode())
            print("Sending to socket:")
            packet = ' '.join(format(x, '02x') for x in bytearray(response))
            print("Packet>{}<".format(packet))
            if client_address is not None:
                sock.sendto(response, ('127.0.0.1', 7777))


def sock_listener(dut1):
    global sock
    global client_address
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(5)
    server_address = '0.0.0.0'
    server_port = 7771
    server = (server_address, server_port)
    sock.bind(server)
    try:
        while not stop_sock_listener.is_set():
            try:
                payload, client_address = sock.recvfrom(1024)
                packet = ' '.join(format(x, '02x') for x in bytearray(payload))
                print("Received from address {}, data {}".format(client_address, packet))
                dut1.write(str.encode(packet))
            except socket.timeout:
                pass
    finally:
        sock.close()
        sock = None


@IDF.idf_example_test(env_tag="Example_WIFI")
def lwip_test_suite(env, extra_data):
    global stop_io_listener
    global stop_sock_listener
    """
    steps: |
      1. Rebuilds test suite with esp32_netsuite.ttcn
      2. Starts listeners on stdout and socket
      3. Execute ttcn3 test suite
      4. Collect result from ttcn3
    """
    dut1 = env.get_dut("net_suite", "examples/system/network_tests", dut_class=ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "net_suite.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("net_suite", "{}KB".format(bin_size // 1024))
    IDF.check_performance("net_suite", bin_size // 1024)
    dut1.start_app()
    thread1 = Thread(target=sock_listener, args=(dut1, ))
    thread2 = Thread(target=io_listener, args=(dut1, ))
    if not manual_test:
        # Variables refering to esp32 ttcn test suite
        TTCN_SRC = 'esp32_netsuite.ttcn'
        TTCN_CFG = 'esp32_netsuite.cfg'
        # System Paths
        netsuite_path = os.getenv("NETSUITE_PATH")
        netsuite_src_path = os.path.join(netsuite_path, "src")
        test_dir = os.path.dirname(os.path.realpath(__file__))
        # Building the suite
        print("Rebuilding the test suite")
        print("-------------------------")
        # copy esp32 specific files to ttcn net-suite dir
        copyfile(os.path.join(test_dir, TTCN_SRC), os.path.join(netsuite_src_path, TTCN_SRC))
        copyfile(os.path.join(test_dir, TTCN_CFG), os.path.join(netsuite_src_path, TTCN_CFG))
        proc = subprocess.Popen(['bash', '-c', 'cd ' + netsuite_src_path + ' && source make.sh'],
                                cwd=netsuite_path, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output = proc.stdout.read()
        print("Note: First build step we expect failure (titan/net_suite build system not suitable for multijob make)")
        print(output)
        proc = subprocess.Popen(['bash', '-c', 'cd ' + netsuite_src_path + ' && make'],
                                cwd=netsuite_path, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("Note: This time all dependencies shall be generated -- multijob make shall pass")
        output = proc.stdout.read()
        print(output)
        # Executing the test suite
        thread1.start()
        thread2.start()
        time.sleep(2)
        print("Executing the test suite")
        print("------------------------")
        proc = subprocess.Popen(['ttcn3_start', os.path.join(netsuite_src_path,'test_suite'), os.path.join(netsuite_src_path, TTCN_CFG)],
                                stdout=subprocess.PIPE)
        output = proc.stdout.read()
        print(output)
        print("Collecting results")
        print("------------------")
        verdict_stats = re.search('(Verdict statistics:.*)', output)
        if verdict_stats:
            verdict_stats = verdict_stats.group(1)
        else:
            verdict_stats = b""
        verdict = re.search('Overall verdict: pass', output)
        if verdict:
            print("Test passed!")
            Utility.console_log(verdict_stats, "green")
        else:
            Utility.console_log(verdict_stats, "red")
            raise ValueError('Test failed with: {}'.format(verdict_stats))
    else:
        try:
            # Executing the test suite
            thread1.start()
            thread2.start()
            time.sleep(2)
            while True:
                time.sleep(0.5)
        except KeyboardInterrupt:
            pass
    print("Executing done, waiting for tests to finish")
    print("-------------------------------------------")
    stop_io_listener.set()
    stop_sock_listener.set()
    thread1.join()
    thread2.join()


if __name__ == '__main__':
    print("Manual execution, please build and start ttcn in a separate console")
    manual_test = True
    lwip_test_suite()
