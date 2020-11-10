// Copyright 2010-2016 Espressif Systems (Shanghai) PTE LTD
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

#ifndef _SOC_BB_REG_H_
#define _SOC_BB_REG_H_

/* Some of the baseband control registers.
 * PU/PD fields defined here are used in sleep related functions.
 */

#define BBPD_CTRL (DR_REG_BB_BASE + 0x0054)
#define BB_FFT_FORCE_PU (BIT(3))
#define BB_FFT_FORCE_PU_M (BIT(3))
#define BB_FFT_FORCE_PU_V 1
#define BB_FFT_FORCE_PU_S 3
#define BB_FFT_FORCE_PD (BIT(2))
#define BB_FFT_FORCE_PD_M (BIT(2))
#define BB_FFT_FORCE_PD_V 1
#define BB_FFT_FORCE_PD_S 2
#define BB_DC_EST_FORCE_PU (BIT(1))
#define BB_DC_EST_FORCE_PU_M (BIT(1))
#define BB_DC_EST_FORCE_PU_V 1
#define BB_DC_EST_FORCE_PU_S 1
#define BB_DC_EST_FORCE_PD (BIT(0))
#define BB_DC_EST_FORCE_PD_M (BIT(0))
#define BB_DC_EST_FORCE_PD_V 1
#define BB_DC_EST_FORCE_PD_S 0


#endif /* _SOC_BB_REG_H_ */
