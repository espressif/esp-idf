import os
import re
import logging
from threading import Thread

import ttfw_idf
from tiny_test_fw import DUT

LOG_LEVEL = logging.DEBUG
LOGGER_NAME = "modbus_test"

# Allowed options for the test
TEST_READ_MAX_ERR_COUNT = 3         # Maximum allowed read errors during initialization
TEST_THREAD_JOIN_TIMEOUT = 60       # Test theread join timeout in seconds
TEST_EXPECT_STR_TIMEOUT = 30        # Test expect timeout in seconds
TEST_MASTER_TCP = 'mb_tcp_master'
TEST_SLAVE_TCP = 'mb_tcp_slave'

STACK_DEFAULT = 0
STACK_IPV4 = 1
STACK_IPV6 = 2
STACK_INIT = 3
STACK_CONNECT = 4
STACK_START = 5
STACK_PAR_OK = 6
STACK_PAR_FAIL = 7
STACK_DESTROY = 8

pattern_dict_slave = {STACK_IPV4: (r'.*I \([0-9]+\) example_connect: - IPv4 address: ([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}).*'),
                      STACK_IPV6: (r'.*I \([0-9]+\) example_connect: - IPv6 address: (([A-Fa-f0-9]{1,4}::?){1,7}[A-Fa-f0-9]{1,4}).*'),
                      STACK_INIT: (r'.*I \(([0-9]+)\) MB_TCP_SLAVE_PORT: (Protocol stack initialized).'),
                      STACK_CONNECT: (r'.*I\s\(([0-9]+)\) MB_TCP_SLAVE_PORT: Socket \(#[0-9]+\), accept client connection from address: '
                                      r'([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}).*'),
                      STACK_START: (r'.*I\s\(([0-9]+)\) SLAVE_TEST: (Start modbus test).*'),
                      STACK_PAR_OK: (r'.*I\s\(([0-9]+)\) SLAVE_TEST: ([A-Z]+ [A-Z]+) \([a-zA-Z0-9_]+ us\),\s'
                                     r'ADDR:([0-9]+), TYPE:[0-9]+, INST_ADDR:0x[a-zA-Z0-9]+, SIZE:[0-9]+'),
                      STACK_PAR_FAIL: (r'.*E \(([0-9]+)\) SLAVE_TEST: Response time exceeds configured [0-9]+ [ms], ignore packet.*'),
                      STACK_DESTROY: (r'.*I\s\(([0-9]+)\) SLAVE_TEST: (Modbus controller destroyed).')}

pattern_dict_master = {STACK_IPV4: (r'.*I \([0-9]+\) example_connect: - IPv4 address: ([0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}).*'),
                       STACK_IPV6: (r'.*I \([0-9]+\) example_connect: - IPv6 address: (([A-Fa-f0-9]{1,4}::?){1,7}[A-Fa-f0-9]{1,4}).*'),
                       STACK_INIT: (r'.*I \(([0-9]+)\) MASTER_TEST: (Modbus master stack initialized).*'),
                       STACK_CONNECT: (r'.*.*I\s\(([0-9]+)\) MB_TCP_MASTER_PORT: (Connected [0-9]+ slaves), start polling.*'),
                       STACK_START: (r'.*I \(([0-9]+)\) MASTER_TEST: (Start modbus test).*'),
                       STACK_PAR_OK: (r'.*I\s\(([0-9]+)\) MASTER_TEST: Characteristic #[0-9]+ ([a-zA-Z0-9_]+)'
                                      r'\s\([a-zA-Z\%\/]+\) value = [a-zA-Z0-9\.]+ \(0x[a-zA-Z0-9]+\) read successful.*'),
                       STACK_PAR_FAIL: (r'.*E \(([0-9]+)\) MASTER_TEST: Characteristic #[0-9]+\s\(([a-zA-Z0-9_]+)\)\s'
                                        r'read fail, err = [0-9]+ \([_A-Z]+\).*'),
                       STACK_DESTROY: (r'.*I\s\(([0-9]+)\) MASTER_TEST: (Destroy master).*')}

logger = logging.getLogger(LOGGER_NAME)


class DutTestThread(Thread):
    """ Test thread class
    """
    def __init__(self, dut=None, name=None, ip_addr=None, expect=None):
        """ Initialize the thread parameters
        """
        self.tname = name
        self.dut = dut
        self.expected = expect
        self.data = None
        self.ip_addr = ip_addr
        self.test_finish = False
        self.param_fail_count = 0
        self.param_ok_count = 0
        self.test_stage = STACK_DEFAULT
        super(DutTestThread, self).__init__()

    def __enter__(self):
        logger.debug("Restart %s." % self.tname)
        # Reset DUT first
        self.dut.reset()
        # Capture output from the DUT
        self.dut.start_capture_raw_data(capture_id=self.dut.name)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        """ The exit method of context manager
        """
        if exc_type is not None or exc_value is not None:
            logger.info("Thread %s rised an exception type: %s, value: %s" % (self.tname, str(exc_type), str(exc_value)))

    def run(self):
        """ The function implements thread functionality
        """
        # Initialize slave IP for master board
        if (self.ip_addr is not None):
            self.set_ip(0)

        # Check expected strings in the listing
        self.test_start(TEST_EXPECT_STR_TIMEOUT)

        # Check DUT exceptions
        dut_exceptions = self.dut.get_exceptions()
        if "Guru Meditation Error:" in dut_exceptions:
            raise Exception("%s generated an exception: %s\n" % (str(self.dut), dut_exceptions))

        # Mark thread has run to completion without any exceptions
        self.data = self.dut.stop_capture_raw_data(capture_id=self.dut.name)

    def set_ip(self, index=0):
        """ The method to send slave IP to master application
        """
        message = r'.*Waiting IP([0-9]{1,2}) from stdin.*'
        # Read all data from previous restart to get prompt correctly
        self.dut.read()
        result = self.dut.expect(re.compile(message), TEST_EXPECT_STR_TIMEOUT)
        if int(result[0]) != index:
            raise Exception("Incorrect index of IP=%d for %s\n" % (int(result[0]), str(self.dut)))
        message = "IP%s=%s" % (result[0], self.ip_addr)
        self.dut.write(message, "\r\n", False)
        logger.debug("Sent message for %s: %s" % (self.tname, message))
        message = r'.*IP\([0-9]+\) = \[([0-9a-zA-Z\.\:]+)\] set from stdin.*'
        result = self.dut.expect(re.compile(message), TEST_EXPECT_STR_TIMEOUT)
        logger.debug("Thread %s initialized with slave IP (%s)." % (self.tname, result[0]))

    def test_start(self, timeout_value):
        """ The method to initialize and handle test stages
        """
        def handle_get_ip4(data):
            """ Handle get_ip v4
            """
            logger.debug("%s[STACK_IPV4]: %s" % (self.tname, str(data)))
            self.test_stage = STACK_IPV4

        def handle_get_ip6(data):
            """ Handle get_ip v6
            """
            logger.debug("%s[STACK_IPV6]: %s" % (self.tname, str(data)))
            self.test_stage = STACK_IPV6

        def handle_init(data):
            """ Handle init
            """
            logger.debug("%s[STACK_INIT]: %s" % (self.tname, str(data)))
            self.test_stage = STACK_INIT

        def handle_connect(data):
            """ Handle connect
            """
            logger.debug("%s[STACK_CONNECT]: %s" % (self.tname, str(data)))
            self.test_stage = STACK_CONNECT

        def handle_test_start(data):
            """ Handle connect
            """
            logger.debug("%s[STACK_START]: %s" % (self.tname, str(data)))
            self.test_stage = STACK_START

        def handle_par_ok(data):
            """ Handle parameter ok
            """
            logger.debug("%s[READ_PAR_OK]: %s" % (self.tname, str(data)))
            if self.test_stage >= STACK_START:
                self.param_ok_count += 1
            self.test_stage = STACK_PAR_OK

        def handle_par_fail(data):
            """ Handle parameter fail
            """
            logger.debug("%s[READ_PAR_FAIL]: %s" % (self.tname, str(data)))
            self.param_fail_count += 1
            self.test_stage = STACK_PAR_FAIL

        def handle_destroy(data):
            """ Handle destroy
            """
            logger.debug("%s[DESTROY]: %s" % (self.tname, str(data)))
            self.test_stage = STACK_DESTROY
            self.test_finish = True

        while not self.test_finish:
            try:
                self.dut.expect_any((re.compile(self.expected[STACK_IPV4]), handle_get_ip4),
                                    (re.compile(self.expected[STACK_IPV6]), handle_get_ip6),
                                    (re.compile(self.expected[STACK_INIT]), handle_init),
                                    (re.compile(self.expected[STACK_CONNECT]), handle_connect),
                                    (re.compile(self.expected[STACK_START]), handle_test_start),
                                    (re.compile(self.expected[STACK_PAR_OK]), handle_par_ok),
                                    (re.compile(self.expected[STACK_PAR_FAIL]), handle_par_fail),
                                    (re.compile(self.expected[STACK_DESTROY]), handle_destroy),
                                    timeout=timeout_value)
            except DUT.ExpectTimeout:
                logger.debug("%s, expect timeout on stage #%d (%s seconds)" % (self.tname, self.test_stage, timeout_value))
                self.test_finish = True


def test_check_mode(dut=None, mode_str=None, value=None):
    """ Check communication mode for dut
    """
    global logger
    try:
        opt = dut.app.get_sdkconfig()[mode_str]
        logger.debug("%s {%s} = %s.\n" % (str(dut), mode_str, opt))
        return value == opt
    except Exception:
        logger.error('ENV_TEST_FAILURE: %s: Cannot find option %s in sdkconfig.' % (str(dut), mode_str))
    return False


@ttfw_idf.idf_example_test(env_tag='Example_Modbus_TCP')
def test_modbus_communication(env, comm_mode):
    global logger

    rel_project_path = os.path.join('examples', 'protocols', 'modbus', 'tcp')
    # Get device under test. Both duts must be able to be connected to WiFi router
    dut_master = env.get_dut('modbus_tcp_master', os.path.join(rel_project_path, TEST_MASTER_TCP))
    dut_slave = env.get_dut('modbus_tcp_slave', os.path.join(rel_project_path, TEST_SLAVE_TCP))
    log_file = os.path.join(env.log_path, "modbus_tcp_test.log")
    print("Logging file name: %s" % log_file)

    try:
        # create file handler which logs even debug messages
        logger.setLevel(logging.DEBUG)
        fh = logging.FileHandler(log_file)
        fh.setLevel(logging.DEBUG)
        # set format of output for both handlers
        formatter = logging.Formatter('%(levelname)s:%(message)s')
        fh.setFormatter(formatter)
        logger.addHandler(fh)
        # create console handler
        ch = logging.StreamHandler()
        ch.setLevel(logging.INFO)
        # set format of output for both handlers
        formatter = logging.Formatter('%(levelname)s:%(message)s')
        ch.setFormatter(formatter)
        logger.addHandler(ch)

        # Check Kconfig configuration options for each built example
        if (test_check_mode(dut_master, "CONFIG_FMB_COMM_MODE_TCP_EN", "y") and
           test_check_mode(dut_slave, "CONFIG_FMB_COMM_MODE_TCP_EN", "y")):
            slave_name = TEST_SLAVE_TCP
            master_name = TEST_MASTER_TCP
        else:
            logger.error("ENV_TEST_FAILURE: IP resolver mode do not match in the master and slave implementation.\n")
            raise Exception("ENV_TEST_FAILURE: IP resolver mode do not match in the master and slave implementation.\n")
        address = None
        if test_check_mode(dut_master, "CONFIG_MB_SLAVE_IP_FROM_STDIN", "y"):
            logger.info("ENV_TEST_INFO: Set slave IP address through STDIN.\n")
            # Flash app onto DUT (Todo: Debug case when the slave flashed before master then expect does not work correctly for no reason
            dut_slave.start_app()
            dut_master.start_app()
            if test_check_mode(dut_master, "CONFIG_EXAMPLE_CONNECT_IPV6", "y"):
                address = dut_slave.expect(re.compile(pattern_dict_slave[STACK_IPV6]), TEST_EXPECT_STR_TIMEOUT)
            else:
                address = dut_slave.expect(re.compile(pattern_dict_slave[STACK_IPV4]), TEST_EXPECT_STR_TIMEOUT)
            if address is not None:
                    print("Found IP slave address: %s" % address[0])
            else:
                raise Exception("ENV_TEST_FAILURE: Slave IP address is not found in the output. Check network settings.\n")
        else:
            raise Exception("ENV_TEST_FAILURE: Slave IP resolver is not configured correctly.\n")

        # Create thread for each dut
        with DutTestThread(dut=dut_master, name=master_name, ip_addr=address[0], expect=pattern_dict_master) as dut_master_thread:
            with DutTestThread(dut=dut_slave, name=slave_name, ip_addr=None, expect=pattern_dict_slave) as dut_slave_thread:

                # Start each thread
                dut_slave_thread.start()
                dut_master_thread.start()

                # Wait for threads to complete
                dut_slave_thread.join(timeout=TEST_THREAD_JOIN_TIMEOUT)
                dut_master_thread.join(timeout=TEST_THREAD_JOIN_TIMEOUT)

                if dut_slave_thread.isAlive():
                    logger.error("ENV_TEST_FAILURE: The thread %s is not completed successfully after %d seconds.\n" %
                                 (dut_slave_thread.tname, TEST_THREAD_JOIN_TIMEOUT))
                    raise Exception("ENV_TEST_FAILURE: The thread %s is not completed successfully after %d seconds.\n" %
                                    (dut_slave_thread.tname, TEST_THREAD_JOIN_TIMEOUT))

                if dut_master_thread.isAlive():
                    logger.error("TEST_FAILURE: The thread %s is not completed successfully after %d seconds.\n" %
                                 (dut_master_thread.tname, TEST_THREAD_JOIN_TIMEOUT))
                    raise Exception("TEST_FAILURE: The thread %s is not completed successfully after %d seconds.\n" %
                                    (dut_master_thread.tname, TEST_THREAD_JOIN_TIMEOUT))

                logger.info("TEST_INFO: %s error count = %d, %s error count = %d.\n" %
                            (dut_master_thread.tname, dut_master_thread.param_fail_count,
                             dut_slave_thread.tname, dut_slave_thread.param_fail_count))
                logger.info("TEST_INFO: %s ok count = %d, %s ok count = %d.\n" %
                            (dut_master_thread.tname, dut_master_thread.param_ok_count,
                             dut_slave_thread.tname, dut_slave_thread.param_ok_count))

                if ((dut_master_thread.param_fail_count > TEST_READ_MAX_ERR_COUNT) or
                   (dut_slave_thread.param_fail_count > TEST_READ_MAX_ERR_COUNT) or
                   (dut_slave_thread.param_ok_count == 0) or
                   (dut_master_thread.param_ok_count == 0)):
                    raise Exception("TEST_FAILURE: %s parameter read error(ok) count = %d(%d), %s parameter read error(ok) count = %d(%d).\n" %
                                    (dut_master_thread.tname, dut_master_thread.param_fail_count, dut_master_thread.param_ok_count,
                                     dut_slave_thread.tname, dut_slave_thread.param_fail_count, dut_slave_thread.param_ok_count))
                logger.info("TEST_SUCCESS: The Modbus parameter test is completed successfully.\n")

    finally:
        dut_master.close()
        dut_slave.close()
        logging.shutdown()


if __name__ == '__main__':
    test_modbus_communication()
