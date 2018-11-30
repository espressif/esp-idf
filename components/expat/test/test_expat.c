// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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

#include <expat.h>
#include <string.h>
#include "unity.h"

typedef struct {
    int depth;
    char output[512];
    int output_off;
} user_data_t;

static void insert_space(user_data_t *user_data)
{
    const char align_str[] = "    ";

    TEST_ASSERT(sizeof(user_data->output) >= user_data->output_off);
    user_data->output[user_data->output_off++] = '\n';

    for (int i = 0; i < user_data->depth; i++) {
        for (int j = 0; j < strlen(align_str); ++j) {
            TEST_ASSERT(sizeof(user_data->output) >= user_data->output_off);
            user_data->output[user_data->output_off++] = align_str[j];
        }
    }
}

static void XMLCALL start_element(void *userData, const XML_Char *name, const XML_Char **atts)
{
    user_data_t *user_data = (user_data_t *) userData;

    insert_space(user_data);

    const int ret = snprintf(user_data->output + user_data->output_off,
            sizeof(user_data->output) - user_data->output_off,
            "<%s>", name);
    TEST_ASSERT_EQUAL(strlen(name) + 2, ret); // 2 are the tag characters: "<>"
    user_data->output_off += ret;
    ++user_data->depth;
}

static void XMLCALL end_element(void *userData, const XML_Char *name)
{
    user_data_t *user_data = (user_data_t *) userData;

    --user_data->depth;
    insert_space(user_data);

    int ret = snprintf(user_data->output + user_data->output_off, sizeof(user_data->output) - user_data->output_off,
                "</%s>", name);
    TEST_ASSERT_EQUAL(strlen(name) + 3, ret); // 3 are the tag characters: "</>"
    user_data->output_off += ret;
}

static void data_handler(void *userData, const XML_Char *s, int len)
{
    user_data_t *user_data = (user_data_t *) userData;

    insert_space(user_data);

    // s is not zero-terminated
    char tmp_str[len+1];
    strlcpy(tmp_str, s, len+1);

    int ret = snprintf(user_data->output + user_data->output_off, sizeof(user_data->output) - user_data->output_off,
                "%s", tmp_str);
    TEST_ASSERT_EQUAL(strlen(tmp_str), ret);
    user_data->output_off += ret;
}

TEST_CASE("Expat parses XML", "[expat]")
{
    const char test_in[] = "<html><title>Page title</title><body><h>header</h><ol><li>A</li>"\
                           "<li>B</li><li>C</li></ol></body></html>";
    const char test_expected[] =    "\n"\
                                    "<html>\n"\
                                    "    <title>\n"\
                                    "        Page title\n"\
                                    "    </title>\n"\
                                    "    <body>\n"\
                                    "        <h>\n"\
                                    "            header\n"\
                                    "        </h>\n"\
                                    "        <ol>\n"\
                                    "            <li>\n"\
                                    "                A\n"\
                                    "            </li>\n"\
                                    "            <li>\n"\
                                    "                B\n"\
                                    "            </li>\n"\
                                    "            <li>\n"\
                                    "                C\n"\
                                    "            </li>\n"\
                                    "        </ol>\n"\
                                    "    </body>\n"\
                                    "</html>";
    user_data_t user_data = {
        .depth = 0,
        .output = { '\0' },
        .output_off = 0
    };

    XML_Parser parser = XML_ParserCreate(NULL);
    XML_SetUserData(parser, &user_data);
    XML_SetElementHandler(parser, start_element, end_element);
    XML_SetCharacterDataHandler(parser, data_handler);

    TEST_ASSERT_NOT_EQUAL(XML_STATUS_ERROR, XML_Parse(parser, test_in, strlen(test_in), 1));
    XML_ParserFree(parser);

    TEST_ASSERT_EQUAL(0, user_data.depth); // all closing tags have been found

    TEST_ASSERT_EQUAL(strlen(test_expected), strlen(user_data.output));
    TEST_ASSERT_EQUAL_STRING(test_expected, user_data.output);
}
