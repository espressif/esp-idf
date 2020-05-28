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

/**
 * Interface used for getting notified about the
 * request status sent over the Transport
 */
public interface ResponseListener {

    /***
     * Successfully sent and received response from device
     * @param returnData
     */
    void onSuccess(byte[] returnData);

    /***
     * Failed to send data or receive response from device
     * @param e
     */
    void onFailure(Exception e);
}
