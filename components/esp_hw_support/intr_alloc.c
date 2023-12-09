/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <esp_types.h>
#include <limits.h>
#include <assert.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_memory_utils.h"
#include "esp_intr_alloc.h"
#include "esp_attr.h"
#include "esp_cpu.h"
#include "esp_private/rtc_ctrl.h"
#include "soc/interrupts.h"
#include "soc/soc_caps.h"
#include "sdkconfig.h"

#if !CONFIG_FREERTOS_UNICORE
#include "esp_ipc.h"
#endif

/* For targets that uses a CLIC as their interrupt controller, CPU_INT_LINES_COUNT represents the external interrupts count */
#define CPU_INT_LINES_COUNT 32

static const char* TAG = "intr_alloc";

#define ETS_INTERNAL_TIMER0_INTR_NO 6
#define ETS_INTERNAL_TIMER1_INTR_NO 15
#define ETS_INTERNAL_TIMER2_INTR_NO 16
#define ETS_INTERNAL_SW0_INTR_NO 7
#define ETS_INTERNAL_SW1_INTR_NO 29
#define ETS_INTERNAL_PROFILING_INTR_NO 11

/*
Define this to debug the choices made when allocating the interrupt. This leads to much debugging
output within a critical region, which can lead to weird effects like e.g. the interrupt watchdog
being triggered, that is why it is separate from the normal LOG* scheme.
*/
// #define DEBUG_INT_ALLOC_DECISIONS

#ifdef DEBUG_INT_ALLOC_DECISIONS
# define ALCHLOG(...) ESP_EARLY_LOGD(TAG, __VA_ARGS__)
#else
# define ALCHLOG(...) do {} while (0)
#endif

typedef struct shared_vector_desc_t shared_vector_desc_t;
typedef struct vector_desc_t vector_desc_t;

struct shared_vector_desc_t {
    int disabled: 1;
    int source: 8;
    volatile uint32_t *statusreg;
    uint32_t statusmask;
    intr_handler_t isr;
    void *arg;
    shared_vector_desc_t *next;
};

#define VECDESC_FL_RESERVED     (1<<0)
#define VECDESC_FL_INIRAM       (1<<1)
#define VECDESC_FL_SHARED       (1<<2)
#define VECDESC_FL_NONSHARED    (1<<3)

//Pack using bitfields for better memory use
struct vector_desc_t {
    int flags: 16;                          //OR of VECDESC_FL_* defines
    unsigned int cpu: 1;
    unsigned int intno: 5;
    int source: 8;                          //Interrupt mux flags, used when not shared
    shared_vector_desc_t *shared_vec_info;  //used when VECDESC_FL_SHARED
    vector_desc_t *next;
};

/** Interrupt handler associated data structure */
typedef struct intr_handle_data_t {
    vector_desc_t *vector_desc;
    shared_vector_desc_t *shared_vector_desc;
} intr_handle_data_t;

typedef struct non_shared_isr_arg_t non_shared_isr_arg_t;

struct non_shared_isr_arg_t {
    intr_handler_t isr;
    void *isr_arg;
    int source;
};

//Linked list of vector descriptions, sorted by cpu.intno value
static vector_desc_t *vector_desc_head = NULL;

//This bitmask has an 1 if the int should be disabled when the flash is disabled.
static uint32_t non_iram_int_mask[SOC_CPU_CORES_NUM];

//This bitmask has 1 in it if the int was disabled using esp_intr_noniram_disable.
static uint32_t non_iram_int_disabled[SOC_CPU_CORES_NUM];
static bool non_iram_int_disabled_flag[SOC_CPU_CORES_NUM];

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

//Inserts an item into vector_desc list so that the list is sorted
//with an incrementing cpu.intno value.
static void insert_vector_desc(vector_desc_t *to_insert)
{
    vector_desc_t *vd = vector_desc_head;
    vector_desc_t *prev = NULL;
    while(vd != NULL) {
        if (vd->cpu > to_insert->cpu) break;
        if (vd->cpu == to_insert->cpu && vd->intno >= to_insert->intno) break;
        prev = vd;
        vd = vd->next;
    }
    if ((vector_desc_head == NULL) || (prev == NULL)) {
        //First item
        to_insert->next = vd;
        vector_desc_head = to_insert;
    } else {
        prev->next = to_insert;
        to_insert->next = vd;
    }
}

//Returns a vector_desc entry for an intno/cpu, or NULL if none exists.
static vector_desc_t *find_desc_for_int(int intno, int cpu)
{
    vector_desc_t *vd = vector_desc_head;
    while(vd != NULL) {
        if (vd->cpu == cpu && vd->intno == intno) {
            break;
        }
        vd = vd->next;
    }
    return vd;
}

//Returns a vector_desc entry for an intno/cpu.
//Either returns a preexisting one or allocates a new one and inserts
//it into the list. Returns NULL on malloc fail.
static vector_desc_t *get_desc_for_int(int intno, int cpu)
{
    vector_desc_t *vd = find_desc_for_int(intno, cpu);
    if (vd == NULL) {
        vector_desc_t *newvd = heap_caps_malloc(sizeof(vector_desc_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (newvd == NULL) {
            return NULL;
        }
        memset(newvd, 0, sizeof(vector_desc_t));
        newvd->intno = intno;
        newvd->cpu = cpu;
        insert_vector_desc(newvd);
        return newvd;
    } else {
        return vd;
    }
}

//Returns a vector_desc entry for a source, the cpu parameter is used to tell GPIO_INT and GPIO_NMI from different CPUs
static vector_desc_t * find_desc_for_source(int source, int cpu)
{
    vector_desc_t *vd = vector_desc_head;
    while(vd != NULL) {
        if (!(vd->flags & VECDESC_FL_SHARED)) {
            if (vd->source == source && cpu == vd->cpu) {
                break;
            }
        } else if (vd->cpu == cpu) {
            // check only shared vds for the correct cpu, otherwise skip
            bool found = false;
            shared_vector_desc_t *svd = vd->shared_vec_info;
            assert(svd != NULL);
            while(svd) {
                if (svd->source == source) {
                    found = true;
                    break;
                }
                svd = svd->next;
            }
            if (found) {
                break;
            }
        }
        vd = vd->next;
    }
    return vd;
}

esp_err_t esp_intr_mark_shared(int intno, int cpu, bool is_int_ram)
{
    if (intno>31) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cpu >= SOC_CPU_CORES_NUM) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&spinlock);
    vector_desc_t *vd = get_desc_for_int(intno, cpu);
    if (vd == NULL) {
        portEXIT_CRITICAL(&spinlock);
        return ESP_ERR_NO_MEM;
    }
    vd->flags = VECDESC_FL_SHARED;
    if (is_int_ram) {
        vd->flags |= VECDESC_FL_INIRAM;
    }
    portEXIT_CRITICAL(&spinlock);

    return ESP_OK;
}

esp_err_t esp_intr_reserve(int intno, int cpu)
{
    if (intno > 31) {
        return ESP_ERR_INVALID_ARG;
    }
    if (cpu >= SOC_CPU_CORES_NUM) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL(&spinlock);
    vector_desc_t *vd = get_desc_for_int(intno, cpu);
    if (vd == NULL) {
        portEXIT_CRITICAL(&spinlock);
        return ESP_ERR_NO_MEM;
    }
    vd->flags = VECDESC_FL_RESERVED;
    portEXIT_CRITICAL(&spinlock);

    return ESP_OK;
}

static bool is_vect_desc_usable(vector_desc_t *vd, int flags, int cpu, int force)
{
    //Check if interrupt is not reserved by design
    int x = vd->intno;
    esp_cpu_intr_desc_t intr_desc;
    esp_cpu_intr_get_desc(cpu, x, &intr_desc);

    if (intr_desc.flags & ESP_CPU_INTR_DESC_FLAG_RESVD) {
        ALCHLOG("....Unusable: reserved");
        return false;
    }
    if (intr_desc.flags & ESP_CPU_INTR_DESC_FLAG_SPECIAL && force == -1) {
        ALCHLOG("....Unusable: special-purpose int");
        return false;
    }

#ifndef SOC_CPU_HAS_FLEXIBLE_INTC
    //Check if the interrupt priority is acceptable
    if (!(flags & (1 << intr_desc.priority))) {
        ALCHLOG("....Unusable: incompatible priority");
        return false;
    }
    //check if edge/level type matches what we want
    if (((flags & ESP_INTR_FLAG_EDGE) && (intr_desc.type == ESP_CPU_INTR_TYPE_LEVEL)) ||
        (((!(flags & ESP_INTR_FLAG_EDGE)) && (intr_desc.type == ESP_CPU_INTR_TYPE_EDGE)))) {
        ALCHLOG("....Unusable: incompatible trigger type");
        return false;
    }
#endif

    //check if interrupt is reserved at runtime
    if (vd->flags & VECDESC_FL_RESERVED)  {
        ALCHLOG("....Unusable: reserved at runtime.");
        return false;
    }

    //Ints can't be both shared and non-shared.
    assert(!((vd->flags & VECDESC_FL_SHARED) && (vd->flags & VECDESC_FL_NONSHARED)));
    //check if interrupt already is in use by a non-shared interrupt
    if (vd->flags & VECDESC_FL_NONSHARED) {
        ALCHLOG("....Unusable: already in (non-shared) use.");
        return false;
    }
    // check shared interrupt flags
    if (vd->flags & VECDESC_FL_SHARED) {
        if (flags & ESP_INTR_FLAG_SHARED) {
            bool in_iram_flag = ((flags & ESP_INTR_FLAG_IRAM) != 0);
            bool desc_in_iram_flag = ((vd->flags & VECDESC_FL_INIRAM) != 0);
            //Bail out if int is shared, but iram property doesn't match what we want.
            if ((vd->flags & VECDESC_FL_SHARED) && (desc_in_iram_flag != in_iram_flag))  {
                ALCHLOG("....Unusable: shared but iram prop doesn't match");
                return false;
            }
        } else {
            //We need an unshared IRQ; can't use shared ones; bail out if this is shared.
            ALCHLOG("...Unusable: int is shared, we need non-shared.");
            return false;
        }
    } else if (esp_cpu_intr_has_handler(x)) {
        //Check if interrupt already is allocated by esp_cpu_intr_set_handler
        ALCHLOG("....Unusable: already allocated");
        return false;
    }

    return true;
}

//Locate a free interrupt compatible with the flags given.
//The 'force' argument can be -1, or 0-31 to force checking a certain interrupt.
//When a CPU is forced, the ESP_CPU_INTR_DESC_FLAG_SPECIAL marked interrupts are also accepted.
static int get_available_int(int flags, int cpu, int force, int source)
{
    int x;
    int best=-1;
    int bestPriority=9;
    int bestSharedCt=INT_MAX;

    //Default vector desc, for vectors not in the linked list
    vector_desc_t empty_vect_desc;
    memset(&empty_vect_desc, 0, sizeof(vector_desc_t));

    //Level defaults to any low/med interrupt
    if (!(flags & ESP_INTR_FLAG_LEVELMASK)) {
        flags |= ESP_INTR_FLAG_LOWMED;
    }

    ALCHLOG("get_available_int: try to find existing. Cpu: %d, Source: %d", cpu, source);
    vector_desc_t *vd = find_desc_for_source(source, cpu);
    if (vd) {
        // if existing vd found, don't need to search any more.
        ALCHLOG("get_available_int: existing vd found. intno: %d", vd->intno);
        if ( force != -1 && force != vd->intno ) {
            ALCHLOG("get_available_int: intr forced but does not match existing. existing intno: %d, force: %d", vd->intno, force);
        } else if (!is_vect_desc_usable(vd, flags, cpu, force)) {
            ALCHLOG("get_available_int: existing vd invalid.");
        } else {
            best = vd->intno;
        }
        return best;
    }
    if (force != -1) {
        ALCHLOG("get_available_int: try to find force. Cpu: %d, Source: %d, Force: %d", cpu, source, force);
        //if force assigned, don't need to search any more.
        vd = find_desc_for_int(force, cpu);
        if (vd == NULL) {
            //if existing vd not found, just check the default state for the intr.
            empty_vect_desc.intno = force;
            vd = &empty_vect_desc;
        }
        if (is_vect_desc_usable(vd, flags, cpu, force)) {
            best = vd->intno;
        } else {
            ALCHLOG("get_avalaible_int: forced vd invalid.");
        }
        return best;
    }

    ALCHLOG("get_free_int: start looking. Current cpu: %d", cpu);
    /* No allocated handlers as well as forced intr, iterate over the 32 possible interrupts */
    for (x = 0; x < CPU_INT_LINES_COUNT; x++) {
        //Grab the vector_desc for this vector.
        vd = find_desc_for_int(x, cpu);
        if (vd == NULL) {
            empty_vect_desc.intno = x;
            vd = &empty_vect_desc;
        }

        esp_cpu_intr_desc_t intr_desc;
        esp_cpu_intr_get_desc(cpu, x, &intr_desc);

        ALCHLOG("Int %d reserved %d priority %d %s hasIsr %d",
            x, intr_desc.flags & ESP_CPU_INTR_DESC_FLAG_RESVD, intr_desc.priority,
            intr_desc.type == ESP_CPU_INTR_TYPE_LEVEL? "LEVEL" : "EDGE", esp_cpu_intr_has_handler(x));

        if (!is_vect_desc_usable(vd, flags, cpu, force)) {
            continue;
        }

        if (flags & ESP_INTR_FLAG_SHARED) {
            //We're allocating a shared int.

            //See if int already is used as a shared interrupt.
            if (vd->flags & VECDESC_FL_SHARED) {
                //We can use this already-marked-as-shared interrupt. Count the already attached isrs in order to see
                //how useful it is.
                int no = 0;
                shared_vector_desc_t *svdesc = vd->shared_vec_info;
                while (svdesc != NULL) {
                    no++;
                    svdesc = svdesc->next;
                }
                if (no<bestSharedCt || bestPriority > intr_desc.priority) {
                    //Seems like this shared vector is both okay and has the least amount of ISRs already attached to it.
                    best = x;
                    bestSharedCt = no;
                    bestPriority = intr_desc.priority;
                    ALCHLOG("...int %d more usable as a shared int: has %d existing vectors", x, no);
                } else {
                    ALCHLOG("...worse than int %d", best);
                }
            } else {
                if (best == -1) {
                    //We haven't found a feasible shared interrupt yet. This one is still free and usable, even if
                    //not marked as shared.
                    //Remember it in case we don't find any other shared interrupt that qualifies.
                    if (bestPriority > intr_desc.priority) {
                        best = x;
                        bestPriority = intr_desc.priority;
                        ALCHLOG("...int %d usable as a new shared int", x);
                    }
                } else {
                    ALCHLOG("...already have a shared int");
                }
            }
        } else {
            //Seems this interrupt is feasible. Select it and break out of the loop; no need to search further.
            if (bestPriority > intr_desc.priority) {
                best = x;
                bestPriority = intr_desc.priority;
            } else {
                ALCHLOG("...worse than int %d", best);
            }
        }
    }
    ALCHLOG("get_available_int: using int %d", best);

    //Okay, by now we have looked at all potential interrupts and hopefully have selected the best one in best.
    return best;
}

//Common shared isr handler. Chain-call all ISRs.
static void IRAM_ATTR shared_intr_isr(void *arg)
{
    vector_desc_t *vd = (vector_desc_t*)arg;
    shared_vector_desc_t *sh_vec = vd->shared_vec_info;
    portENTER_CRITICAL_ISR(&spinlock);
    while(sh_vec) {
        if (!sh_vec->disabled) {
            if ((sh_vec->statusreg == NULL) || (*sh_vec->statusreg & sh_vec->statusmask)) {
                traceISR_ENTER(sh_vec->source + ETS_INTERNAL_INTR_SOURCE_OFF);
                sh_vec->isr(sh_vec->arg);
                // check if we will return to scheduler or to interrupted task after ISR
                if (!os_task_switch_is_pended(esp_cpu_get_core_id())) {
                    traceISR_EXIT();
                }
            }
        }
        sh_vec = sh_vec->next;
    }
    portEXIT_CRITICAL_ISR(&spinlock);
}

#if CONFIG_APPTRACE_SV_ENABLE
//Common non-shared isr handler wrapper.
static void IRAM_ATTR non_shared_intr_isr(void *arg)
{
    non_shared_isr_arg_t *ns_isr_arg = (non_shared_isr_arg_t*)arg;
    portENTER_CRITICAL_ISR(&spinlock);
    traceISR_ENTER(ns_isr_arg->source + ETS_INTERNAL_INTR_SOURCE_OFF);
    // FIXME: can we call ISR and check os_task_switch_is_pended() after releasing spinlock?
    // when CONFIG_APPTRACE_SV_ENABLE = 0 ISRs for non-shared IRQs are called without spinlock
    ns_isr_arg->isr(ns_isr_arg->isr_arg);
    // check if we will return to scheduler or to interrupted task after ISR
    if (!os_task_switch_is_pended(esp_cpu_get_core_id())) {
        traceISR_EXIT();
    }
    portEXIT_CRITICAL_ISR(&spinlock);
}
#endif

//We use ESP_EARLY_LOG* here because this can be called before the scheduler is running.
esp_err_t esp_intr_alloc_intrstatus(int source, int flags, uint32_t intrstatusreg, uint32_t intrstatusmask, intr_handler_t handler,
                                        void *arg, intr_handle_t *ret_handle)
{
    intr_handle_data_t *ret=NULL;
    int force = -1;
    ESP_EARLY_LOGV(TAG, "esp_intr_alloc_intrstatus (cpu %u): checking args", esp_cpu_get_core_id());
    //Shared interrupts should be level-triggered.
    if ((flags & ESP_INTR_FLAG_SHARED) && (flags & ESP_INTR_FLAG_EDGE)) {
        return ESP_ERR_INVALID_ARG;
    }
    //You can't set an handler / arg for a non-C-callable interrupt.
    if ((flags & ESP_INTR_FLAG_HIGH) && (handler)) {
        return ESP_ERR_INVALID_ARG;
    }
    //Shared ints should have handler and non-processor-local source
    if ((flags & ESP_INTR_FLAG_SHARED) && (!handler || source<0)) {
        return ESP_ERR_INVALID_ARG;
    }
    //Statusreg should have a mask
    if (intrstatusreg && !intrstatusmask) {
        return ESP_ERR_INVALID_ARG;
    }
    //If the ISR is marked to be IRAM-resident, the handler must not be in the cached region
    //ToDo: if we are to allow placing interrupt handlers into the 0x400c0000—0x400c2000 region,
    //we need to make sure the interrupt is connected to the CPU0.
    //CPU1 does not have access to the RTC fast memory through this region.
    if ((flags & ESP_INTR_FLAG_IRAM) && handler && !esp_ptr_in_iram(handler) && !esp_ptr_in_rtc_iram_fast(handler)) {
        return ESP_ERR_INVALID_ARG;
    }

    //Default to prio 1 for shared interrupts. Default to prio 1, 2 or 3 for non-shared interrupts.
    if ((flags & ESP_INTR_FLAG_LEVELMASK) == 0) {
        if (flags & ESP_INTR_FLAG_SHARED) {
            flags |= ESP_INTR_FLAG_LEVEL1;
        } else {
            flags |= ESP_INTR_FLAG_LOWMED;
        }
    }
    ESP_EARLY_LOGV(TAG, "esp_intr_alloc_intrstatus (cpu %u): Args okay. Resulting flags 0x%X", esp_cpu_get_core_id(), flags);

    //Check 'special' interrupt sources. These are tied to one specific interrupt, so we
    //have to force get_free_int to only look at that.
    if (source == ETS_INTERNAL_TIMER0_INTR_SOURCE) {
        force = ETS_INTERNAL_TIMER0_INTR_NO;
    }
    if (source == ETS_INTERNAL_TIMER1_INTR_SOURCE) {
        force = ETS_INTERNAL_TIMER1_INTR_NO;
    }
    if (source == ETS_INTERNAL_TIMER2_INTR_SOURCE) {
        force = ETS_INTERNAL_TIMER2_INTR_NO;
    }
    if (source == ETS_INTERNAL_SW0_INTR_SOURCE) {
        force = ETS_INTERNAL_SW0_INTR_NO;
    }
    if (source == ETS_INTERNAL_SW1_INTR_SOURCE) {
        force = ETS_INTERNAL_SW1_INTR_NO;
    }
    if (source == ETS_INTERNAL_PROFILING_INTR_SOURCE) {
        force = ETS_INTERNAL_PROFILING_INTR_NO;
    }

    //Allocate a return handle. If we end up not needing it, we'll free it later on.
    ret = heap_caps_malloc(sizeof(intr_handle_data_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    if (ret == NULL) {
        return ESP_ERR_NO_MEM;
    }

    portENTER_CRITICAL(&spinlock);
    uint32_t cpu = esp_cpu_get_core_id();
    //See if we can find an interrupt that matches the flags.
    int intr = get_available_int(flags, cpu, force, source);
    if (intr == -1) {
        //None found. Bail out.
        portEXIT_CRITICAL(&spinlock);
        free(ret);
        ESP_LOGE(TAG, "No free interrupt inputs for %s interrupt (flags 0x%X)", esp_isr_names[source], flags);
        return ESP_ERR_NOT_FOUND;
    }
    //Get an int vector desc for int.
    vector_desc_t *vd = get_desc_for_int(intr, cpu);
    if (vd == NULL) {
        portEXIT_CRITICAL(&spinlock);
        free(ret);
        return ESP_ERR_NO_MEM;
    }

    //Allocate that int!
    if (flags & ESP_INTR_FLAG_SHARED) {
        //Populate vector entry and add to linked list.
        shared_vector_desc_t *sh_vec = heap_caps_malloc(sizeof(shared_vector_desc_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
        if (sh_vec == NULL) {
            portEXIT_CRITICAL(&spinlock);
            free(ret);
            return ESP_ERR_NO_MEM;
        }
        memset(sh_vec, 0, sizeof(shared_vector_desc_t));
        sh_vec->statusreg = (uint32_t*)intrstatusreg;
        sh_vec->statusmask = intrstatusmask;
        sh_vec->isr = handler;
        sh_vec->arg = arg;
        sh_vec->next = vd->shared_vec_info;
        sh_vec->source = source;
        sh_vec->disabled = 0;
        vd->shared_vec_info = sh_vec;
        vd->flags |= VECDESC_FL_SHARED;
        //(Re-)set shared isr handler to new value.
        esp_cpu_intr_set_handler(intr, (esp_cpu_intr_handler_t)shared_intr_isr, vd);
    } else {
        //Mark as unusable for other interrupt sources. This is ours now!
        vd->flags = VECDESC_FL_NONSHARED;
        if (handler) {
#if CONFIG_APPTRACE_SV_ENABLE
            non_shared_isr_arg_t *ns_isr_arg = heap_caps_malloc(sizeof(non_shared_isr_arg_t), MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
            if (!ns_isr_arg) {
                portEXIT_CRITICAL(&spinlock);
                free(ret);
                return ESP_ERR_NO_MEM;
            }
            ns_isr_arg->isr = handler;
            ns_isr_arg->isr_arg = arg;
            ns_isr_arg->source = source;
            esp_cpu_intr_set_handler(intr, (esp_cpu_intr_handler_t)non_shared_intr_isr, ns_isr_arg);
#else
            esp_cpu_intr_set_handler(intr, (esp_cpu_intr_handler_t)handler, arg);
#endif
        }

        if (flags & ESP_INTR_FLAG_EDGE) {
            esp_cpu_intr_edge_ack(intr);
        }

        vd->source = source;
    }
    if (flags & ESP_INTR_FLAG_IRAM) {
        vd->flags |= VECDESC_FL_INIRAM;
        non_iram_int_mask[cpu] &= ~(1<<intr);
    } else {
        vd->flags &= ~VECDESC_FL_INIRAM;
        non_iram_int_mask[cpu] |= (1<<intr);
    }
    if (source>=0) {
        esp_rom_route_intr_matrix(cpu, source, intr);
    }

    //Fill return handle data.
    ret->vector_desc = vd;
    ret->shared_vector_desc = vd->shared_vec_info;

    //Enable int at CPU-level;
    ESP_INTR_ENABLE(intr);

    //If interrupt has to be started disabled, do that now; ints won't be enabled for real until the end
    //of the critical section.
    if (flags & ESP_INTR_FLAG_INTRDISABLED) {
        esp_intr_disable(ret);
    }

#ifdef SOC_CPU_HAS_FLEXIBLE_INTC
    //Extract the level from the interrupt passed flags
    int level = esp_intr_flags_to_level(flags);
    esp_cpu_intr_set_priority(intr, level);

    if (flags & ESP_INTR_FLAG_EDGE) {
        esp_cpu_intr_set_type(intr, ESP_CPU_INTR_TYPE_EDGE);
    } else {
        esp_cpu_intr_set_type(intr, ESP_CPU_INTR_TYPE_LEVEL);
    }
#endif

    portEXIT_CRITICAL(&spinlock);

    //Fill return handle if needed, otherwise free handle.
    if (ret_handle != NULL) {
        *ret_handle = ret;
    } else {
        free(ret);
    }

    ESP_EARLY_LOGD(TAG, "Connected src %d to int %d (cpu %"PRIu32")", source, intr, cpu);
    return ESP_OK;
}

esp_err_t esp_intr_alloc(int source, int flags, intr_handler_t handler, void *arg, intr_handle_t *ret_handle)
{
    /*
      As an optimization, we can create a table with the possible interrupt status registers and masks for every single
      source there is. We can then add code here to look up an applicable value and pass that to the
      esp_intr_alloc_intrstatus function.
    */
    return esp_intr_alloc_intrstatus(source, flags, 0, 0, handler, arg, ret_handle);
}

esp_err_t IRAM_ATTR esp_intr_set_in_iram(intr_handle_t handle, bool is_in_iram)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    vector_desc_t *vd = handle->vector_desc;
    if (vd->flags & VECDESC_FL_SHARED) {
      return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL(&spinlock);
    uint32_t mask = (1 << vd->intno);
    if (is_in_iram) {
        vd->flags |= VECDESC_FL_INIRAM;
        non_iram_int_mask[vd->cpu] &= ~mask;
    } else {
        vd->flags &= ~VECDESC_FL_INIRAM;
        non_iram_int_mask[vd->cpu] |= mask;
    }
    portEXIT_CRITICAL(&spinlock);
    return ESP_OK;
}

#if !CONFIG_FREERTOS_UNICORE
static void esp_intr_free_cb(void *arg)
{
    (void)esp_intr_free((intr_handle_t)arg);
}
#endif /* !CONFIG_FREERTOS_UNICORE */

esp_err_t esp_intr_free(intr_handle_t handle)
{
    bool free_shared_vector=false;
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }

#if !CONFIG_FREERTOS_UNICORE
    //Assign this routine to the core where this interrupt is allocated on.
    if (handle->vector_desc->cpu != esp_cpu_get_core_id()) {
        esp_err_t ret = esp_ipc_call_blocking(handle->vector_desc->cpu, &esp_intr_free_cb, (void *)handle);
        return ret == ESP_OK ? ESP_OK : ESP_FAIL;
    }
#endif /* !CONFIG_FREERTOS_UNICORE */

    portENTER_CRITICAL(&spinlock);
    esp_intr_disable(handle);
    if (handle->vector_desc->flags & VECDESC_FL_SHARED) {
        //Find and kill the shared int
        shared_vector_desc_t *svd = handle->vector_desc->shared_vec_info;
        shared_vector_desc_t *prevsvd = NULL;
        assert(svd); //should be something in there for a shared int
        while (svd != NULL) {
            if (svd == handle->shared_vector_desc) {
                //Found it. Now kill it.
                if (prevsvd) {
                    prevsvd->next = svd->next;
                } else {
                    handle->vector_desc->shared_vec_info = svd->next;
                }
                free(svd);
                break;
            }
            prevsvd = svd;
            svd = svd->next;
        }
        //If nothing left, disable interrupt.
        if (handle->vector_desc->shared_vec_info == NULL) {
            free_shared_vector = true;
        }
        ESP_EARLY_LOGV(TAG,
                       "esp_intr_free: Deleting shared int: %s. Shared int is %s",
                       svd ? "not found or last one" : "deleted",
                       free_shared_vector ? "empty now." : "still in use");
    }

    if ((handle->vector_desc->flags & VECDESC_FL_NONSHARED) || free_shared_vector) {
        ESP_EARLY_LOGV(TAG, "esp_intr_free: Disabling int, killing handler");
#if CONFIG_APPTRACE_SV_ENABLE
        if (!free_shared_vector) {
            void *isr_arg = esp_cpu_intr_get_handler_arg(handle->vector_desc->intno);
            if (isr_arg) {
                free(isr_arg);
            }
        }
#endif
        //Reset to normal handler:
        esp_cpu_intr_set_handler(handle->vector_desc->intno, NULL, (void*)((int)handle->vector_desc->intno));
        //Theoretically, we could free the vector_desc... not sure if that's worth the few bytes of memory
        //we save.(We can also not use the same exit path for empty shared ints anymore if we delete
        //the desc.) For now, just mark it as free.
        handle->vector_desc->flags &= ~(VECDESC_FL_NONSHARED|VECDESC_FL_RESERVED|VECDESC_FL_SHARED);
        handle->vector_desc->source = ETS_INTERNAL_UNUSED_INTR_SOURCE;

        //Also kill non_iram mask bit.
        non_iram_int_mask[handle->vector_desc->cpu] &= ~(1<<(handle->vector_desc->intno));
    }
    portEXIT_CRITICAL(&spinlock);
    free(handle);
    return ESP_OK;
}

int esp_intr_get_intno(intr_handle_t handle)
{
    return handle->vector_desc->intno;
}

int esp_intr_get_cpu(intr_handle_t handle)
{
    return handle->vector_desc->cpu;
}

/*
 Interrupt disabling strategy:
 If the source is >=0 (meaning a muxed interrupt), we disable it by muxing the interrupt to a non-connected
 interrupt. If the source is <0 (meaning an internal, per-cpu interrupt), we disable it using ESP_INTR_DISABLE.
 This allows us to, for the muxed CPUs, disable an int from the other core. It also allows disabling shared
 interrupts.
 */

//Muxing an interrupt source to interrupt 6, 7, 11, 15, 16 or 29 cause the interrupt to effectively be disabled.
#define INT_MUX_DISABLED_INTNO 6

esp_err_t IRAM_ATTR esp_intr_enable(intr_handle_t handle)
{
    if (!handle) {
        return ESP_ERR_INVALID_ARG;
    }
    portENTER_CRITICAL_SAFE(&spinlock);
    int source;
    if (handle->shared_vector_desc) {
        handle->shared_vector_desc->disabled = 0;
        source=handle->shared_vector_desc->source;
    } else {
        source=handle->vector_desc->source;
    }
    if (source >= 0) {
        //Disabled using int matrix; re-connect to enable
        esp_rom_route_intr_matrix(handle->vector_desc->cpu, source, handle->vector_desc->intno);
    } else {
        //Re-enable using cpu int ena reg
        if (handle->vector_desc->cpu != esp_cpu_get_core_id()) {
            portEXIT_CRITICAL_SAFE(&spinlock);
            return ESP_ERR_INVALID_ARG; //Can only enable these ints on this cpu
        }
        ESP_INTR_ENABLE(handle->vector_desc->intno);
    }
    portEXIT_CRITICAL_SAFE(&spinlock);
    return ESP_OK;
}

esp_err_t IRAM_ATTR esp_intr_disable(intr_handle_t handle)
{
    if (handle == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    portENTER_CRITICAL_SAFE(&spinlock);
    int source;
    bool disabled = true;
    if (handle->shared_vector_desc) {
        handle->shared_vector_desc->disabled = 1;
        source=handle->shared_vector_desc->source;

        shared_vector_desc_t *svd = handle->vector_desc->shared_vec_info;
        assert(svd != NULL);
        while(svd) {
            if (svd->source == source && !svd->disabled) {
                disabled = false;
                break;
            }
            svd = svd->next;
        }
    } else {
        source=handle->vector_desc->source;
    }

    if (source >= 0) {
        if (disabled) {
            //Disable using int matrix
            esp_rom_route_intr_matrix(handle->vector_desc->cpu, source, INT_MUX_DISABLED_INTNO);
        }
    } else {
        //Disable using per-cpu regs
        if (handle->vector_desc->cpu != esp_cpu_get_core_id()) {
            portEXIT_CRITICAL_SAFE(&spinlock);
            return ESP_ERR_INVALID_ARG; //Can only enable these ints on this cpu
        }
        ESP_INTR_DISABLE(handle->vector_desc->intno);
    }
    portEXIT_CRITICAL_SAFE(&spinlock);
    return ESP_OK;
}

void IRAM_ATTR esp_intr_noniram_disable(void)
{
    portENTER_CRITICAL_SAFE(&spinlock);
    uint32_t oldint;
    uint32_t cpu = esp_cpu_get_core_id();
    uint32_t non_iram_ints = non_iram_int_mask[cpu];
    if (non_iram_int_disabled_flag[cpu]) {
        abort();
    }
    non_iram_int_disabled_flag[cpu] = true;
    oldint = esp_cpu_intr_get_enabled_mask();
    esp_cpu_intr_disable(non_iram_ints);
    // Disable the RTC bit which don't want to be put in IRAM.
    rtc_isr_noniram_disable(cpu);
    // Save disabled ints
    non_iram_int_disabled[cpu] = oldint & non_iram_ints;
    portEXIT_CRITICAL_SAFE(&spinlock);
}

void IRAM_ATTR esp_intr_noniram_enable(void)
{
    portENTER_CRITICAL_SAFE(&spinlock);
    uint32_t cpu = esp_cpu_get_core_id();
    int non_iram_ints = non_iram_int_disabled[cpu];
    if (!non_iram_int_disabled_flag[cpu]) {
        abort();
    }
    non_iram_int_disabled_flag[cpu] = false;
    esp_cpu_intr_enable(non_iram_ints);
    rtc_isr_noniram_enable(cpu);
    portEXIT_CRITICAL_SAFE(&spinlock);
}

//These functions are provided in ROM, but the ROM-based functions use non-multicore-capable
//virtualized interrupt levels. Thus, we disable them in the ld file and provide working
//equivalents here.


void IRAM_ATTR ets_isr_unmask(uint32_t mask) {
    esp_cpu_intr_enable(mask);
}

void IRAM_ATTR ets_isr_mask(uint32_t mask) {
    esp_cpu_intr_disable(mask);
}

void IRAM_ATTR esp_intr_enable_source(int inum)
{
    esp_cpu_intr_enable(1 << inum);
}

void IRAM_ATTR esp_intr_disable_source(int inum)
{
    esp_cpu_intr_disable(1 << inum);
}

esp_err_t esp_intr_dump(FILE *stream)
{
    if (stream == NULL) {
        stream = stdout;
    }
#ifdef CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE
    const int cpu_num = 1;
#else
    const int cpu_num = SOC_CPU_CORES_NUM;
#endif

    int general_use_ints_free = 0;
    int shared_ints = 0;

    for (int cpu = 0; cpu < cpu_num; ++cpu) {
        fprintf(stream, "CPU %d interrupt status:\n", cpu);
        fprintf(stream, " Int  Level  Type   Status\n");
        for (int i_num = 0; i_num < CPU_INT_LINES_COUNT; ++i_num) {
            fprintf(stream, " %2d  ", i_num);
            esp_cpu_intr_desc_t intr_desc;
            esp_cpu_intr_get_desc(cpu, i_num, &intr_desc);
            bool is_general_use = true;
            vector_desc_t *vd = find_desc_for_int(i_num, cpu);

#ifndef SOC_CPU_HAS_FLEXIBLE_INTC
            fprintf(stream, "   %d    %s  ",
                    intr_desc.priority,
                    intr_desc.type == ESP_CPU_INTR_TYPE_EDGE ? "Edge " : "Level");

            is_general_use = (intr_desc.type == ESP_CPU_INTR_TYPE_LEVEL) && (intr_desc.priority <= XCHAL_EXCM_LEVEL);
#else // SOC_CPU_HAS_FLEXIBLE_INTC
            if (vd == NULL) {
                fprintf(stream, "   *      *    ");
            } else {
                // esp_cpu_intr_get_* functions need to be extended with cpu parameter.
                // Showing info for the current cpu only, in the meantime.
                if (esp_cpu_get_core_id() == cpu) {
                    fprintf(stream, "   %d    %s  ",
                            esp_cpu_intr_get_priority(i_num),
                            esp_cpu_intr_get_type(i_num) == ESP_CPU_INTR_TYPE_EDGE ? "Edge " : "Level");
                } else {
                    fprintf(stream, "   ?      ?    ");
                }
            }
#endif // SOC_CPU_HAS_FLEXIBLE_INTC

            if (intr_desc.flags & ESP_CPU_INTR_DESC_FLAG_RESVD) {
                fprintf(stream, "Reserved");
            } else if (intr_desc.flags & ESP_CPU_INTR_DESC_FLAG_SPECIAL) {
                fprintf(stream, "CPU-internal");
            } else {
                if (vd == NULL || (vd->flags & (VECDESC_FL_RESERVED | VECDESC_FL_NONSHARED | VECDESC_FL_SHARED)) == 0) {
                    fprintf(stream, "Free");
                    if (is_general_use) {
                        ++general_use_ints_free;
                    } else {
                        fprintf(stream, " (not general-use)");
                    }
                } else if (vd->flags & VECDESC_FL_RESERVED)  {
                    fprintf(stream, "Reserved (run-time)");
                } else if (vd->flags & VECDESC_FL_NONSHARED) {
                    fprintf(stream, "Used: %s", esp_isr_names[vd->source]);
                } else if (vd->flags & VECDESC_FL_SHARED) {
                    fprintf(stream, "Shared: ");
                    for (shared_vector_desc_t *svd = vd->shared_vec_info; svd != NULL; svd = svd->next) {
                        fprintf(stream, "%s ", esp_isr_names[svd->source]);
                    }
                    ++shared_ints;
                } else {
                    fprintf(stream, "Unknown, flags = 0x%x", vd->flags);
                }
            }

            fprintf(stream, "\n");
        }
    }
    fprintf(stream, "Interrupts available for general use: %d\n", general_use_ints_free);
    fprintf(stream, "Shared interrupts: %d\n", shared_ints);
    return ESP_OK;
}
