#!/usr/bin/env python

import re
import os
import sys
import glob

try:
    import IDF
except ImportError:
    # This environment variable is expected on the host machine
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)

    import IDF

import Utility


@IDF.idf_test_app_test(env_tag="test_jtag_arm")
def test_startup(env, extra_data):
    config_files = glob.glob(os.path.join(os.path.dirname(__file__), "sdkconfig.ci.*"))
    config_names = [s.replace("sdkconfig.ci.", "") for s in config_files]
    for name in config_names:
        Utility.console_log("Checking config \"{}\"... ".format(name), end="")
        dut = env.get_dut("startup", "tools/test_apps/startup", app_config_name=name)
        dut.start_app()
        dut.expect("app_main running")
        env.close_dut(dut.name)
        Utility.console_log("done")


if __name__ == '__main__':
    test_startup()
