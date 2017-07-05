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

#pragma once

/* This header contains register/bit masks for the standard
   PHY MII registers that should be supported by all PHY models.
*/

#define MII_BASIC_MODE_CONTROL_REG          (0x0)
#define MII_SOFTWARE_RESET                     BIT(15)

#define MII_BASIC_MODE_STATUS_REG           (0x1)
#define MII_AUTO_NEGOTIATION_COMPLETE          BIT(5)
#define MII_LINK_STATUS                        BIT(2)

#define MII_PHY_IDENTIFIER_1_REG              (0x2)
#define MII_PHY_IDENTIFIER_2_REG              (0x3)

#define MII_AUTO_NEG_ADVERTISEMENT_REG      (0x4)
#define MII_ASM_DIR                            BIT(11)
#define MII_PAUSE                              BIT(10)

#define MII_PHY_LINK_PARTNER_ABILITY_REG    (0x5)
#define MII_PARTNER_ASM_DIR                    BIT(11)
#define MII_PARTNER_PAUSE                      BIT(10)
