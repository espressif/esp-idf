# Need Python 3 string formatting functions
from __future__ import print_function

import os
import re
import logging
from threading import Thread

import ttfw_idf

LOG_LEVEL = logging.DEBUG
LOGGER_NAME = "modbus_test"

# Allowed parameter reads
TEST_READ_MIN_COUNT = 10            # Minimum number of correct readings
TEST_READ_MAX_ERR_COUNT = 2         # Maximum allowed read errors during initialization

TEST_THREAD_EXPECT_TIMEOUT = 120    # Test theread expect timeout in seconds
TEST_THREAD_JOIN_TIMEOUT = 180      # Test theread join timeout in seconds

# Test definitions
TEST_MASTER_RTU = 'master_rtu'
TEST_SLAVE_RTU = 'slave_rtu'

TEST_MASTER_ASCII = 'master_ascii'
TEST_SLAVE_ASCII = 'slave_ascii'

# Define tuple of strings to expect for each DUT.
#
master_expect = ("MASTER_TEST: Modbus master stack initialized...", "MASTER_TEST: Start modbus test...", "MASTER_TEST: Destroy master...")
slave_expect = ("SLAVE_TEST: Modbus slave stack initialized.", "SLAVE_TEST: Start modbus test...", "SLAVE_TEST: Modbus controller destroyed.")

# The dictionary for expected values in listing
expect_dict_master_ok = {"START": (),
                         "READ_PAR_OK": (),
                         "ALARM_MSG": (u'7',)}

expect_dict_master_err = {"READ_PAR_ERR": (u'263', u'ESP_ERR_TIMEOUT'),
                          "READ_STK_ERR": (u'107', u'ESP_ERR_TIMEOUT')}

# The dictionary for regular expression patterns to check in listing
pattern_dict_master_ok = {"START": (r'.*I \([0-9]+\) MASTER_TEST: Start modbus test...'),
                          "READ_PAR_OK": (r'.*I\s\([0-9]+\) MASTER_TEST: Characteristic #[0-9]+ [a-zA-Z0-9_]+'
                                          r'\s\([a-zA-Z\%\/]+\) value = [a-zA-Z0-9\.\s]*\(0x[a-zA-Z0-9]+\) read successful.'),
                          "ALARM_MSG": (r'.*I \([0-9]*\) MASTER_TEST: Alarm triggered by cid #([0-9]+).')}

pattern_dict_master_err = {"READ_PAR_ERR_TOUT": (r'.*E \([0-9]+\) MASTER_TEST: Characteristic #[0-9]+'
                                                 r'\s\([a-zA-Z0-9_]+\) read fail, err = [0-9]+ \([_A-Z]+\).'),
                           "READ_STK_ERR_TOUT": (r'.*E \([0-9]+\) MB_CONTROLLER_MASTER: [a-zA-Z0-9_]+\([0-9]+\):\s'
                                                 r'SERIAL master get parameter failure error=\(0x([a-zA-Z0-9]+)\) \(([_A-Z]+)\).')}

# The dictionary for expected values in listing
expect_dict_slave_ok = {"START": (),
                        "READ_PAR_OK": (),
                        "DESTROY": ()}

# The dictionary for regular expression patterns to check in listing
pattern_dict_slave_ok = {"START": (r'.*I \([0-9]+\) SLAVE_TEST: Start modbus test...'),
                         "READ_PAR_OK": (r'.*I\s\([0-9]+\) SLAVE_TEST: [A-Z]+ READ \([a-zA-Z0-9_]+ us\),\s'
                                         r'ADDR:[0-9]+, TYPE:[0-9]+, INST_ADDR:0x[a-zA-Z0-9]+, SIZE:[0-9]+'),
                         "DESTROY": (r'.*I\s\([0-9]+\) SLAVE_TEST: Modbus controller destroyed.')}

logger = logging.getLogger(LOGGER_NAME)


class DutTestThread(Thread):
    def __init__(self, dut=None, name=None, expect=None):
        """ Initialize the thread parameters
        """
        self.tname = name
        self.dut = dut
        self.expected = expect
        self.result = False
        self.data = None
        super(DutTestThread, self).__init__()

    def run(self):
        """ The function implements thread functionality
        """
        # Must reset again as flashing during start_app will reset multiple times, causing unexpected results
        self.dut.reset()

        # Capture output from the DUT
        self.dut.start_capture_raw_data()

        # Check expected strings in the listing
        for string in self.expected:
            self.dut.expect(string, TEST_THREAD_EXPECT_TIMEOUT)

        # Check DUT exceptions
        dut_exceptions = self.dut.get_exceptions()
        if "Guru Meditation Error:" in dut_exceptions:
            raise Exception("%s generated an exception: %s\n" % (str(self.dut), dut_exceptions))

        # Mark thread has run to completion without any exceptions
        self.data = self.dut.stop_capture_raw_data()
        self.result = True


def test_filter_output(data=None, start_pattern=None, end_pattern=None):
    """Use patters to filter output
    """
    start_index = str(data).find(start_pattern)
    end_index = str(data).find(end_pattern)
    logger.debug("Listing start index= %d, end=%d" % (start_index, end_index))
    if start_index == -1 or end_index == -1:
        return data
    return data[start_index:end_index + len(end_pattern)]


def test_expect_re(data, pattern):
    """
    Check if re pattern is matched in data cache
    :param data: data to process
    :param pattern: compiled RegEx pattern
    :return: match groups if match succeed otherwise None
    """
    ret = None
    if isinstance(pattern, type(u'')):
        pattern = pattern.encode('utf-8')
    regex = re.compile(pattern)
    if isinstance(data, type(u'')):
        data = data.encode('utf-8')
    match = regex.search(data)
    if match:
        ret = tuple(None if x is None else x.decode() for x in match.groups())
        index = match.end()
    else:
        index = None
    return ret, index


def test_check_output(data=None, check_dict=None, expect_dict=None):
    """ Check output for the test
        Check log using regular expressions:
    """
    global logger
    match_count = 0
    index = 0
    data_lines = data.splitlines()
    for key, pattern in check_dict.items():
        if key not in expect_dict:
            break
        # Check pattern in the each line
        for line in data_lines:
            group, index = test_expect_re(line, pattern)
            if index is not None:
                logger.debug("Found key{%s}=%s, line: \n%s" % (key, group, line))
                if expect_dict[key] == group:
                    logger.debug("The result is correct for the key:%s, expected:%s == returned:%s" % (key, str(expect_dict[key]), str(group)))
                    match_count += 1
    return match_count


def test_check_mode(dut=None, mode_str=None, value=None):
    """ Check communication mode for dut
    """
    global logger
    try:
        opt = dut.app.get_sdkconfig()[mode_str]
        logger.info("%s {%s} = %s.\n" % (str(dut), mode_str, opt))
        return value == opt
    except Exception:
        logger.info('ENV_TEST_FAILURE: %s: Cannot find option %s in sdkconfig.' % (str(dut), mode_str))
    return False


@ttfw_idf.idf_example_test(env_tag='UT_T2_RS485')
def test_modbus_communication(env, comm_mode):
    global logger

    # Get device under test. "dut1 - master", "dut2 - slave" must be properly connected through RS485 interface driver
    dut_master = env.get_dut("modbus_master", "examples/protocols/modbus/serial/mb_master", dut_class=ttfw_idf.ESP32DUT)
    dut_slave = env.get_dut("modbus_slave", "examples/protocols/modbus/serial/mb_slave", dut_class=ttfw_idf.ESP32DUT)

    try:
        logger.debug("Environment vars: %s\r\n" % os.environ)
        logger.debug("DUT slave sdkconfig: %s\r\n" % dut_slave.app.get_sdkconfig())
        logger.debug("DUT master sdkconfig: %s\r\n" % dut_master.app.get_sdkconfig())

        # Check Kconfig configuration options for each built example
        if test_check_mode(dut_master, "CONFIG_MB_COMM_MODE_ASCII", "y") and test_check_mode(dut_slave, "CONFIG_MB_COMM_MODE_ASCII", "y"):
            logger.info("ENV_TEST_INFO: Modbus ASCII test mode selected in the configuration. \n")
            slave_name = TEST_SLAVE_ASCII
            master_name = TEST_MASTER_ASCII
        elif test_check_mode(dut_master, "CONFIG_MB_COMM_MODE_RTU", "y") and test_check_mode(dut_slave, "CONFIG_MB_COMM_MODE_RTU", "y"):
            logger.info("ENV_TEST_INFO: Modbus RTU test mode selected in the configuration. \n")
            slave_name = TEST_SLAVE_RTU
            master_name = TEST_MASTER_RTU
        else:
            logger.error("ENV_TEST_FAILURE: Communication mode in master and slave configuration don't match.\n")
            raise Exception("ENV_TEST_FAILURE: Communication mode in master and slave configuration don't match.\n")
        # Check if slave address for example application is default one to be able to communicate
        if not test_check_mode(dut_slave, "CONFIG_MB_SLAVE_ADDR", "1"):
            logger.error("ENV_TEST_FAILURE: Slave address option is incorrect.\n")
            raise Exception("ENV_TEST_FAILURE: Slave address option is incorrect.\n")

        # Flash app onto each DUT
        dut_master.start_app()
        dut_slave.start_app()

        # Create thread for each dut
        dut_master_thread = DutTestThread(dut=dut_master, name=master_name, expect=master_expect)
        dut_slave_thread = DutTestThread(dut=dut_slave, name=slave_name, expect=slave_expect)

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
            logger.error("ENV_TEST_FAILURE: The thread %s is not completed successfully after %d seconds.\n" %
                         (dut_master_thread.tname, TEST_THREAD_JOIN_TIMEOUT))
            raise Exception("ENV_TEST_FAILURE: The thread %s is not completed successfully after %d seconds.\n" %
                            (dut_master_thread.tname, TEST_THREAD_JOIN_TIMEOUT))
    finally:
        dut_master.close()
        dut_slave.close()

    # Check if test threads completed successfully and captured data
    if not dut_slave_thread.result or dut_slave_thread.data is None:
        logger.error("The thread %s was not run successfully." % dut_slave_thread.tname)
        raise Exception("The thread %s was not run successfully." % dut_slave_thread.tname)

    if not dut_master_thread.result or dut_master_thread.data is None:
        logger.error("The thread %s was not run successfully." % dut_slave_thread.tname)
        raise Exception("The thread %s was not run successfully." % dut_master_thread.tname)

    # Filter output to get test messages
    master_output = test_filter_output(dut_master_thread.data, master_expect[0], master_expect[len(master_expect) - 1])
    if master_output is not None:
        logger.info("The data for master thread is captured.")
        logger.debug(master_output)

    slave_output = test_filter_output(dut_slave_thread.data, slave_expect[0], slave_expect[len(slave_expect) - 1])
    if slave_output is not None:
        logger.info("The data for slave thread is captured.")
        logger.debug(slave_output)

    # Check if parameters are read correctly by master
    match_count = test_check_output(master_output, pattern_dict_master_ok, expect_dict_master_ok)
    if match_count < TEST_READ_MIN_COUNT:
        logger.error("There are errors reading parameters from %s, %d" % (dut_master_thread.tname, match_count))
        raise Exception("There are errors reading parameters from %s, %d" % (dut_master_thread.tname, match_count))
    logger.info("OK pattern test for %s, match_count=%d." % (dut_master_thread.tname, match_count))

    # If the test completed successfully (alarm triggered) but there are some errors during reading of parameters
    match_count = test_check_output(master_output, pattern_dict_master_err, expect_dict_master_err)
    if match_count > TEST_READ_MAX_ERR_COUNT:
        logger.error("There are errors reading parameters from %s, %d" % (dut_master_thread.tname, match_count))
        raise Exception("There are errors reading parameters from %s, %d" % (dut_master_thread.tname, match_count))
    logger.info("ERROR pattern test for %s, match_count=%d." % (dut_master_thread.tname, match_count))

    match_count = test_check_output(slave_output, pattern_dict_slave_ok, expect_dict_slave_ok)
    if match_count < TEST_READ_MIN_COUNT:
        logger.error("There are errors reading parameters from %s, %d" % (dut_slave_thread.tname, match_count))
        raise Exception("There are errors reading parameters from %s, %d" % (dut_slave_thread.tname, match_count))
    logger.info("OK pattern test for %s, match_count=%d." % (dut_slave_thread.tname, match_count))


if __name__ == '__main__':
    logger = logging.getLogger(LOGGER_NAME)
    # create file handler which logs even debug messages
    fh = logging.FileHandler('modbus_test.log')
    fh.setLevel(logging.DEBUG)
    logger.setLevel(logging.DEBUG)
    # create console handler
    ch = logging.StreamHandler()
    ch.setLevel(logging.INFO)
    # set format of output for both handlers
    formatter = logging.Formatter('%(levelname)s:%(message)s')
    ch.setFormatter(formatter)
    fh.setFormatter(formatter)
    logger.addHandler(fh)
    logger.addHandler(ch)
    logger.info("Start script %s." % os.path.basename(__file__))
    print("Logging file name: %s" % logger.handlers[0].baseFilename)
    test_modbus_communication()
    logging.shutdown()
