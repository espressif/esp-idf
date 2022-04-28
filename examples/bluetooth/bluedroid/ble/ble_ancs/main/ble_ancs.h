/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//EventID values
typedef enum {
   EventIDNotificationAdded     = 0,
   EventIDNotificationModified  = 1,
   EventIDNotificationRemoved   = 2,
   //Reserved EventID values = 3–255
} esp_EventID;

//EventFlags
typedef enum {
   EventFlagSilent              = (1 << 0),
   EventFlagImportant           = (1 << 1),
   EventFlagPreExisting         = (1 << 2),
   EventFlagPositiveAction      = (1 << 3),
   EventFlagNegativeAction      = (1 << 4),
   //Reserved EventFlags   = (1 << 5)–(1 << 7
}esp_EventFlags;

// CategoryID values
typedef enum {
   CategoryIDOther              = 0,
   CategoryIDIncomingCall       = 1,
   CategoryIDMissedCall         = 2,
   CategoryIDVoicemail          = 3,
   CategoryIDSocial             = 4,
   CategoryIDSchedule           = 5,
   CategoryIDEmail              = 6,
   CategoryIDNews               = 7,
   CategoryIDHealthAndFitness   = 8,
   CategoryIDBusinessAndFinance = 9,
   CategoryIDLocation           = 10,
   CategoryIDEntertainment      = 11,
   //Reserved CategoryID values   = 12–255
} esp_CategoryID;

//CommandID values
typedef enum {
   CommandIDGetNotificationAttributes = 0,
   CommandIDGetAppAttributes          = 1,
   CommandIDPerformNotificationAction = 2,
   //Reserved CommandID values = 3–255
} esp_CommandID;

//NotificationAttributeID
typedef enum {
   NotificationAttributeIDAppIdentifier       = 0,
   NotificationAttributeIDTitle               = 1, //(Needs to be followed by a 2-bytes max length parameter)
   NotificationAttributeIDSubtitle            = 2, //(Needs to be followed by a 2-bytes max length parameter)
   NotificationAttributeIDMessage             = 3, //(Needs to be followed by a 2-bytes max length parameter)
   NotificationAttributeIDMessageSize         = 4,
   NotificationAttributeIDDate                = 5,
   NotificationAttributeIDPositiveActionLabel = 6,
   NotificationAttributeIDNegativeActionLabel = 7,
   //Reserved NotificationAttributeID values = 8–255
} esp_NotificationAttributeID;

/*
Note: The format of the NotificationAttributeIDMessageSize constant is a string that represents the integral value
of the message size. The format of the NotificationAttributeIDDate constant is a string that uses the Unicode Technical
Standard (UTS) #35 date format pattern yyyyMMdd'T'HHmmSS. The format of all the other constants in Table 3-5 are UTF-8
strings.
*/

//ActionID values
typedef enum {
   ActionIDPositive = 0,
   ActionIDNegative = 1,
   //Reserved ActionID values = 2–255
} esp_ActionID;

//AppAttributeID Values
typedef enum {
   AppAttributeIDDisplayName = 0,
   //Reserved AppAttributeID values = 1–255
} esp_AppAttributeID;

typedef struct {
    uint8_t                            noti_attribute_id;
    uint16_t                           attribute_len;
} esp_noti_attr_list_t;

typedef enum {
   Unknown_command   = (0xA0), //The commandID was not recognized by the NP.
   Invalid_command   = (0xA1), //The command was improperly formatted.
   Invalid_parameter = (0xA2), // One of the parameters (for example, the NotificationUID) does not refer to an existing object on the NP.
   Action_failed     = (0xA3), //The action was not performed
} esp_error_code;

typedef enum {
   attr_appidentifier_index   = 0, //The commandID was not recognized by the NP.
   attr_title_index,
   attr_subtitle_index,
   attr_message_index,
   attr_messagesize_index,
   attr_date_index,
   attr_positiveactionlabel_index,
   attr_negativeactionlabel_index,
} esp_attr_index;

#define ESP_NOTIFICATIONUID_LEN   4


char *EventID_to_String(uint8_t EventID);
char *CategoryID_to_String(uint8_t CategoryID);
void esp_receive_apple_notification_source(uint8_t *message, uint16_t message_len);
void esp_receive_apple_data_source(uint8_t *message, uint16_t message_len);
char *Errcode_to_String(uint16_t status);
