package com.espressif.provisioning.listeners;

import com.espressif.provisioning.DeviceConnectionEvent;

public interface EventUpdateListener {

    void onEvent(DeviceConnectionEvent deviceConnectionEvent);
}
