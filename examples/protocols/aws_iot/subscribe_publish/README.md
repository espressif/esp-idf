# Amazon Web Services IoT MQTT Subscribe/Publish Example

This is an adaptation of the [AWS IoT C SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C) "subscribe_publish" example for ESP-IDF.

# Configuration

See the README.md in the parent directory for information about configuring the AWS IoT examples.

# Monitoring MQTT Data from the device

After flashing the example to your ESP32, it should connect to Amazon and start subscribing/publishing MQTT data.

The example code publishes MQTT data to the topic `test_topic/esp32`. Amazon provides a web interface to subscribe to MQTT topics for testing:

* On the AWS IoT console, click "MQTT Client" near the top-right.
* Click "Generate Client ID" to generate a random client ID.
* Click "Connect"

One connection succeeds, you can subscribe to the data published by the ESP32:

* Click "Subscribe to Topic"
* Enter "Subscription Topic" `test_topic/esp32`
* Click "Subscribe"

... you should see MQTT data published from the running example.

To publish data back to the device:

* Click "Publish to Topic"
* Enter "Publish Topic" `test_topic/esp32`
* Enter a message in the payload field
* Click Publish
