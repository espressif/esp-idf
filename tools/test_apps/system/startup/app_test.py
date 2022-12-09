#!/usr/bin/env python

import os
import glob
import ttfw_idf
from tiny_test_fw import Utility


@ttfw_idf.idf_custom_test(env_tag="test_jtag_arm", group="test-apps")
def test_startup(env, extra_data):
    config_files = glob.glob(os.path.join(os.path.dirname(__file__), "sdkconfig.ci.*"))
    config_names = [os.path.basename(s).replace("sdkconfig.ci.", "") for s in config_files]
    for name in config_names:
        Utility.console_log("Checking config \"{}\"... ".format(name), end="")
        dut = env.get_dut("startup", "tools/test_apps/system/startup", app_config_name=name)
        dut.start_app()
        dut.expect("app_main running")
        env.close_dut(dut.name)
        Utility.console_log("done")


if __name__ == '__main__':
    test_startup()
