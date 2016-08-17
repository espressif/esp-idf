/** @file */

// xos_syslog.h - XOS Event logging module.

// Copyright (c) 2003-2015 Cadence Design Systems, Inc.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
// SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// NOTE: Do not include this file directly in your application. Including
// xos.h will automatically include this file.


#ifndef __XOS_SYSLOG_H__
#define __XOS_SYSLOG_H__

#include "xos_types.h"

#ifdef __cplusplus
extern "C" {
#endif


//-----------------------------------------------------------------------------
//  The XOS system log is an array of fixed size entries. The size of the log
//  is determined by the application, and memory for the log must be provided
//  at init time. Every time the log function is called, an entry is made in
//  the log and the next pointer advanced. When the log is full, it will wrap
//  around and start overwriting the oldest entries.
//  Logging can be done from C/C++ code as well as assembly code, and at any
//  interrupt level, even from high level interrupt handlers.
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//  Defines.
//-----------------------------------------------------------------------------
#define XOS_SYSLOG_ENABLED              0x0001


///----------------------------------------------------------------------------
///
/// Use this macro to compute how much memory to allocate for the syslog.
///
///----------------------------------------------------------------------------
#define XOS_SYSLOG_SIZE(num_entries) \
    ( sizeof(XosSysLog) + ((num_entries - 1) * sizeof(XosSysLogEntry)) )


///----------------------------------------------------------------------------
///
///  System log entry structure.
///
///----------------------------------------------------------------------------
typedef struct XosSysLogEntry {
    uint32_t                timestamp;  ///< Timestamp in clock cycles
    uint32_t                param1;     ///< User defined value
    uint32_t                param2;     ///< User defined value
    struct XosSysLogEntry * next;       ///< Link to next entry
} XosSysLogEntry;


///----------------------------------------------------------------------------
///
///  System log structure.
///
///----------------------------------------------------------------------------
typedef struct XosSysLog {
    uint16_t         flags;             ///< Flags
    uint16_t         size;              ///< Number of entries
    XosSysLogEntry * next;              ///< Next write position
    XosSysLogEntry   entries[1];        ///< First entry
} XosSysLog;


//-----------------------------------------------------------------------------
//  Pointer to syslog area.
//-----------------------------------------------------------------------------
extern XosSysLog * xos_syslog;


//----------------------------------------------------------------------------
///
///  Initialize the syslog. Initializing the log also enables it. The system
///  log always wraps around when full and overwrites the oldest entries.
///
///  \param     log_mem         Pointer to allocated memory for the log.
///
///  \param     num_entries     The number of entries that the log can contain.
///
///  \return Returns nothing.
///
//----------------------------------------------------------------------------
static inline void
xos_syslog_init(void * log_mem, uint16_t num_entries)
{
    uint16_t i;

    xos_syslog = (XosSysLog *) log_mem;
    xos_syslog->size = num_entries;
    xos_syslog->next = xos_syslog->entries;

    for (i = 0; i < num_entries - 1; i++) {
        xos_syslog->entries[i].next = &(xos_syslog->entries[i+1]);
        xos_syslog->entries[i].timestamp = 0;
    }
    xos_syslog->entries[i].next = xos_syslog->entries;
    xos_syslog->entries[i].timestamp = 0;

    xos_syslog->flags = XOS_SYSLOG_ENABLED;
}


///----------------------------------------------------------------------------
///
///  Reset the syslog. All entries made up to now are abandoned and the write
///  pointer is set to the first entry location.
///
///  No parameters.
///
///  \return Returns nothing.
///
///----------------------------------------------------------------------------
static inline void
xos_syslog_clear()
{
#if XCHAL_HAVE_INTERRUPTS
    uint32_t ps = XT_RSIL(XCHAL_NUM_INTLEVELS);
#endif

    xos_syslog_init(xos_syslog, xos_syslog->size);
#if XCHAL_HAVE_INTERRUPTS
    xos_restore_int_pri_level(ps);
#endif
}


///----------------------------------------------------------------------------
///
///  Enable logging to the syslog. This function needs to be called only if
///  logging had been previously disabled via xos_syslog_disable(), since
///  initializing the syslog automatically enables it.
///
///  No parameters.
///
///  \return Returns nothing.
///
///----------------------------------------------------------------------------
static inline void
xos_syslog_enable() 
{
#if XCHAL_HAVE_INTERRUPTS
    uint32_t ps = XT_RSIL(XCHAL_NUM_INTLEVELS);
#endif

    xos_syslog->flags |= XOS_SYSLOG_ENABLED;
#if XCHAL_HAVE_INTERRUPTS
    xos_restore_int_pri_level(ps);
#endif
}


///----------------------------------------------------------------------------
///
///  Disable logging to the syslog. It is sometimes useful to disable logging
///  while the log is being examined or dumped.
///
///  No parameters.
///
///  \return Returns nothing.
///
///----------------------------------------------------------------------------
static inline void
xos_syslog_disable()
{
#if XCHAL_HAVE_INTERRUPTS
    uint32_t ps = XT_RSIL(XCHAL_NUM_INTLEVELS);
#endif
    xos_syslog->flags &= ~XOS_SYSLOG_ENABLED;
#if XCHAL_HAVE_INTERRUPTS
    xos_restore_int_pri_level(ps);
#endif
}


///----------------------------------------------------------------------------
///
///  Write an entry into the syslog. This function does disable all interrupts
///  since logging can be done from interrupt handlers as well. It will write
///  into the log only if the log exists and is enabled.
///
///  \param     param1              User defined value.
///
///  \param     param2              User defined value.
///
///  \return Returns nothing.
///
///----------------------------------------------------------------------------
static inline void
xos_syslog_write(uint32_t param1, uint32_t param2)
{
    if (xos_syslog != XOS_NULL) {
#if XCHAL_HAVE_INTERRUPTS
        uint32_t ps = XT_RSIL(XCHAL_NUM_INTLEVELS);
#endif

        if ((xos_syslog->flags & XOS_SYSLOG_ENABLED) != 0) {
            XosSysLogEntry * next = xos_syslog->next;

            next->timestamp = xos_get_ccount();
            next->param1    = param1;
            next->param2    = param2;

            xos_syslog->next = next->next;
        }

#if XCHAL_HAVE_INTERRUPTS
        xos_restore_int_pri_level(ps);
#endif
    }
}


///----------------------------------------------------------------------------
///
///  Read the first (oldest) entry in the syslog. Will return an error if the
///  log has not been created or is empty. Storage to copy the entry must be
///  provided by the caller.
///
///  \param     entry       Pointer to storage where the entry data will be
///                         copied. This pointer must be passed to 
///                         xos_syslog_get_next().
///
///  \return Returns XOS_OK on success, else error code.
///
///----------------------------------------------------------------------------
static inline int32_t
xos_syslog_get_first(XosSysLogEntry * entry)
{
    if (xos_syslog == XOS_NULL) {
        return XOS_ERR_NOT_FOUND;
    }

    if (entry != XOS_NULL) {
#if XCHAL_HAVE_INTERRUPTS
        uint32_t         ps   = XT_RSIL(XCHAL_NUM_INTLEVELS);
#endif
        XosSysLogEntry * next = xos_syslog->next;

        // 'next' should be pointing to the next entry to be overwritten, if we
        // have wrapped. This means it is the oldest entry. However if this entry
        // has a zero timestamp then we have not wrapped, in which case we must
        // look at the first entry in the list.
        if (next->timestamp == 0) {
            next = xos_syslog->entries;
        }

        *entry = *next;
#if XCHAL_HAVE_INTERRUPTS
        xos_restore_int_pri_level(ps);
#endif
        return entry->timestamp ? XOS_OK : XOS_ERR_NOT_FOUND;
    }

    return XOS_ERR_INVALID_PARAMETER;
}


///----------------------------------------------------------------------------
///
///  Get the next sequential entry from the syslog. This function must be called
///  only after xos_syslog_get_first() has been called.
///
///  \param     entry       Pointer to storage where entry data will be copied.
///                         Must be the same pointer that was passed in the call
///                         to xos_syslog_get_first(), as it is used to keep track
///                         of the current position.
///
///  \return Returns XOS_OK on success, else error code.
///
///----------------------------------------------------------------------------
static inline int32_t
xos_syslog_get_next(XosSysLogEntry * entry)
{
    if (entry != XOS_NULL) {
#if XCHAL_HAVE_INTERRUPTS
        uint32_t         ps   = XT_RSIL(XCHAL_NUM_INTLEVELS);
#endif
        XosSysLogEntry * next = entry->next;
        int32_t          ret  = XOS_OK;

        // Make sure we're not pointing past the last entry.
        if ((next != XOS_NULL) && (next != xos_syslog->next) && (next->timestamp != 0)) {
            *entry = *next;
        }
        else {
            ret = XOS_ERR_NOT_FOUND;
        }
#if XCHAL_HAVE_INTERRUPTS
        xos_restore_int_pri_level(ps);
#endif
        return ret;
    }

    return XOS_ERR_INVALID_PARAMETER;
}


#ifdef __cplusplus
}
#endif

#endif // __XOS_SYSLOG_H__

