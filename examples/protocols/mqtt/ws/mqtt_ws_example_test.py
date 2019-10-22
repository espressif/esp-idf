from __future__ import print_function
from __future__ import unicode_literals
from builtins import str
import re
import os
import sys
import paho.mqtt.client as mqtt
from threading import Thread, Event


try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except Exception:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

import DUT

event_client_connected = Event()
event_stop_client = Event()
event_client_received_correct = Event()
message_log = ""


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    event_client_connected.set()
    client.subscribe("/topic/qos0")


def mqtt_client_task(client):
    while not event_stop_client.is_set():
        client.loop()


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global message_log
    payload = msg.payload.decode()
    if not event_client_received_correct.is_set() and payload == "data":
        client.publish("/topic/qos0", "data_to_esp32")
        if msg.topic == "/topic/qos0" and payload == "data":
            event_client_received_correct.set()
    message_log += "Received data:" + msg.topic + " " + payload + "\n"


@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_mqtt_ws(env, extra_data):
    broker_url = ""
    broker_port = 0
    """
    steps: |
      1. join AP and connects to ws broker
      2. Test connects a client to the same broker
      3. Test evaluates it received correct qos0 message
      4. Test ESP32 client received correct qos0 message
    """
    dut1 = env.get_dut("mqtt_websocket", "examples/protocols/mqtt/ws", dut_class=ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mqtt_websocket.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("mqtt_websocket_bin_size", "{}KB".format(bin_size // 1024))
    IDF.check_performance("mqtt_websocket_size", bin_size // 1024)
    # Look for host:port in sdkconfig
    try:
        value = re.search(r'\:\/\/([^:]+)\:([0-9]+)', dut1.app.get_sdkconfig()["CONFIG_BROKER_URI"])
        broker_url = value.group(1)
        broker_port = int(value.group(2))
    except Exception:
        print('ENV_TEST_FAILURE: Cannot find broker url in sdkconfig')
        raise
    client = None
    # 1. Test connects to a broker
    try:
        client = mqtt.Client(transport="websockets")
        client.on_connect = on_connect
        client.on_message = on_message
        print("Connecting...")
        client.connect(broker_url, broker_port, 60)
    except Exception:
        print("ENV_TEST_FAILURE: Unexpected error while connecting to broker {}: {}:".format(broker_url, sys.exc_info()[0]))
        raise
    # Starting a py-client in a separate thread
    thread1 = Thread(target=mqtt_client_task, args=(client,))
    thread1.start()
    try:
        print("Connecting py-client to broker {}:{}...".format(broker_url, broker_port))
        if not event_client_connected.wait(timeout=30):
            raise ValueError("ENV_TEST_FAILURE: Test script cannot connect to broker: {}".format(broker_url))
        dut1.start_app()
        try:
            ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
            print("Connected to AP with IP: {}".format(ip_address))
        except DUT.ExpectTimeout:
            print('ENV_TEST_FAILURE: Cannot connect to AP')
            raise
        print("Checking py-client received msg published from esp...")
        if not event_client_received_correct.wait(timeout=30):
            raise ValueError('Wrong data received, msg log: {}'.format(message_log))
        print("Checking esp-client received msg published from py-client...")
        dut1.expect(re.compile(r"DATA=data_to_esp32"), timeout=30)
    finally:
        event_stop_client.set()
        thread1.join()


if __name__ == '__main__':
    test_examples_protocol_mqtt_ws()
