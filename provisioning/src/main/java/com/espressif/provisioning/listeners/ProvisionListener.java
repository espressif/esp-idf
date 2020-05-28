// Copyright 2020 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package com.espressif.provisioning.listeners;

import com.espressif.provisioning.ESPConstants;

/**
 * Interface for provisioning callbacks.
 */
public interface ProvisionListener {

    /**
     * Called when session creation is failed.
     *
     * @param e Exception
     */
    void createSessionFailed(Exception e);

    /**
     * Called when Wi-Fi credentials successfully sent to the device.
     */
    void wifiConfigSent();

    /**
     * Called when Wi-Fi credentials failed to send to the device.
     *
     * @param e Exception
     */
    void wifiConfigFailed(Exception e);

    /**
     * Called when Wi-Fi credentials successfully applied to the device.
     */
    void wifiConfigApplied();

    /**
     * Called when Wi-Fi credentials failed to apply to the device.
     *
     * @param e Exception
     */
    void wifiConfigApplyFailed(Exception e);

    /**
     * Callback for giving provision status update.
     *
     * @param failureReason Failure reason received form device.
     */
    void provisioningFailedFromDevice(ESPConstants.ProvisionFailureReason failureReason);

    /**
     * Called when device is provisioned successfully.
     */
    void deviceProvisioningSuccess();

    /**
     * Called when provisioning is failed.
     *
     * @param e Exception
     */
    void onProvisioningFailed(Exception e);
}
