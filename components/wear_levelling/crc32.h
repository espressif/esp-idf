// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _crc32_H_
#define _crc32_H_

/**
* @brief This class is used to access crc32 module
*
*/
class crc32
{
public:
    static unsigned int crc32_le(unsigned int crc, unsigned char const *buf, unsigned int len);
};
#endif // _crc32_H_