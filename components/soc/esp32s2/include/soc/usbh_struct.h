// Copyright 2015-2020 Espressif Systems (Shanghai) PTE LTD
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ---------------------------- Register Types ------------------------------ */

typedef union {
    struct {
        uint32_t sesreqscs: 1;
        uint32_t sesreq: 1;
        uint32_t vbvalidoven: 1;
        uint32_t vbvalidovval: 1;
        uint32_t avalidoven: 1;
        uint32_t avalidovval: 1;
        uint32_t bvalidoven: 1;
        uint32_t bvalidovval: 1;
        uint32_t hstnegscs: 1;
        uint32_t hnpreq: 1;
        uint32_t hstsethnpen: 1;
        uint32_t devhnpen: 1;
        uint32_t ehen: 1;
        uint32_t reserved2: 2;
        uint32_t dbncefltrbypass: 1;
        uint32_t conidsts: 1;
        uint32_t dbnctime: 1;
        uint32_t asesvld: 1;
        uint32_t bsesvld: 1;
        uint32_t otgver: 1;
        uint32_t curmod: 1;
        uint32_t reserved10: 10;
    };
    uint32_t val;
} usb_gotgctl_reg_t;

typedef union {
    struct {
        uint32_t reserved2: 2;
        uint32_t sesenddet: 1;
        uint32_t reserved5: 5;
        uint32_t sesreqsucstschng: 1;
        uint32_t hstnegsucstschng: 1;
        uint32_t reserved7: 7;
        uint32_t hstnegdet: 1;
        uint32_t adevtoutchg: 1;
        uint32_t dbncedone: 1;
        uint32_t reserved12: 12;
    };
    uint32_t val;
} usb_gotgint_reg_t;

typedef union {
        struct {
        uint32_t glbllntrmsk: 1;
        uint32_t hbstlen: 4;
        uint32_t dmaen: 1;
        uint32_t reserved1: 1;
        uint32_t nptxfemplvl: 1;
        uint32_t ptxfemplvl: 1;
        uint32_t reserved12: 12;
        uint32_t remmemsupp: 1;
        uint32_t notialldmawrit: 1;
        uint32_t ahbsingle: 1;
        uint32_t invdescendianess: 1;
        uint32_t reserved7: 7;
    };
    uint32_t val;
    //Checked
} usb_gahbcfg_reg_t;

typedef union {
    struct {
        uint32_t toutcal: 3;
        uint32_t phyif: 1;
        uint32_t reserved1a: 1;
        uint32_t fsintf: 1;
        uint32_t physel: 1;
        uint32_t reserved1b: 1;
        uint32_t srpcap: 1;
        uint32_t hnpcap: 1;
        uint32_t usbtrdtim: 4;
        uint32_t reserved8: 8;
        uint32_t termseldlpulse: 1;
        uint32_t reserved5: 5;
        uint32_t txenddelay: 1;
        uint32_t forcehstmode: 1;
        uint32_t forcedevmode: 1;
        uint32_t corrupttxpkt: 1;
    };
    uint32_t val;
} usb_gusbcfg_reg_t;

typedef union {
    struct {
        uint32_t csftrst: 1;
        uint32_t piufssftrst: 1;
        uint32_t frmcntrrst: 1;
        uint32_t reserved1: 1;
        uint32_t rxfflsh: 1;
        uint32_t txfflsh: 1;
        uint32_t txfnum: 5;
        uint32_t reserved19: 19;
        uint32_t dmareq: 1;
        uint32_t ahbidle: 1;
    };
    uint32_t val;
} usb_grstctl_reg_t;

typedef union {
    struct {
        uint32_t curmod_int: 1;
        uint32_t modemis: 1;
        uint32_t otgint: 1;
        uint32_t sof: 1;
        uint32_t rxflvi: 1;
        uint32_t nptxfemp: 1;
        uint32_t ginnakeff: 1;
        uint32_t goutnakeff: 1;
        uint32_t reserved2: 2;
        uint32_t erlysusp: 1;
        uint32_t usbsusp: 1;
        uint32_t usbrst: 1;
        uint32_t enumdone: 1;
        uint32_t isooutdrop: 1;
        uint32_t eopf: 1;
        uint32_t reserved1a: 1;
        uint32_t epmis: 1;
        uint32_t iepint: 1;
        uint32_t oepint: 1;
        uint32_t incompisoin: 1;
        uint32_t incompip: 1;
        uint32_t fetsusp: 1;
        uint32_t resetdet: 1;
        uint32_t prtlnt: 1;
        uint32_t hchlnt: 1;
        uint32_t ptxfemp: 1;
        uint32_t reserved1b: 1;
        uint32_t conidstschng: 1;
        uint32_t disconnint: 1;
        uint32_t sessreqint: 1;
        uint32_t wkupint: 1;
    };
    uint32_t val;
} usb_gintsts_reg_t;

typedef union {
    struct {
        uint32_t reserved1a: 1;
        uint32_t modemismsk: 1;
        uint32_t otgintmsk: 1;
        uint32_t sofmsk: 1;
        uint32_t rxflvimsk: 1;
        uint32_t nptxfempmsk: 1;
        uint32_t ginnakeffmsk: 1;
        uint32_t goutnackeffmsk: 1;
        uint32_t reserved2: 2;
        uint32_t erlysuspmsk: 1;
        uint32_t usbsuspmsk: 1;
        uint32_t usbrstmsk: 1;
        uint32_t enumdonemsk: 1;
        uint32_t isooutdropmsk: 1;
        uint32_t eopfmsk: 1;
        uint32_t reserved1b: 1;
        uint32_t epmismsk: 1;
        uint32_t iepintmsk: 1;
        uint32_t oepintmsk: 1;
        uint32_t incompisoinmsk: 1;
        uint32_t incompipmsk: 1;
        uint32_t fetsuspmsk: 1;
        uint32_t resetdetmsk: 1;
        uint32_t prtlntmsk: 1;
        uint32_t hchintmsk: 1;
        uint32_t ptxfempmsk: 1;
        uint32_t reserved1c: 1;
        uint32_t conidstschngmsk: 1;
        uint32_t disconnintmsk: 1;
        uint32_t sessreqintmsk: 1;
        uint32_t wkupintmsk: 1;
    };
    uint32_t val;
} usb_gintmsk_reg_t;

typedef union {
    struct {
        uint32_t g_chnum: 4;
        uint32_t g_bcnt: 11;
        uint32_t g_dpid: 2;
        uint32_t g_pktsts: 4;
        uint32_t g_fn: 4;
        uint32_t reserved7: 7;
    };
    uint32_t val;
} usb_grxstsr_reg_t;

typedef union {
    struct {
        uint32_t chnum: 4;
        uint32_t bcnt: 11;
        uint32_t dpid: 2;
        uint32_t pktsts: 4;
        uint32_t fn: 4;
        uint32_t reserved7: 7;
    };
    uint32_t val;
} usb_grxstsp_reg_t;

typedef union {
    struct {
        uint32_t rxfdep: 16;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} usb_grxfsiz_reg_t;

typedef union {
    struct {
        uint32_t nptxfstaddr: 16;
        uint32_t nptxfdep: 16;
    };
    uint32_t val;
} usb_gnptxfsiz_reg_t;

typedef union {
    struct {
        uint32_t nptxfspcavail: 16;
        uint32_t nptxqspcavail: 4;
        uint32_t reserved4: 4;
        uint32_t nptxqtop: 7;
        uint32_t reserved1: 1;
    };
    uint32_t val;
} usb_gnptxsts_reg_t;

typedef union {
    struct {
        uint32_t synopsysid;
    };
    uint32_t val;
} usb_gsnpsid_reg_t;

typedef union {
    struct {
        uint32_t epdir;
    };
    uint32_t val;
} usb_ghwcfg1_reg_t;

typedef union {
    struct {
        uint32_t otgmode: 3;
        uint32_t otgarch: 2;
        uint32_t singpnt: 1;
        uint32_t hsphytype: 2;
        uint32_t fsphytype: 2;
        uint32_t numdeveps: 4;
        uint32_t numhstchnl: 4;
        uint32_t periosupport: 1;
        uint32_t dynfifosizing: 1;
        uint32_t multiprocintrpt: 1;
        uint32_t reserved1a: 1;
        uint32_t nptxqdepth: 2;
        uint32_t ptxqdepth: 2;
        uint32_t tknqdepth: 5;
        uint32_t reserved1b: 1;
    };
    uint32_t val;
} usb_ghwcfg2_reg_t;

typedef union {
    struct {
        uint32_t xfersizewidth: 4;
        uint32_t pktsizewidth: 3;
        uint32_t otgen: 1;
        uint32_t i2cintsel: 1;
        uint32_t vndctlsupt: 1;
        uint32_t optfeature: 1;
        uint32_t rsttype: 1;
        uint32_t adpsupport: 1;
        uint32_t hsicmode: 1;
        uint32_t bcsupport: 1;
        uint32_t lpmmode: 1;
        uint32_t dfifodepth: 16;
    };
    uint32_t val;
} usb_ghwcfg3_reg_t;

typedef union {
    struct {
        uint32_t g_numdevperioeps: 4;
        uint32_t g_partialpwrdn: 1;
        uint32_t g_ahbfreq: 1;
        uint32_t g_hibernation: 1;
        uint32_t g_extendedhibernation: 1;
        uint32_t reserved4: 4;
        uint32_t g_acgsupt: 1;
        uint32_t g_enhancedlpmsupt: 1;
        uint32_t g_phydatawidth: 2;
        uint32_t g_numctleps: 4;
        uint32_t g_iddqfltr: 1;
        uint32_t g_vbusvalidfltr: 1;
        uint32_t g_avalidfltr: 1;
        uint32_t g_bvalidfltr: 1;
        uint32_t g_sessendfltr: 1;
        uint32_t g_dedfifomode: 1;
        uint32_t g_ineps: 4;
        uint32_t g_descdmaenabled: 1;
        uint32_t g_descdma: 1;
    };
    uint32_t val;
} usb_ghwcfg4_reg_t;

typedef union {
    struct {
        uint32_t gdfifocfg: 16;
        uint32_t epinfobaseaddr: 16;

    };
    uint32_t val;
} usb_gdfifocfg_reg_t;

typedef union {
    struct {
        uint32_t ptxfstaddr: 16;
        uint32_t ptxfsize: 16;
    };
    uint32_t val;
} usb_hptxfsiz_reg_t;

typedef union {
    struct {
        uint32_t inepitxfstaddr: 16;
        uint32_t inep1txfdep: 16;
    };
    uint32_t val;
} usb_dieptxfi_reg_t;

typedef union {
    struct {
        uint32_t fslspclksel: 2;
        uint32_t fslssupp: 1;
        uint32_t reserved4a: 4;
        uint32_t ena32khzs: 1;
        uint32_t resvalid: 8;
        uint32_t reserved1: 1;
        uint32_t reserved6: 6;
        uint32_t descdma: 1;
        uint32_t frlisten: 2;
        uint32_t perschedena: 1;
        uint32_t reserved4b: 4;
        uint32_t modechtimen: 1;
    };
    uint32_t val;
} usb_hcfg_reg_t;

typedef union {
    struct {
        uint32_t frint: 16;
        uint32_t hfirrldctrl: 1;
        uint32_t reserved15: 15;
    };
    uint32_t val;
} usb_hfir_reg_t;

typedef union {
    struct {
        uint32_t frnum: 14;
        uint32_t reserved: 2;
        uint32_t frrem: 16;
    };
    uint32_t val;
} usb_hfnum_reg_t;

typedef union {
    struct {
        uint32_t ptxfspcavail: 16;
        uint32_t ptxqspcavail: 5;
        uint32_t reserved: 3;
        uint32_t ptxqtop: 8;
    };
    uint32_t val;
} usb_hptxsts_reg_t;

typedef union {
    struct {
        uint32_t haint: 8;
        uint32_t reserved24: 24;
    };
    uint32_t val;
} usb_haint_reg_t;

typedef union {
    struct {
        uint32_t haintmsk: 8;
        uint32_t reserved24: 24;
    };
    uint32_t val;
} usb_haintmsk_reg_t;

typedef union {
    struct {
        uint32_t hflbaddr;
    };
    uint32_t val;
} usb_hflbaddr_reg_t;

typedef union {
    struct {
        uint32_t prtconnsts: 1;
        uint32_t prtconndet: 1;
        uint32_t prtena: 1;
        uint32_t prtenchng: 1;
        uint32_t prtovrcurract: 1;
        uint32_t prtovrcurrchng: 1;
        uint32_t prtres: 1;
        uint32_t prtsusp: 1;
        uint32_t prtrst: 1;
        uint32_t reserved1: 1;
        uint32_t prtlnsts: 2;
        uint32_t prtpwr: 1;
        uint32_t prttstctl: 4;
        uint32_t prtspd: 2;
        uint32_t reserved13: 13;
    };
    uint32_t val;
} usb_hprt_reg_t;

typedef union {
    struct {
        uint32_t mps: 11;
        uint32_t epnum: 4;
        uint32_t epdir: 1;
        uint32_t reserved: 1;
        uint32_t lspddev: 1;
        uint32_t eptype: 2;
        uint32_t ec: 2;
        uint32_t devaddr: 7;
        uint32_t oddfrm: 1;
        uint32_t chdis: 1;
        uint32_t chena: 1;
    };
    uint32_t val;
    //Checked with changes
} usb_hcchar_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t chhltd: 1;
        uint32_t ahberr: 1;
        uint32_t stall: 1;
        uint32_t nack: 1;
        uint32_t ack: 1;
        uint32_t nyet: 1;
        uint32_t xacterr: 1;
        uint32_t bblerr: 1;
        uint32_t frmovrun: 1;
        uint32_t datatglerr: 1;
        uint32_t bnaintr: 1;
        uint32_t xcs_xact_err: 1;
        uint32_t desc_lst_rollintr: 1;
        uint32_t reserved18: 18;
    };
    uint32_t val;
    //Checked
} usb_hcint_reg_t;

typedef union {
    struct {
        uint32_t xfercomplmsk: 1;
        uint32_t chhltdmsk: 1;
        uint32_t ahberrmsk: 1;
        uint32_t stallmsk: 1;
        uint32_t nakmsk: 1;
        uint32_t ackmsk: 1;
        uint32_t nyetmsk: 1;
        uint32_t xacterrmsk: 1;
        uint32_t bblerrmsk: 1;
        uint32_t frmovrunmsk: 1;
        uint32_t datatglerrmsk: 1;
        uint32_t bnaintrmsk: 1;
        uint32_t reserved1: 1;
        uint32_t desc_lst_rollintrmsk: 1;
        uint32_t reserved18: 18;
    };
    uint32_t val;
    //Checked
} usb_hcintmsk_reg_t;

typedef union {
    struct {
        uint32_t sched_info: 8;
        uint32_t ntd: 8;
        uint32_t reserved3: 3;
        uint32_t reserved10: 10;
        uint32_t pid: 2;
        uint32_t dopng: 1;
    };
    uint32_t val;
    //Checked
} usb_hctsiz_reg_t;

typedef union {
    struct {
        uint32_t reserved3: 3;
        uint32_t ctd: 6;
        uint32_t dmaaddr: 23;
    } non_iso;
    struct {
        uint32_t reserved3: 3;
        uint32_t dmaaddr_ctd: 29;
    } iso;
    uint32_t val;
    //Checked
} usb_hcdma_reg_t;

typedef union {
    struct {
        uint32_t hcdmab;
    };
    uint32_t val;
} usb_hcdmab_reg_t;

typedef union {
    struct {
        uint32_t reserved2a: 2;
        uint32_t nzstsouthshk: 1;
        uint32_t reserved1: 1;
        uint32_t devaddr: 7;
        uint32_t perfrlint: 2;
        uint32_t endevoutnak: 1;
        uint32_t xcvrdly: 1;
        uint32_t erraticintmsk: 1;
        uint32_t reserved2b: 2;
        uint32_t epmiscnt: 5;
        uint32_t descdma: 1;
        uint32_t perschintvl: 2;
        uint32_t resvalid: 6;
    };
    uint32_t val;
} usb_dcfg_reg_t;

typedef union {
    struct {
        uint32_t rmtwkupsig: 1;
        uint32_t sftdiscon: 1;
        uint32_t gnpinnaksts: 1;
        uint32_t goutnaksts: 1;
        uint32_t tstctl: 3;
        uint32_t sgnpinnak: 1;
        uint32_t cgnpinnak: 1;
        uint32_t sgoutnak: 1;
        uint32_t cgoutnak: 1;
        uint32_t pwronprgdone: 1;
        uint32_t reserved1: 1;
        uint32_t gmc: 2;
        uint32_t ignrfrmnum: 1;
        uint32_t nakonbble: 1;
        uint32_t encountonbna: 1;
        uint32_t deepsleepbeslreject: 1;
        uint32_t reserved3: 13;
    };
    uint32_t val;
} usb_dctl_reg_t;

typedef union {
    struct {
        uint32_t suspsts: 1;
        uint32_t enumspd: 2;
        uint32_t errticerr: 1;
        uint32_t reserved4: 4;
        uint32_t soffn: 14;
        uint32_t devlnsts: 2;
        uint32_t reserved8: 8;
    };
    uint32_t val;
} usb_dsts_reg_t;

typedef union {
    struct {
        uint32_t di_xfercomplmsk: 1;
        uint32_t di_epdisbldmsk: 1;
        uint32_t di_ahbermsk: 1;
        uint32_t timeoutmsk: 1;
        uint32_t intkntxfempmsk: 1;
        uint32_t intknepmismsk: 1;
        uint32_t inepnakeffmsk: 1;
        uint32_t reserved1: 1;
        uint32_t txfifoundrnmsk: 1;
        uint32_t bnainintrmsk: 1;
        uint32_t reserved3: 3;
        uint32_t di_nakmsk: 1;
        uint32_t reserved18: 18;
    };
    uint32_t val;
} usb_diepmsk_reg_t;

typedef union {
    struct {
        uint32_t xfercomplmsk: 1;
        uint32_t epdisbldmsk: 1;
        uint32_t ahbermsk: 1;
        uint32_t setupmsk: 1;
        uint32_t outtknepdismsk: 1;
        uint32_t stsphsercvdmsk: 1;
        uint32_t back2backsetup: 1;
        uint32_t reserved1: 1;
        uint32_t outpkterrmsk: 1;
        uint32_t bnaoutintrmsk: 1;
        uint32_t reserved2: 2;
        uint32_t bbleerrmsk: 1;
        uint32_t nakmsk: 1;
        uint32_t nyetmsk: 1;
        uint32_t reserved17: 17;
    };
    uint32_t val;
} usb_doepmsk_reg_t;

typedef union {
    struct {
        uint32_t inepint0: 1;
        uint32_t inepint1: 1;
        uint32_t inepint2: 1;
        uint32_t inepint3: 1;
        uint32_t inepint4: 1;
        uint32_t inepint5: 1;
        uint32_t inepint6: 1;
        uint32_t reserved9a: 9;
        uint32_t outepint0: 1;
        uint32_t outepint1: 1;
        uint32_t outepint2: 1;
        uint32_t outepint3: 1;
        uint32_t outepint4: 1;
        uint32_t outepint5: 1;
        uint32_t outepint6: 1;
        uint32_t reserved9b: 9;
    };
    uint32_t val;
} usb_daint_reg_t;

typedef union {
    struct {
        uint32_t inepmsk0: 1;
        uint32_t inepmsk1: 1;
        uint32_t inepmsk2: 1;
        uint32_t inepmsk3: 1;
        uint32_t inepmsk4: 1;
        uint32_t inepmsk5: 1;
        uint32_t inepmsk6: 1;
        uint32_t reserved9a: 9;
        uint32_t outepmsk0: 1;
        uint32_t outepmsk1: 1;
        uint32_t outepmsk2: 1;
        uint32_t outepmsk3: 1;
        uint32_t outepmsk4: 1;
        uint32_t outepmsk5: 1;
        uint32_t outepmsk6: 1;
        uint32_t reserved9b: 9;
    };
    uint32_t val;
} usb_daintmsk_reg_t;

typedef union {
    struct {
        uint32_t dvbusdis: 16;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} usb_dvbusdis_reg_t;

typedef union {
    struct {
        uint32_t dvbuspulse: 12;
        uint32_t reserved20: 20;
    };
    uint32_t val;
} usb_dvbuspulse_reg_t;

typedef union {
    struct {
        uint32_t nonisothren: 1;
        uint32_t isothren: 1;
        uint32_t txthrlen: 9;
        uint32_t ahbthrratio: 2;
        uint32_t reserved3: 3;
        uint32_t rxthren: 1;
        uint32_t rxthrlen: 9;
        uint32_t reserved1: 1;
        uint32_t arbprken: 1;
        uint32_t reserved4: 4;
    };
    uint32_t val;
} usb_dthrctl_reg_t;

typedef union {
    struct {
        uint32_t ineptxfernpmsk: 16;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} usb_diepempmsk_reg_t;

typedef union {
    struct {
        uint32_t mps0: 2;
        uint32_t reserved9: 9;
        uint32_t reserved4: 4;
        uint32_t usbactep0: 1;
        uint32_t reserved1a: 1;
        uint32_t naksts0: 1;
        uint32_t eptype0: 2;
        uint32_t reserved1b: 1;
        uint32_t stall0: 1;
        uint32_t txfnum0: 4;
        uint32_t cnak0: 1;
        uint32_t snak0: 1;
        uint32_t reserved2: 2;
        uint32_t epdis0: 1;
        uint32_t epena0: 1;
    };
    uint32_t val;
} usb_diepctl0_reg_t;

typedef union {
    struct {
        uint32_t xfercompl0: 1;
        uint32_t epdisbld0: 1;
        uint32_t ahberr0: 1;
        uint32_t timeout0: 1;
        uint32_t intkntxfemp0: 1;
        uint32_t intknepmis0: 1;
        uint32_t inepnakeff0: 1;
        uint32_t txfemp0: 1;
        uint32_t txfifoundrn0: 1;
        uint32_t bnaintr0: 1;
        uint32_t reserved1: 1;
        uint32_t pktdrpsts0: 1;
        uint32_t bbleerr0: 1;
        uint32_t nakintrpt0: 1;
        uint32_t nyetintrpt0: 1;
        uint32_t reserved17: 17;
    };
    uint32_t val;
} usb_diepint0_reg_t;

typedef union {
    struct {
        uint32_t xfersize0: 7;
        uint32_t reserved12: 12;
        uint32_t pktcnt0: 2;
        uint32_t reserved11: 11;
    };
    uint32_t val;
} usb_dieptsiz0_reg_t;

typedef union {
    struct {
        uint32_t dmaaddr0;
    };
    uint32_t val;
} usb_diepdma0_reg_t;

typedef union {
    struct {
        uint32_t ineptxfspcavail0: 16;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} usb_dtxfsts0_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr0;
    };
    uint32_t val;
} usb_diepdmab0_reg_t;

typedef union {
    struct {
        uint32_t mps: 2;
        uint32_t reserved9: 9;
        uint32_t reserved4: 4;
        uint32_t usbactep: 1;
        uint32_t reserved1a: 1;
        uint32_t naksts: 1;
        uint32_t eptype: 2;
        uint32_t reserved1b: 1;
        uint32_t stall: 1;
        uint32_t txfnum: 4;
        uint32_t cnak: 1;
        uint32_t snak: 1;
        uint32_t setd0pid: 1;
        uint32_t setd1pid: 1;
        uint32_t epdis: 1;
        uint32_t epena: 1;
    };
    uint32_t val;
} usb_diepctl_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t epdisbld: 1;
        uint32_t ahberr: 1;
        uint32_t timeout: 1;
        uint32_t intkntxfemp: 1;
        uint32_t intknepmis: 1;
        uint32_t inepnakeff: 1;
        uint32_t txfemp: 1;
        uint32_t txfifoundrn: 1;
        uint32_t bnaintr: 1;
        uint32_t reserved1: 1;
        uint32_t pktdrpsts: 1;
        uint32_t bbleerr: 1;
        uint32_t nakintrpt: 1;
        uint32_t nyetintrpt: 1;
        uint32_t reserved15: 17;
    };
    uint32_t val;
} usb_diepint_reg_t;

typedef union {
    struct {
        uint32_t xfersize: 7;
        uint32_t reserved12: 12;
        uint32_t pktcnt: 2;
        uint32_t reserved11: 11;
    };
    uint32_t val;
} usb_dieptsiz_reg_t;

typedef union {
    struct {
        uint32_t dmaddr1;
    };
    uint32_t val;
} usb_diepdma_reg_t;

typedef union {
    struct {
        uint32_t ineptxfspcavail: 16;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} usb_dtxfsts_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr1;
    };
    uint32_t val;
} usb_diepdmab_reg_t;

typedef union {
    struct {
        uint32_t mps0: 2;
        uint32_t reserved13: 13;
        uint32_t usbactep0: 1;
        uint32_t reserved1: 1;
        uint32_t naksts0: 1;
        uint32_t eptype0: 2;
        uint32_t snp0: 1;
        uint32_t stall0: 1;
        uint32_t reserved4: 4;
        uint32_t cnak0: 1;
        uint32_t snak0: 1;
        uint32_t reserved2: 2;
        uint32_t epdis0: 1;
        uint32_t epena0: 1;
    };
    uint32_t val;
} usb_doepctl0_reg_t;

typedef union {
    struct {
        uint32_t xfercompl0: 1;
        uint32_t epdisbld0: 1;
        uint32_t ahberr0: 1;
        uint32_t setup0: 1;
        uint32_t outtknepdis0: 1;
        uint32_t stsphsercvd0: 1;
        uint32_t back2backsetup0: 1;
        uint32_t reserved1a: 1;
        uint32_t outpkterr0: 1;
        uint32_t bnaintr0: 1;
        uint32_t reserved1b: 1;
        uint32_t pktdrpsts0: 1;
        uint32_t bbleerr0: 1;
        uint32_t nakintrpt0: 1;
        uint32_t nyepintrpt0: 1;
        uint32_t stuppktrcvd0: 1;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} usb_doepint0_reg_t;

typedef union {
    struct {
        uint32_t xfersize0: 7;
        uint32_t reserved12: 12;
        uint32_t pktcnt0: 1;
        uint32_t reserved9: 9;
        uint32_t supcnt0: 2;
        uint32_t reserved1: 1;
    };
    uint32_t val;
} usb_doeptsiz0_reg_t;

typedef union {
    struct {
        uint32_t dmaaddr0;
    };
    uint32_t val;
} usb_doepdma0_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr0;
    };
    uint32_t val;
} usb_doepdmab0_reg_t;

typedef union {
    struct {
        uint32_t mps: 11;
        uint32_t reserved4a: 4;
        uint32_t usbactep: 1;
        uint32_t reserved1: 1;
        uint32_t naksts: 1;
        uint32_t eptype: 2;
        uint32_t snp: 1;
        uint32_t stall: 1;
        uint32_t reserved4b: 4;
        uint32_t cnak: 1;
        uint32_t snak: 1;
        uint32_t setd0pid: 1;
        uint32_t setd1pid: 1;
        uint32_t epdis: 1;
        uint32_t epena: 1;
    };
    uint32_t val;
} usb_doepctl_reg_t;

typedef union {
    struct {
        uint32_t xfercompl: 1;
        uint32_t epdisbld: 1;
        uint32_t ahberr: 1;
        uint32_t setup: 1;
        uint32_t outtknepdis: 1;
        uint32_t stsphsercvd: 1;
        uint32_t back2backsetup: 1;
        uint32_t reserved1a: 1;
        uint32_t outpkterr: 1;
        uint32_t bnaintr: 1;
        uint32_t reserved1b: 1;
        uint32_t pktdrpsts: 1;
        uint32_t bbleerr: 1;
        uint32_t nakintrpt: 1;
        uint32_t nyepintrpt: 1;
        uint32_t stuppktrcvd: 1;
        uint32_t reserved16: 16;
    };
    uint32_t val;
} usb_doepint_reg_t;

typedef union {
    struct {
        uint32_t xfersize: 7;
        uint32_t reserved12: 12;
        uint32_t pktcnt: 1;
        uint32_t reserved9: 9;
        uint32_t supcnt: 2;
        uint32_t reserved1: 1;
    };
    uint32_t val;
} usb_doeptsiz_reg_t;

typedef union {
    struct {
        uint32_t dmaaddr;
    };
    uint32_t val;
} usb_doepdma_reg_t;

typedef union {
    struct {
        uint32_t dmabufferaddr;
    };
    uint32_t val;
} usb_doepdmab_reg_t;

typedef union {
    struct {
        uint32_t stoppclk: 1;
        uint32_t gatehclk: 1;
        uint32_t pwrclmp: 1;
        uint32_t rstpdwnmodule: 1;
        uint32_t reserved2: 2;
        uint32_t physleep: 1;
        uint32_t l1suspended: 1;
        uint32_t resetaftersusp: 1;
        uint32_t reserved23: 23;
    };
    uint32_t val;
} usb_pcgcctl_reg_t;

/* --------------------------- Register Groups ------------------------------ */

typedef struct {
    volatile usb_hcchar_reg_t hcchar_reg;    //0x00
    uint32_t reserved_0x04_0x08[1]; //0x04
    volatile usb_hcint_reg_t hcint_reg;      //0x08
    volatile usb_hcintmsk_reg_t hcintmsk_reg;    //0x0c
    volatile usb_hctsiz_reg_t hctsiz_reg;    //0x10
    volatile usb_hcdma_reg_t hcdma_reg;      //0x14
    uint32_t reserved_0x14_0x14[1]; //0x18*
    volatile usb_hcdmab_reg_t hcdmab_reg;    //0x1c
} usb_host_chan_regs_t;

typedef struct {
    volatile usb_diepctl_reg_t diepctl_reg;  //0x00
    uint32_t reserved_0x04_0x08[1]; //0x04
    volatile usb_diepint_reg_t diepint_reg;  //0x08
    uint32_t reserved_0x0c_0x10[1]; //0x0c
    volatile usb_dieptsiz_reg_t dieptsiz_reg;    //0x010
    volatile usb_diepdma_reg_t diepdma_reg;  //0x14
    volatile usb_dtxfsts_reg_t dtxfsts_reg;  //0x18
    volatile usb_diepdmab_reg_t diepdmab_reg;    //0x1c
} usb_in_ep_regs_t;

typedef struct {
    volatile usb_doepctl_reg_t doepctl_reg;  //0x00
    uint32_t reserved_0x04_0x08[1]; //0x04
    volatile usb_doepint_reg_t doepint_reg;  //0x08
    uint32_t reserved_0x0c_0x10[1]; //0x0c
    volatile usb_doeptsiz_reg_t doeptsiz_reg;    //0x10
    volatile usb_doepdma_reg_t doepdma_reg;  //0x14
    uint32_t reserved_0x18_0x1c[1]; //0x18
    volatile usb_doepdmab_reg_t doepdmab_reg;    //0x1c
} usb_out_ep_regs_t;

/* --------------------------- Register Layout ------------------------------ */

typedef struct {
    //Global Registers
    volatile usb_gotgctl_reg_t gotgctl_reg;             //0x0000
    volatile usb_gotgint_reg_t gotgint_reg;             //0x0004
    volatile usb_gahbcfg_reg_t gahbcfg_reg;             //0x0008
    volatile usb_gusbcfg_reg_t gusbcfg_reg;             //0x000c
    volatile usb_grstctl_reg_t grstctl_reg;             //0x0010
    volatile usb_gintsts_reg_t gintsts_reg;             //0x0014
    volatile usb_gintmsk_reg_t gintmsk_reg;             //0x0018
    volatile usb_grxstsr_reg_t grxstsr_reg;             //0x001c
    volatile usb_grxstsp_reg_t grxstsp_reg;             //0x0020
    volatile usb_grxfsiz_reg_t grxfsiz_reg;             //0x0024
    volatile usb_gnptxfsiz_reg_t gnptxfsiz_reg;         //0x0028
    volatile usb_gnptxsts_reg_t gnptxsts_reg;           //0x002c
    uint32_t reserved_0x0030_0x0040[4];                 //0x0030 to 0x0040
    volatile usb_gsnpsid_reg_t gsnpsid_reg;             //0x0040
    volatile usb_ghwcfg1_reg_t ghwcfg1_reg;             //0x0044
    volatile usb_ghwcfg2_reg_t ghwcfg2_reg;             //0x0048
    volatile usb_ghwcfg3_reg_t ghwcfg3_reg;             //0x004c
    volatile usb_ghwcfg4_reg_t ghwcfg4_reg;             //0x0050
    uint32_t reserved_0x0054_0x005c[2];                 //0x0054 to 0x005c

    //FIFO Configurations
    volatile usb_gdfifocfg_reg_t gdfifocfg_reg;         //0x005c
    uint32_t reserved_0x0060_0x0100[40];                //0x0060 to 0x0100
    volatile usb_hptxfsiz_reg_t hptxfsiz_reg;           //0x0100
    volatile usb_dieptxfi_reg_t dieptxfi_regs[4];       //0x0104 to 0x0114
    usb_dieptxfi_reg_t reserved_0x0114_0x0140[11];      //0x0114 to 0x0140
    uint32_t reserved_0x140_0x400[176];                 //0x0140 to 0x0400

    //Host Mode Registers
    volatile usb_hcfg_reg_t hcfg_reg;                   //0x0400
    volatile usb_hfir_reg_t hfir_reg;                   //0x0404
    volatile usb_hfnum_reg_t hfnum_reg;                 //0x0408
    uint32_t reserved_0x40c_0x410[1];                   //0x040c to 0x0410
    volatile usb_hptxsts_reg_t hptxsts_reg;             //0x0410
    volatile usb_haint_reg_t haint_reg;                 //0x0414
    volatile usb_haintmsk_reg_t haintmsk_reg;           //0x0418
    volatile usb_hflbaddr_reg_t hflbaddr_reg;           //0x041c
    uint32_t reserved_0x420_0x440[8];                   //0x0420 to 0x0440
    volatile usb_hprt_reg_t hprt_reg;                   //0x0440
    uint32_t reserved_0x0444_0x0500[47];                //0x0444 to 0x0500
    usb_host_chan_regs_t host_chans[8];                 //0x0500 to 0x0600
    usb_host_chan_regs_t reserved_0x0600_0x0700[8];     //0x0600 to 0x0700
    uint32_t reserved_0x0700_0x0800[64];                //0x0700 to 0x0800
    volatile usb_dcfg_reg_t dcfg_reg;                   //0x0800
    volatile usb_dctl_reg_t dctl_reg;                   //0x0804
    volatile usb_dsts_reg_t dsts_reg;                   //0x0808
    uint32_t reserved_0x080c_0x0810[1];                 //0x080c to 0x0810

    //Device Mode Registers
    volatile usb_diepmsk_reg_t diepmsk_reg;             //0x810
    volatile usb_doepmsk_reg_t doepmsk_reg;             //0x0814
    volatile usb_daint_reg_t daint_reg;                 //0x0818
    volatile usb_daintmsk_reg_t daintmsk_reg;           //0x081c
    uint32_t reserved_0x0820_0x0828[2];                 //0x0820 to 0x0828
    volatile usb_dvbusdis_reg_t dvbusdis_reg;           //0x0828
    volatile usb_dvbuspulse_reg_t dvbuspulse_reg;       //0x082c
    volatile usb_dthrctl_reg_t dthrctl_reg;             //0x0830
    volatile usb_diepempmsk_reg_t diepempmsk_reg;       //0x0834
    uint32_t reserved_0x0838_0x0900[50];                //0x0838 to 0x0900

    //Deivce: IN EP0 reigsters
    volatile usb_diepctl0_reg_t diepctl0_reg;           //0x0900
    uint32_t reserved_0x0904_0x0908[1];                 //0x0904 to 0x0908
    volatile usb_diepint0_reg_t diepint0_reg;           //0x0908
    uint32_t reserved_0x090c_0x0910[1];                 //0x090c to 0x0910
    volatile usb_dieptsiz0_reg_t dieptsiz0_reg;         //0x0910
    volatile usb_diepdma0_reg_t diepdma0_reg;           //0x0914
    volatile usb_dtxfsts0_reg_t dtxfsts0_reg;           //0x0918
    volatile usb_diepdmab0_reg_t diepdmab0_reg;         //0x091c

    //Deivce: IN EP registers
    usb_in_ep_regs_t in_eps[6];                         //0x0920 to 0x09e0
    usb_in_ep_regs_t reserved_0x09e0_0x0b00[9];         //0x09e0 to 0x0b00

    //Device: OUT EP0 reigsters
    volatile usb_doepctl0_reg_t doepctl0_reg;           //0x0b00
    uint32_t reserved_0x0b04_0x0b08[1];                 //0x0b04 to 0x0b08
    volatile usb_doepint0_reg_t doepint0_reg;           //0b0b08
    uint32_t reserved_0x0b0c_0x0b10[1];                 //0x0b0c to 0x0b10
    volatile usb_doeptsiz0_reg_t doeptsiz0_reg;         //0x0b10
    volatile usb_doepdma0_reg_t doepdma0_reg;           //0x0b14
    uint32_t reserved_0x0b18_0x0b1c[1];                 //0x0b18 to 0x0b1c
    volatile usb_doepdmab0_reg_t doepdmab0_reg;         //0x0b1c

    //Deivce: OUT EP registers
    usb_out_ep_regs_t out_eps[6];                       //0xb1c
    usb_out_ep_regs_t reserved_0x0be0_0x0d00[9];        //0x0be0 to 0x0d00
    uint32_t reserved_0x0d00_0x0e00[64];                //0x0d00 to 0x0e00
    volatile usb_pcgcctl_reg_t pcgcctl_reg;             //0x0e00
    uint32_t reserved_0x0e04_0x0e08[1];                 //0x0d00 to 0x0e00
} usbh_dev_t;


_Static_assert(sizeof(usbh_dev_t) == 0xe08, "USB new struct should be 0xe08 large");

extern usbh_dev_t USBH;


#ifdef __cplusplus
}
#endif
