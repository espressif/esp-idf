/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "esp_log.h"
#include "ble_ancs.h"

#define BLE_ANCS_TAG  "BLE_ANCS"

/*
| EventID(1 Byte) | EventFlags(1 Byte) | CategoryID(1 Byte) | CategoryCount(1 Byte) | NotificationUID(4 Bytes) |

A GATT notification delivered through the Notification Source characteristic contains the following information:
* EventID: This field informs the accessory whether the given iOS notification was added, modified, or removed. The enumerated values for this field are defined
            in EventID Values.
* EventFlags: A bitmask whose set bits inform an NC of specificities with the iOS notification. For example, if an iOS notification is considered “important”,
              the NC may want to display a more aggressive user interface (UI) to make sure the user is properly alerted. The enumerated bits for this field
              are defined in EventFlags.
* CategoryID: A numerical value providing a category in which the iOS notification can be classified. The NP will make a best effort to provide an accurate category
              for each iOS notification. The enumerated values for this field are defined in CategoryID Values.
* CategoryCount: The current number of active iOS notifications in the given category. For example, if two unread emails are sitting in a user’s email inbox, and a new
                 email is pushed to the user’s iOS device, the value of CategoryCount is 3.
* NotificationUID: A 32-bit numerical value that is the unique identifier (UID) for the iOS notification. This value can be used as a handle in commands sent to the
                   Control Point characteristic to interact with the iOS notification.
*/

char *EventID_to_String(uint8_t EventID)
{
    char *str = NULL;
    switch (EventID)
    {
        case EventIDNotificationAdded:
            str = "New message";
            break;
        case EventIDNotificationModified:
            str = "Modified message";
            break;
        case EventIDNotificationRemoved:
            str = "Removed message";
            break;
        default:
            str = "unknown EventID";
            break;
    }
    return str;
}

char *CategoryID_to_String(uint8_t CategoryID)
{
    char *Cidstr = NULL;
    switch(CategoryID) {
        case CategoryIDOther:
            Cidstr = "Other";
            break;
        case CategoryIDIncomingCall:
            Cidstr = "IncomingCall";
            break;
        case CategoryIDMissedCall:
            Cidstr = "MissedCall";
            break;
        case CategoryIDVoicemail:
            Cidstr = "Voicemail";
            break;
        case CategoryIDSocial:
            Cidstr = "Social";
            break;
        case CategoryIDSchedule:
            Cidstr = "Schedule";
            break;
        case CategoryIDEmail:
            Cidstr = "Email";
            break;
        case CategoryIDNews:
            Cidstr = "News";
            break;
        case CategoryIDHealthAndFitness:
            Cidstr = "HealthAndFitness";
            break;
        case CategoryIDBusinessAndFinance:
            Cidstr = "BusinessAndFinance";
            break;
        case CategoryIDLocation:
            Cidstr = "Location";
            break;
        case CategoryIDEntertainment:
            Cidstr = "Entertainment";
            break;
        default:
            Cidstr = "Unknown CategoryID";
            break;
    }
    return Cidstr;
}

/*
| EventID(1 Byte) | EventFlags(1 Byte) | CategoryID(1 Byte) | CategoryCount(1 Byte) | NotificationUID(4 Bytes) |
*/

void esp_receive_apple_notification_source(uint8_t *message, uint16_t message_len)
{
    if (!message || message_len < 5) {
        return;
    }

    uint8_t EventID    = message[0];
    char    *EventIDS  = EventID_to_String(EventID);
    uint8_t EventFlags = message[1];
    uint8_t CategoryID = message[2];
    char    *Cidstr    = CategoryID_to_String(CategoryID);
    uint8_t CategoryCount = message[3];
    uint32_t NotificationUID = (message[4]) | (message[5]<< 8) | (message[6]<< 16) | (message[7] << 24);
    ESP_LOGI(BLE_ANCS_TAG, "EventID:%s EventFlags:0x%x CategoryID:%s CategoryCount:%d NotificationUID:%" PRIu32, EventIDS, EventFlags, Cidstr, CategoryCount, NotificationUID);
}

void esp_receive_apple_data_source(uint8_t *message, uint16_t message_len)
{
    //esp_log_buffer_hex("data source", message, message_len);
    if (!message || message_len == 0) {
        return;
    }
    uint8_t Command_id = message[0];
    switch (Command_id)
    {
        case CommandIDGetNotificationAttributes: {
            uint32_t NotificationUID = (message[1]) | (message[2]<< 8) | (message[3]<< 16) | (message[4] << 24);
            uint32_t remian_attr_len = message_len - 5;
            uint8_t *attrs = &message[5];
            ESP_LOGI(BLE_ANCS_TAG, "recevice Notification Attributes response Command_id %d NotificationUID %" PRIu32, Command_id, NotificationUID);
            while(remian_attr_len > 0) {
                uint8_t AttributeID = attrs[0];
                uint16_t len = attrs[1] | (attrs[2] << 8);
                if(len > (remian_attr_len -3)) {
                    ESP_LOGE(BLE_ANCS_TAG, "data error");
                    break;
                }
                switch (AttributeID)
                {
                    case NotificationAttributeIDAppIdentifier:
                        esp_log_buffer_char("Identifier", &attrs[3], len);
                        break;
                    case NotificationAttributeIDTitle:
                        esp_log_buffer_char("Title", &attrs[3], len);
                        break;
                    case NotificationAttributeIDSubtitle:
                        esp_log_buffer_char("Subtitle", &attrs[3], len);
                        break;
                    case NotificationAttributeIDMessage:
                        esp_log_buffer_char("Message", &attrs[3], len);
                        break;
                    case NotificationAttributeIDMessageSize:
                        esp_log_buffer_char("MessageSize", &attrs[3], len);
                        break;
                    case NotificationAttributeIDDate:
                        //yyyyMMdd'T'HHmmSS
                        esp_log_buffer_char("Date", &attrs[3], len);
                        break;
                    case NotificationAttributeIDPositiveActionLabel:
                        esp_log_buffer_hex("PActionLabel", &attrs[3], len);
                        break;
                    case NotificationAttributeIDNegativeActionLabel:
                        esp_log_buffer_hex("NActionLabel", &attrs[3], len);
                        break;
                    default:
                        esp_log_buffer_hex("unknownAttributeID", &attrs[3], len);
                        break;
                }

                attrs += (1 + 2 + len);
                remian_attr_len -= (1 + 2 + len);
            }

            break;
        }
        case CommandIDGetAppAttributes:
            ESP_LOGI(BLE_ANCS_TAG, "recevice APP Attributes response");
            break;
        case CommandIDPerformNotificationAction:
            ESP_LOGI(BLE_ANCS_TAG, "recevice Perform Notification Action");
            break;
        default:
            ESP_LOGI(BLE_ANCS_TAG, "unknown Command ID");
            break;
    }
}

char *Errcode_to_String(uint16_t status)
{
    char *Errstr = NULL;
    switch (status) {
        case Unknown_command:
            Errstr = "Unknown_command";
            break;
        case Invalid_command:
            Errstr = "Invalid_command";
            break;
        case Invalid_parameter:
            Errstr = "Invalid_parameter";
            break;
        case Action_failed:
            Errstr = "Action_failed";
            break;
        default:
            Errstr = "unknown_failed";
            break;
    }
    return Errstr;

}
