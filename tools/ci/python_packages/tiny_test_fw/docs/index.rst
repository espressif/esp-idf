.. TinyTestFW documentation master file, created by
   sphinx-quickstart on Thu Sep 21 20:19:12 2017.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to TinyTestFW's documentation!
======================================

We have a lot of test which depends on interact with DUT via communication port.
Usually we send command to the port and then check response to see if the test succeed.
TinyTestFW is designed for such scenarios.
It supports ESP-IDF applications and can be adapted to other applications by writing new bundles.

Example
-------

Let's first check a simple example::

    import re
    import os
    import sys
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path:
        sys.path.insert(0, test_fw_path)

    import TinyFW
    from IDF import IDFApp, IDFDUT


    @TinyFW.test_method(app=IDFApp.Example, dut=IDFDUT.IDFDUT, env_tag="Example_WIFI",
                        chip="ESP32", module="examples", execution_time=1)
    def test_examples_protocol_https_request(env, extra_data):
        """
        steps: |
          1. join AP
          2. connect to www.howsmyssl.com:443
          3. send http request
        """
        dut1 = env.get_dut("https_request", "examples/protocols/https_request")
        dut1.start_app()
        dut1.expect("Connecting to www.howsmyssl.com:443", timeout=30)
        dut1.expect("Performing the SSL/TLS handshake")
        dut1.expect("Certificate verified.", timeout=15)
        dut1.expect_all(re.compile(r"Cipher suite is TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256"),
                        "Reading HTTP response",
                        timeout=20)
        dut1.expect(re.compile(r"Completed (\d) requests"))


    if __name__ == '__main__':
        TinyFW.set_default_config(env_config_file="EnvConfigTemplate.yml")
        test_examples_protocol_https_request()


SOP for adding test cases
-------------------------

1. Import test framework:
^^^^^^^^^^^^^^^^^^^^^^^^^

* We assume ``TEST_FW_PATH`` is pre-defined before running the tests
* Then we can import python packages and files from ``TEST_FW_PATH``

2. Define test case:
^^^^^^^^^^^^^^^^^^^^

1. Define test case ``test_xxx(env, extra_data)``
    * env: instance of test env, see :doc:`Test Env <Env>` for details
    * extra_data: extra data passed from test case caller
2. Add decorator for test case
    * add decorator ``TinyFW.test_method`` to test method
    * define default case configs and filters in decorator, see :doc:`TinyFW.test_method <TinyFW>`

3. Execute test cases:
^^^^^^^^^^^^^^^^^^^^^^

* define in ``main`` section and execute from this file
    1. set preset configs(optional). If the config is not define in case decorator, it will use the preset configs.
    2. call test case method:
        * if you don't pass any arguments, it will use default values
        * you can pass ``extra_data`` to test case by adding ``extra_data=some_data`` as kwarg of test case method.
          default value for extra_data is None.
        * you can overwrite test case config by adding them as kwarg of test case method.
          It will overwrite preset configs and case default configs.

    Examples::

        test_examples_protocol_https_request(extra_data=["data1", "data2"], dut=SomeOtherDUT, env_tag="OtherEnv")

* or, use ``runner`` to execute. see :doc:`runner <Runner>` for details

Test FW features
----------------

1. Test Environment:
    1. DUT: DUT class provides methods to interact with DUT
        * read/write through port
        * expect method which supports expect one or multiple string or RegEx
        * tool methods provided by the tool bundle, like ``start_app``, ``reset``
    2. App:
        * provide some specific features to the test application of DUT, for example:
            * SDK path
            * SDK tools
            * application information like partition table, download configs
    3. Environment Configs:
        * support get env configs from config file or auto-detect from current PC
        * provide ``get_variable`` method to get variables
2. Allow to customize components (DUT, App) to support different devices
3. Integrate to CI:
    * provide interfaces for Gitlab-CI
    * provide ``search case`` and ``runner`` interfaces, able to integrate with other CI


Class Diagram
=============
.. uml::

   class BaseDUT {
   {field} app
   {method} expect
   {method} expect_any
   {method} expect_all
   {method} read
   {method} write
   {method} start_receive
   {method} stop_receive
   {method} close
   }
   class SerialDUT {
   {method} _port_read
   {method} _port_write
   {method} _port_open
   {method} _port_close
   }
   class IDFDUT {
   {method} reset
   {method} start_app
   }
   class BaseApp {
   {method} get_sdk_path
   {method} get_log_folder
   }
   class IDFApp {
   {field} flash_files
   {field} flash_settings
   {field} partition_table
   }
   class Example {
   {method} get_binary_path
   }
   class EnvConfig {
   {method} get_variable
   }
   class Env {
   {field} config
   {field} allocated_duts
   {field} app_cls
   {method} get_dut
   {method} close_dut
   {method} get_variable
   {method} get_pc_nic_info
   {method} close
   }

   SerialDUT --|> BaseDUT
   IDFDUT --|> SerialDUT
   IDFApp --|> BaseApp
   Example --|> IDFApp
   Env *-- EnvConfig
   Env *-- BaseDUT
   Env o-- BaseApp
   BaseDUT o-- BaseApp


.. toctree::
   :maxdepth: 2
   :caption: Contents:

   modules

Dependencies
============

Support for both Python2 and Python3 (tested on python 2.7.13 and 3.6.2).

The following 3rd party lib is required:

    * pyserial
    * pyyaml
    * junit_xml
    * netifaces
    * matplotlib (if use Utility.LineChart)

These libraries can be installed by running ``pip install --user -r requirements.txt`` in tiny-test-fw directory.

To build document, we need to install ``Sphinx``, ``plantweb`` and ``sphinx-rtd-theme`` (you may replace this with your own theme). ``plantweb`` requires internet access during building document.


Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
