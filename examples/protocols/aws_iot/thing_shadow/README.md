# Amazon Web Services IoT Thing Shadow Example

This is an adaptation of the [AWS IoT C SDK](https://github.com/aws/aws-iot-device-sdk-embedded-C) "shadow_sample" example for ESP-IDF.

# Provisioning/Configuration

See the README.md in the parent directory for information about configuring the AWS IoT examples.

After following those steps, there is one additional step for this exmaple:

## Set Thing Name

For this example, you will need to set a Thing Name under `make menuconfig` -> `Example Configuration` -> `AWS IoT Thing Name`.

The Thing Name should match a Thing that you created while following the Getting Started guide (to check the Things you have registered, go t othe AWS IoT console web interface, click Registry and then click Things).

# Monitoring Thing Status

After flashing the example to your ESP32, it should connect to Amazon and start updating the example Thing's shadow.

In the ESP32's serial output, you should see the message "Update accepted" logged every couple of seconds.

You can monitor the Thing status from the AWS IoT console web interface:

* On the left-hand toolbar, click Registry and then click Things.
* Click on the "Thing" you set up for the example.
* Click on the "Shadow" sidebar link. You should see the Thing Shadow updating regularly.
* Click on the "Activity" sidebar link to see all Thing Shadow updates in a list. You can examine each update individually.

# Troubleshooting

If you're having problems with the AWS IoT connection itself, check the Troubleshooting section of the README in the parent directory.

* If your Thing is connecting and appears to be successfully updating, but you don't see any updates in the AWS IoT console, then check that the Thing Name in the Example Configuration under menuconfig matches exactly the thing name in AWS IoT console (including case).
