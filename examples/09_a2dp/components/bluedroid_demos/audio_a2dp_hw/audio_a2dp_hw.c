/******************************************************************************
 *
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*****************************************************************************
 *
 *  Filename:      audio_a2dp_hw.c
 *
 *  Description:   Implements hal for bluedroid a2dp audio device
 *
 *****************************************************************************/

#include <errno.h>
#include <inttypes.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cutils/str_parms.h>
#include <cutils/sockets.h>

#include <system/audio.h>
#include <hardware/audio.h>

#include <hardware/hardware.h>
#include "audio_a2dp_hw.h"
#include "bt_utils.h"

#define LOG_TAG "bt_a2dp_hw"
#include "osi/include/log.h"

/*****************************************************************************
**  Constants & Macros
******************************************************************************/

#define CTRL_CHAN_RETRY_COUNT 3
#define USEC_PER_SEC 1000000L

#define CASE_RETURN_STR(const) case const: return #const;

#define FNLOG()             LOG_VERBOSE("%s", __FUNCTION__);
#define DEBUG(fmt, ...)     LOG_VERBOSE("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define INFO(fmt, ...)      LOG_INFO("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)
#define ERROR(fmt, ...)     LOG_ERROR("%s: " fmt,__FUNCTION__, ## __VA_ARGS__)

#define ASSERTC(cond, msg, val) if (!(cond)) {ERROR("### ASSERT : %s line %d %s (%d) ###", __FILE__, __LINE__, msg, val);}

/*****************************************************************************
**  Local type definitions
******************************************************************************/

typedef enum {
    AUDIO_A2DP_STATE_STARTING,
    AUDIO_A2DP_STATE_STARTED,
    AUDIO_A2DP_STATE_STOPPING,
    AUDIO_A2DP_STATE_STOPPED,
    AUDIO_A2DP_STATE_SUSPENDED, /* need explicit set param call to resume (suspend=false) */
    AUDIO_A2DP_STATE_STANDBY    /* allows write to autoresume */
} a2dp_state_t;

struct a2dp_stream_in;
struct a2dp_stream_out;

struct a2dp_audio_device {
    struct audio_hw_device device;
    struct a2dp_stream_in  *input;
    struct a2dp_stream_out *output;
};

struct a2dp_config {
    uint32_t                rate;
    uint32_t                channel_flags;
    int                     format;
};

/* move ctrl_fd outside output stream and keep open until HAL unloaded ? */

struct a2dp_stream_common {
    pthread_mutex_t         lock;
    int                     ctrl_fd;
    int                     audio_fd;
    size_t                  buffer_sz;
    struct a2dp_config      cfg;
    a2dp_state_t            state;
};

struct a2dp_stream_out {
    struct audio_stream_out stream;
    struct a2dp_stream_common common;
    uint64_t frames_presented; // frames written, never reset
    uint64_t frames_rendered;  // frames written, reset on standby
};

struct a2dp_stream_in {
    struct audio_stream_in  stream;
    struct a2dp_stream_common common;
};

/*****************************************************************************
**  Static variables
******************************************************************************/

/*****************************************************************************
**  Static functions
******************************************************************************/

static size_t out_get_buffer_size(const struct audio_stream *stream);

/*****************************************************************************
**  Externs
******************************************************************************/

/*****************************************************************************
**  Functions
******************************************************************************/

/*****************************************************************************
**   Miscellaneous helper functions
******************************************************************************/

static const char* dump_a2dp_ctrl_event(char event)
{
    switch(event)
    {
        CASE_RETURN_STR(A2DP_CTRL_CMD_NONE)
        CASE_RETURN_STR(A2DP_CTRL_CMD_CHECK_READY)
        CASE_RETURN_STR(A2DP_CTRL_CMD_START)
        CASE_RETURN_STR(A2DP_CTRL_CMD_STOP)
        CASE_RETURN_STR(A2DP_CTRL_CMD_SUSPEND)
        default:
            return "UNKNOWN MSG ID";
    }
}

/* logs timestamp with microsec precision
   pprev is optional in case a dedicated diff is required */
static void ts_log(char *tag, int val, struct timespec *pprev_opt)
{
    struct timespec now;
    static struct timespec prev = {0,0};
    unsigned long long now_us;
    unsigned long long diff_us;
    UNUSED(tag);
    UNUSED(val);

    clock_gettime(CLOCK_MONOTONIC, &now);

    now_us = now.tv_sec*USEC_PER_SEC + now.tv_nsec/1000;

    if (pprev_opt)
    {
        diff_us = (now.tv_sec - prev.tv_sec) * USEC_PER_SEC + (now.tv_nsec - prev.tv_nsec)/1000;
        *pprev_opt = now;
        DEBUG("[%s] ts %08lld, *diff %08lld, val %d", tag, now_us, diff_us, val);
    }
    else
    {
        diff_us = (now.tv_sec - prev.tv_sec) * USEC_PER_SEC + (now.tv_nsec - prev.tv_nsec)/1000;
        prev = now;
        DEBUG("[%s] ts %08lld, diff %08lld, val %d", tag, now_us, diff_us, val);
    }
}

static int calc_audiotime(struct a2dp_config cfg, int bytes)
{
    int chan_count = popcount(cfg.channel_flags);

    ASSERTC(cfg.format == AUDIO_FORMAT_PCM_16_BIT,
            "unsupported sample sz", cfg.format);

    return bytes*(1000000/(chan_count*2))/cfg.rate;
}

/*****************************************************************************
**
**   bluedroid stack adaptation
**
*****************************************************************************/

static int skt_connect(char *path, size_t buffer_sz)
{
    int ret;
    int skt_fd;
    struct sockaddr_un remote;
    int len;

    INFO("connect to %s (sz %zu)", path, buffer_sz);

    skt_fd = socket(AF_LOCAL, SOCK_STREAM, 0);

    if(socket_local_client_connect(skt_fd, path,
            ANDROID_SOCKET_NAMESPACE_ABSTRACT, SOCK_STREAM) < 0)
    {
        ERROR("failed to connect (%s)", strerror(errno));
        close(skt_fd);
        return -1;
    }

    len = buffer_sz;
    ret = setsockopt(skt_fd, SOL_SOCKET, SO_SNDBUF, (char*)&len, (int)sizeof(len));

    /* only issue warning if failed */
    if (ret < 0)
        ERROR("setsockopt failed (%s)", strerror(errno));

    ret = setsockopt(skt_fd, SOL_SOCKET, SO_RCVBUF, (char*)&len, (int)sizeof(len));

    /* only issue warning if failed */
    if (ret < 0)
        ERROR("setsockopt failed (%s)", strerror(errno));

    INFO("connected to stack fd = %d", skt_fd);

    return skt_fd;
}

static int skt_read(int fd, void *p, size_t len)
{
    int read;
    struct pollfd pfd;
    struct timespec ts;

    FNLOG();

    ts_log("skt_read recv", len, NULL);

    if ((read = recv(fd, p, len, MSG_NOSIGNAL)) == -1)
    {
        ERROR("write failed with errno=%d\n", errno);
        return -1;
    }

    return read;
}

static int skt_write(int fd, const void *p, size_t len)
{
    int sent;
    struct pollfd pfd;

    FNLOG();

    pfd.fd = fd;
    pfd.events = POLLOUT;

    /* poll for 500 ms */

    /* send time out */
    if (poll(&pfd, 1, 500) == 0)
        return 0;

    ts_log("skt_write", len, NULL);

    if ((sent = send(fd, p, len, MSG_NOSIGNAL)) == -1)
    {
        ERROR("write failed with errno=%d\n", errno);
        return -1;
    }

    return sent;
}

static int skt_disconnect(int fd)
{
    INFO("fd %d", fd);

    if (fd != AUDIO_SKT_DISCONNECTED)
    {
        shutdown(fd, SHUT_RDWR);
        close(fd);
    }
    return 0;
}



/*****************************************************************************
**
**  AUDIO CONTROL PATH
**
*****************************************************************************/

static int a2dp_ctrl_receive(struct a2dp_stream_common *common, void* buffer, int length)
{
    int ret = recv(common->ctrl_fd, buffer, length, MSG_NOSIGNAL);
    if (ret < 0)
    {
        ERROR("ack failed (%s)", strerror(errno));
        if (errno == EINTR)
        {
            /* retry again */
            ret = recv(common->ctrl_fd, buffer, length, MSG_NOSIGNAL);
            if (ret < 0)
            {
               ERROR("ack failed (%s)", strerror(errno));
               skt_disconnect(common->ctrl_fd);
               common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
               return -1;
            }
        }
        else
        {
               skt_disconnect(common->ctrl_fd);
               common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
               return -1;

        }
    }
    return ret;
}

static int a2dp_command(struct a2dp_stream_common *common, char cmd)
{
    char ack;

    DEBUG("A2DP COMMAND %s", dump_a2dp_ctrl_event(cmd));

    /* send command */
    if (send(common->ctrl_fd, &cmd, 1, MSG_NOSIGNAL) == -1)
    {
        ERROR("cmd failed (%s)", strerror(errno));
        skt_disconnect(common->ctrl_fd);
        common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
        return -1;
    }

    /* wait for ack byte */
    if (a2dp_ctrl_receive(common, &ack, 1) < 0)
        return -1;

    DEBUG("A2DP COMMAND %s DONE STATUS %d", dump_a2dp_ctrl_event(cmd), ack);

    if (ack == A2DP_CTRL_ACK_INCALL_FAILURE)
        return ack;
    if (ack != A2DP_CTRL_ACK_SUCCESS)
        return -1;

    return 0;
}

static int check_a2dp_ready(struct a2dp_stream_common *common)
{
    if (a2dp_command(common, A2DP_CTRL_CMD_CHECK_READY) < 0)
    {
        ERROR("check a2dp ready failed");
        return -1;
    }
    return 0;
}

static int a2dp_read_audio_config(struct a2dp_stream_common *common)
{
    char cmd = A2DP_CTRL_GET_AUDIO_CONFIG;
    uint32_t sample_rate;
    uint8_t channel_count;

    if (a2dp_command(common, A2DP_CTRL_GET_AUDIO_CONFIG) < 0)
    {
        ERROR("check a2dp ready failed");
        return -1;
    }

    if (a2dp_ctrl_receive(common, &sample_rate, 4) < 0)
        return -1;
    if (a2dp_ctrl_receive(common, &channel_count, 1) < 0)
        return -1;

    common->cfg.channel_flags = (channel_count == 1 ? AUDIO_CHANNEL_IN_MONO : AUDIO_CHANNEL_IN_STEREO);
    common->cfg.format = AUDIO_STREAM_DEFAULT_FORMAT;
    common->cfg.rate = sample_rate;

    INFO("got config %d %d", common->cfg.format, common->cfg.rate);

    return 0;
}

static void a2dp_open_ctrl_path(struct a2dp_stream_common *common)
{
    int i;

    /* retry logic to catch any timing variations on control channel */
    for (i = 0; i < CTRL_CHAN_RETRY_COUNT; i++)
    {
        /* connect control channel if not already connected */
        if ((common->ctrl_fd = skt_connect(A2DP_CTRL_PATH, common->buffer_sz)) > 0)
        {
            /* success, now check if stack is ready */
            if (check_a2dp_ready(common) == 0)
                break;

            ERROR("error : a2dp not ready, wait 250 ms and retry");
            usleep(250000);
            skt_disconnect(common->ctrl_fd);
            common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
        }

        /* ctrl channel not ready, wait a bit */
        usleep(250000);
    }
}

/*****************************************************************************
**
** AUDIO DATA PATH
**
*****************************************************************************/

static void a2dp_stream_common_init(struct a2dp_stream_common *common)
{
    pthread_mutexattr_t lock_attr;

    FNLOG();

    pthread_mutexattr_init(&lock_attr);
    pthread_mutexattr_settype(&lock_attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&common->lock, &lock_attr);

    common->ctrl_fd = AUDIO_SKT_DISCONNECTED;
    common->audio_fd = AUDIO_SKT_DISCONNECTED;
    common->state = AUDIO_A2DP_STATE_STOPPED;

    /* manages max capacity of socket pipe */
    common->buffer_sz = AUDIO_STREAM_OUTPUT_BUFFER_SZ;
}

static int start_audio_datapath(struct a2dp_stream_common *common)
{
    INFO("state %d", common->state);

    if (common->ctrl_fd == AUDIO_SKT_DISCONNECTED) {
        INFO("%s AUDIO_SKT_DISCONNECTED", __func__);
        return -1;
    }

    int oldstate = common->state;
    common->state = AUDIO_A2DP_STATE_STARTING;

    int a2dp_status = a2dp_command(common, A2DP_CTRL_CMD_START);
    if (a2dp_status < 0)
    {
        ERROR("%s Audiopath start failed (status %d)", __func__, a2dp_status);

        common->state = oldstate;
        return -1;
    }
    else if (a2dp_status == A2DP_CTRL_ACK_INCALL_FAILURE)
    {
        ERROR("%s Audiopath start failed - in call, move to suspended", __func__);
        common->state = oldstate;
        return -1;
    }

    /* connect socket if not yet connected */
    if (common->audio_fd == AUDIO_SKT_DISCONNECTED)
    {
        common->audio_fd = skt_connect(A2DP_DATA_PATH, common->buffer_sz);
        if (common->audio_fd < 0)
        {
            common->state = oldstate;
            return -1;
        }

        common->state = AUDIO_A2DP_STATE_STARTED;
    }

    return 0;
}

static int stop_audio_datapath(struct a2dp_stream_common *common)
{
    int oldstate = common->state;

    INFO("state %d", common->state);

    if (common->ctrl_fd == AUDIO_SKT_DISCONNECTED)
         return -1;

    /* prevent any stray output writes from autostarting the stream
       while stopping audiopath */
    common->state = AUDIO_A2DP_STATE_STOPPING;

    if (a2dp_command(common, A2DP_CTRL_CMD_STOP) < 0)
    {
        ERROR("audiopath stop failed");
        common->state = oldstate;
        return -1;
    }

    common->state = AUDIO_A2DP_STATE_STOPPED;

    /* disconnect audio path */
    skt_disconnect(common->audio_fd);
    common->audio_fd = AUDIO_SKT_DISCONNECTED;

    return 0;
}

static int suspend_audio_datapath(struct a2dp_stream_common *common, bool standby)
{
    INFO("state %d", common->state);

    if (common->ctrl_fd == AUDIO_SKT_DISCONNECTED)
         return -1;

    if (common->state == AUDIO_A2DP_STATE_STOPPING)
        return -1;

    if (a2dp_command(common, A2DP_CTRL_CMD_SUSPEND) < 0)
        return -1;

    if (standby)
        common->state = AUDIO_A2DP_STATE_STANDBY;
    else
        common->state = AUDIO_A2DP_STATE_SUSPENDED;

    /* disconnect audio path */
    skt_disconnect(common->audio_fd);

    common->audio_fd = AUDIO_SKT_DISCONNECTED;

    return 0;
}


/*****************************************************************************
**
**  audio output callbacks
**
*****************************************************************************/

static ssize_t out_write(struct audio_stream_out *stream, const void* buffer,
                         size_t bytes)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    int sent;

    DEBUG("write %zu bytes (fd %d)", bytes, out->common.audio_fd);

    pthread_mutex_lock(&out->common.lock);

    if (out->common.state == AUDIO_A2DP_STATE_SUSPENDED)
    {
        DEBUG("stream suspended");
        pthread_mutex_unlock(&out->common.lock);
        return -1;
    }

    /* only allow autostarting if we are in stopped or standby */
    if ((out->common.state == AUDIO_A2DP_STATE_STOPPED) ||
        (out->common.state == AUDIO_A2DP_STATE_STANDBY))
    {
        if (start_audio_datapath(&out->common) < 0)
        {
            /* emulate time this write represents to avoid very fast write
               failures during transition periods or remote suspend */

            int us_delay = calc_audiotime(out->common.cfg, bytes);

            DEBUG("emulate a2dp write delay (%d us)", us_delay);

            usleep(us_delay);
            pthread_mutex_unlock(&out->common.lock);
            return -1;
        }
    }
    else if (out->common.state != AUDIO_A2DP_STATE_STARTED)
    {
        ERROR("stream not in stopped or standby");
        pthread_mutex_unlock(&out->common.lock);
        return -1;
    }

    pthread_mutex_unlock(&out->common.lock);
    sent = skt_write(out->common.audio_fd, buffer,  bytes);

    if (sent == -1) {
        skt_disconnect(out->common.audio_fd);
        out->common.audio_fd = AUDIO_SKT_DISCONNECTED;
        if (out->common.state != AUDIO_A2DP_STATE_SUSPENDED)
            out->common.state = AUDIO_A2DP_STATE_STOPPED;
        else
            ERROR("write failed : stream suspended, avoid resetting state");
    } else {
        const size_t frames = bytes / audio_stream_out_frame_size(stream);
        out->frames_rendered += frames;
        out->frames_presented += frames;
    }

    DEBUG("wrote %d bytes out of %zu bytes", sent, bytes);
    return sent;
}


static uint32_t out_get_sample_rate(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("rate %" PRIu32,out->common.cfg.rate);

    return out->common.cfg.rate;
}

static int out_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("out_set_sample_rate : %" PRIu32, rate);

    if (rate != AUDIO_STREAM_DEFAULT_RATE)
    {
        ERROR("only rate %d supported", AUDIO_STREAM_DEFAULT_RATE);
        return -1;
    }

    out->common.cfg.rate = rate;

    return 0;
}

static size_t out_get_buffer_size(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("buffer_size : %zu", out->common.buffer_sz);

    return out->common.buffer_sz;
}

static uint32_t out_get_channels(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    DEBUG("channels 0x%" PRIx32, out->common.cfg.channel_flags);

    return out->common.cfg.channel_flags;
}

static audio_format_t out_get_format(const struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    DEBUG("format 0x%x", out->common.cfg.format);
    return out->common.cfg.format;
}

static int out_set_format(struct audio_stream *stream, audio_format_t format)
{
    UNUSED(format);
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    DEBUG("setting format not yet supported (0x%x)", format);
    return -ENOSYS;
}

static int out_standby(struct audio_stream *stream)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    int retVal = 0;

    FNLOG();

    pthread_mutex_lock(&out->common.lock);
    // Do nothing in SUSPENDED state.
    if (out->common.state != AUDIO_A2DP_STATE_SUSPENDED)
        retVal = suspend_audio_datapath(&out->common, true);
    out->frames_rendered = 0; // rendered is reset, presented is not
    pthread_mutex_unlock (&out->common.lock);

    return retVal;
}

static int out_dump(const struct audio_stream *stream, int fd)
{
    UNUSED(fd);
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    FNLOG();
    return 0;
}

static int out_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;
    struct str_parms *parms;
    char keyval[16];
    int retval;
    int status = 0;

    INFO("state %d", out->common.state);

    parms = str_parms_create_str(kvpairs);

    /* dump params */
    str_parms_dump(parms);

    retval = str_parms_get_str(parms, "closing", keyval, sizeof(keyval));

    if (retval >= 0)
    {
        if (strcmp(keyval, "true") == 0)
        {
            DEBUG("stream closing, disallow any writes");
            pthread_mutex_lock(&out->common.lock);
            out->common.state = AUDIO_A2DP_STATE_STOPPING;
            pthread_mutex_unlock(&out->common.lock);
        }
    }

    retval = str_parms_get_str(parms, "A2dpSuspended", keyval, sizeof(keyval));

    if (retval >= 0)
    {
        pthread_mutex_lock(&out->common.lock);
        if (strcmp(keyval, "true") == 0)
        {
            if (out->common.state == AUDIO_A2DP_STATE_STARTED)
                status = suspend_audio_datapath(&out->common, false);
        }
        else
        {
            /* Do not start the streaming automatically. If the phone was streaming
             * prior to being suspended, the next out_write shall trigger the
             * AVDTP start procedure */
            if (out->common.state == AUDIO_A2DP_STATE_SUSPENDED)
                out->common.state = AUDIO_A2DP_STATE_STANDBY;
            /* Irrespective of the state, return 0 */
        }
        pthread_mutex_unlock(&out->common.lock);
    }

    str_parms_destroy(parms);

    return status;
}

static char * out_get_parameters(const struct audio_stream *stream, const char *keys)
{
    UNUSED(keys);
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    FNLOG();

    /* add populating param here */

    return strdup("");
}

static uint32_t out_get_latency(const struct audio_stream_out *stream)
{
    int latency_us;

    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    FNLOG();

    latency_us = ((out->common.buffer_sz * 1000 ) /
                    audio_stream_out_frame_size(&out->stream) /
                    out->common.cfg.rate) * 1000;


    return (latency_us / 1000) + 200;
}

static int out_set_volume(struct audio_stream_out *stream, float left,
                          float right)
{
    UNUSED(stream);
    UNUSED(left);
    UNUSED(right);

    FNLOG();

    /* volume controlled in audioflinger mixer (digital) */

    return -ENOSYS;
}

static int out_get_presentation_position(const struct audio_stream_out *stream,
                                         uint64_t *frames, struct timespec *timestamp)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    FNLOG();
    if (stream == NULL || frames == NULL || timestamp == NULL)
        return -EINVAL;

    int ret = -EWOULDBLOCK;
    pthread_mutex_lock(&out->common.lock);
    uint64_t latency_frames = (uint64_t)out_get_latency(stream) * out->common.cfg.rate / 1000;
    if (out->frames_presented >= latency_frames) {
        *frames = out->frames_presented - latency_frames;
        clock_gettime(CLOCK_MONOTONIC, timestamp); // could also be associated with out_write().
        ret = 0;
    }
    pthread_mutex_unlock(&out->common.lock);
    return ret;
}

static int out_get_render_position(const struct audio_stream_out *stream,
                                   uint32_t *dsp_frames)
{
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    FNLOG();
    if (stream == NULL || dsp_frames == NULL)
        return -EINVAL;

    pthread_mutex_lock(&out->common.lock);
    uint64_t latency_frames = (uint64_t)out_get_latency(stream) * out->common.cfg.rate / 1000;
    if (out->frames_rendered >= latency_frames) {
        *dsp_frames = (uint32_t)(out->frames_rendered - latency_frames);
    } else {
        *dsp_frames = 0;
    }
    pthread_mutex_unlock(&out->common.lock);
    return 0;
}

static int out_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    UNUSED(stream);
    UNUSED(effect);

    FNLOG();
    return 0;
}

static int out_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    UNUSED(stream);
    UNUSED(effect);

    FNLOG();
    return 0;
}

/*
 * AUDIO INPUT STREAM
 */

static uint32_t in_get_sample_rate(const struct audio_stream *stream)
{
    struct a2dp_stream_in *in = (struct a2dp_stream_in *)stream;

    FNLOG();
    return in->common.cfg.rate;
}

static int in_set_sample_rate(struct audio_stream *stream, uint32_t rate)
{
    struct a2dp_stream_in *in = (struct a2dp_stream_in *)stream;

    FNLOG();

    if (in->common.cfg.rate > 0 && in->common.cfg.rate == rate)
        return 0;
    else
        return -1;
}

static size_t in_get_buffer_size(const struct audio_stream *stream)
{
    UNUSED(stream);

    FNLOG();
    return 320;
}

static uint32_t in_get_channels(const struct audio_stream *stream)
{
    struct a2dp_stream_in *in = (struct a2dp_stream_in *)stream;

    FNLOG();
    return in->common.cfg.channel_flags;
}

static audio_format_t in_get_format(const struct audio_stream *stream)
{
    UNUSED(stream);

    FNLOG();
    return AUDIO_FORMAT_PCM_16_BIT;
}

static int in_set_format(struct audio_stream *stream, audio_format_t format)
{
    UNUSED(stream);
    UNUSED(format);

    FNLOG();
    if (format == AUDIO_FORMAT_PCM_16_BIT)
        return 0;
    else
        return -1;
}

static int in_standby(struct audio_stream *stream)
{
    UNUSED(stream);

    FNLOG();
    return 0;
}

static int in_dump(const struct audio_stream *stream, int fd)
{
    UNUSED(stream);
    UNUSED(fd);

    FNLOG();
    return 0;
}

static int in_set_parameters(struct audio_stream *stream, const char *kvpairs)
{
    UNUSED(stream);
    UNUSED(kvpairs);

    FNLOG();
    return 0;
}

static char * in_get_parameters(const struct audio_stream *stream,
                                const char *keys)
{
    UNUSED(stream);
    UNUSED(keys);

    FNLOG();
    return strdup("");
}

static int in_set_gain(struct audio_stream_in *stream, float gain)
{
    UNUSED(stream);
    UNUSED(gain);

    FNLOG();
    return 0;
}

static ssize_t in_read(struct audio_stream_in *stream, void* buffer,
                       size_t bytes)
{
    struct a2dp_stream_in *in = (struct a2dp_stream_in *)stream;
    int read;

    DEBUG("read %zu bytes, state: %d", bytes, in->common.state);

    if (in->common.state == AUDIO_A2DP_STATE_SUSPENDED)
    {
        DEBUG("stream suspended");
        return -1;
    }

    /* only allow autostarting if we are in stopped or standby */
    if ((in->common.state == AUDIO_A2DP_STATE_STOPPED) ||
        (in->common.state == AUDIO_A2DP_STATE_STANDBY))
    {
        pthread_mutex_lock(&in->common.lock);

        if (start_audio_datapath(&in->common) < 0)
        {
            /* emulate time this write represents to avoid very fast write
               failures during transition periods or remote suspend */

            int us_delay = calc_audiotime(in->common.cfg, bytes);

            DEBUG("emulate a2dp read delay (%d us)", us_delay);

            usleep(us_delay);
            pthread_mutex_unlock(&in->common.lock);
            return -1;
        }

        pthread_mutex_unlock(&in->common.lock);
    }
    else if (in->common.state != AUDIO_A2DP_STATE_STARTED)
    {
        ERROR("stream not in stopped or standby");
        return -1;
    }

    read = skt_read(in->common.audio_fd, buffer, bytes);

    if (read == -1)
    {
        skt_disconnect(in->common.audio_fd);
        in->common.audio_fd = AUDIO_SKT_DISCONNECTED;
        in->common.state = AUDIO_A2DP_STATE_STOPPED;
    } else if (read == 0) {
        DEBUG("read time out - return zeros");
        memset(buffer, 0, bytes);
        read = bytes;
    }

    DEBUG("read %d bytes out of %zu bytes", read, bytes);
    return read;
}

static uint32_t in_get_input_frames_lost(struct audio_stream_in *stream)
{
    UNUSED(stream);

    FNLOG();
    return 0;
}

static int in_add_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    UNUSED(stream);
    UNUSED(effect);

    FNLOG();
    return 0;
}

static int in_remove_audio_effect(const struct audio_stream *stream, effect_handle_t effect)
{
    UNUSED(stream);
    UNUSED(effect);

    FNLOG();

    return 0;
}

static int adev_open_output_stream(struct audio_hw_device *dev,
                                   audio_io_handle_t handle,
                                   audio_devices_t devices,
                                   audio_output_flags_t flags,
                                   struct audio_config *config,
                                   struct audio_stream_out **stream_out,
                                   const char *address)

{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_out *out;
    int ret = 0;
    int i;
    UNUSED(address);
    UNUSED(handle);
    UNUSED(devices);
    UNUSED(flags);

    INFO("opening output");

    out = (struct a2dp_stream_out *)calloc(1, sizeof(struct a2dp_stream_out));

    if (!out)
        return -ENOMEM;

    out->stream.common.get_sample_rate = out_get_sample_rate;
    out->stream.common.set_sample_rate = out_set_sample_rate;
    out->stream.common.get_buffer_size = out_get_buffer_size;
    out->stream.common.get_channels = out_get_channels;
    out->stream.common.get_format = out_get_format;
    out->stream.common.set_format = out_set_format;
    out->stream.common.standby = out_standby;
    out->stream.common.dump = out_dump;
    out->stream.common.set_parameters = out_set_parameters;
    out->stream.common.get_parameters = out_get_parameters;
    out->stream.common.add_audio_effect = out_add_audio_effect;
    out->stream.common.remove_audio_effect = out_remove_audio_effect;
    out->stream.get_latency = out_get_latency;
    out->stream.set_volume = out_set_volume;
    out->stream.write = out_write;
    out->stream.get_render_position = out_get_render_position;
    out->stream.get_presentation_position = out_get_presentation_position;


    /* initialize a2dp specifics */
    a2dp_stream_common_init(&out->common);

    out->common.cfg.channel_flags = AUDIO_STREAM_DEFAULT_CHANNEL_FLAG;
    out->common.cfg.format = AUDIO_STREAM_DEFAULT_FORMAT;
    out->common.cfg.rate = AUDIO_STREAM_DEFAULT_RATE;

   /* set output config values */
   if (config)
   {
      config->format = out_get_format((const struct audio_stream *)&out->stream);
      config->sample_rate = out_get_sample_rate((const struct audio_stream *)&out->stream);
      config->channel_mask = out_get_channels((const struct audio_stream *)&out->stream);
   }
    *stream_out = &out->stream;
    a2dp_dev->output = out;

    a2dp_open_ctrl_path(&out->common);
    if (out->common.ctrl_fd == AUDIO_SKT_DISCONNECTED)
    {
        ERROR("ctrl socket failed to connect (%s)", strerror(errno));
        ret = -1;
        goto err_open;
    }

    DEBUG("success");
    /* Delay to ensure Headset is in proper state when START is initiated
       from DUT immediately after the connection due to ongoing music playback. */
    usleep(250000);
    return 0;

err_open:
    free(out);
    *stream_out = NULL;
    a2dp_dev->output = NULL;
    ERROR("failed");
    return ret;
}

static void adev_close_output_stream(struct audio_hw_device *dev,
                                     struct audio_stream_out *stream)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_out *out = (struct a2dp_stream_out *)stream;

    INFO("closing output (state %d)", out->common.state);

    pthread_mutex_lock(&out->common.lock);
    if ((out->common.state == AUDIO_A2DP_STATE_STARTED) || (out->common.state == AUDIO_A2DP_STATE_STOPPING))
        stop_audio_datapath(&out->common);

    skt_disconnect(out->common.ctrl_fd);
    free(stream);
    a2dp_dev->output = NULL;
    pthread_mutex_unlock(&out->common.lock);

    DEBUG("done");
}

static int adev_set_parameters(struct audio_hw_device *dev, const char *kvpairs)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_out *out = a2dp_dev->output;
    int retval = 0;

    if (out == NULL)
        return retval;

    INFO("state %d", out->common.state);

    retval = out->stream.common.set_parameters((struct audio_stream *)out, kvpairs);

    return retval;
}

static char * adev_get_parameters(const struct audio_hw_device *dev,
                                  const char *keys)
{
    struct str_parms *parms;
    UNUSED(dev);

    FNLOG();

    parms = str_parms_create_str(keys);

    str_parms_dump(parms);

    str_parms_destroy(parms);

    return strdup("");
}

static int adev_init_check(const struct audio_hw_device *dev)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device*)dev;

    FNLOG();

    return 0;
}

static int adev_set_voice_volume(struct audio_hw_device *dev, float volume)
{
    UNUSED(dev);
    UNUSED(volume);

    FNLOG();

    return -ENOSYS;
}

static int adev_set_master_volume(struct audio_hw_device *dev, float volume)
{
    UNUSED(dev);
    UNUSED(volume);

    FNLOG();

    return -ENOSYS;
}

static int adev_set_mode(struct audio_hw_device *dev, int mode)
{
    UNUSED(dev);
    UNUSED(mode);

    FNLOG();

    return 0;
}

static int adev_set_mic_mute(struct audio_hw_device *dev, bool state)
{
    UNUSED(dev);
    UNUSED(state);

    FNLOG();

    return -ENOSYS;
}

static int adev_get_mic_mute(const struct audio_hw_device *dev, bool *state)
{
    UNUSED(dev);
    UNUSED(state);

    FNLOG();

    return -ENOSYS;
}

static size_t adev_get_input_buffer_size(const struct audio_hw_device *dev,
                                         const struct audio_config *config)
{
    UNUSED(dev);
    UNUSED(config);

    FNLOG();

    return 320;
}

static int adev_open_input_stream(struct audio_hw_device *dev,
                                  audio_io_handle_t handle,
                                  audio_devices_t devices,
                                  struct audio_config *config,
                                  struct audio_stream_in **stream_in,
                                  audio_input_flags_t flags,
                                  const char *address,
                                  audio_source_t source)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_in *in;
    int ret;
    UNUSED(address);
    UNUSED(config);
    UNUSED(devices);
    UNUSED(flags);
    UNUSED(handle);
    UNUSED(source);

    FNLOG();

    in = (struct a2dp_stream_in *)calloc(1, sizeof(struct a2dp_stream_in));

    if (!in)
        return -ENOMEM;

    in->stream.common.get_sample_rate = in_get_sample_rate;
    in->stream.common.set_sample_rate = in_set_sample_rate;
    in->stream.common.get_buffer_size = in_get_buffer_size;
    in->stream.common.get_channels = in_get_channels;
    in->stream.common.get_format = in_get_format;
    in->stream.common.set_format = in_set_format;
    in->stream.common.standby = in_standby;
    in->stream.common.dump = in_dump;
    in->stream.common.set_parameters = in_set_parameters;
    in->stream.common.get_parameters = in_get_parameters;
    in->stream.common.add_audio_effect = in_add_audio_effect;
    in->stream.common.remove_audio_effect = in_remove_audio_effect;
    in->stream.set_gain = in_set_gain;
    in->stream.read = in_read;
    in->stream.get_input_frames_lost = in_get_input_frames_lost;

    /* initialize a2dp specifics */
    a2dp_stream_common_init(&in->common);

    *stream_in = &in->stream;
    a2dp_dev->input = in;

    a2dp_open_ctrl_path(&in->common);
    if (in->common.ctrl_fd == AUDIO_SKT_DISCONNECTED)
    {
        ERROR("ctrl socket failed to connect (%s)", strerror(errno));
        ret = -1;
        goto err_open;
    }

    if (a2dp_read_audio_config(&in->common) < 0) {
        ERROR("a2dp_read_audio_config failed (%s)", strerror(errno));
        ret = -1;
        goto err_open;
    }

    DEBUG("success");
    return 0;

err_open:
    free(in);
    *stream_in = NULL;
    a2dp_dev->input = NULL;
    ERROR("failed");
    return ret;
}

static void adev_close_input_stream(struct audio_hw_device *dev,
                                   struct audio_stream_in *stream)
{
    struct a2dp_audio_device *a2dp_dev = (struct a2dp_audio_device *)dev;
    struct a2dp_stream_in* in = (struct a2dp_stream_in *)stream;
    a2dp_state_t state = in->common.state;

    INFO("closing input (state %d)", state);

    if ((state == AUDIO_A2DP_STATE_STARTED) || (state == AUDIO_A2DP_STATE_STOPPING))
        stop_audio_datapath(&in->common);

    skt_disconnect(in->common.ctrl_fd);
    free(stream);
    a2dp_dev->input = NULL;

    DEBUG("done");
}

static int adev_dump(const audio_hw_device_t *device, int fd)
{
    UNUSED(device);
    UNUSED(fd);

    FNLOG();

    return 0;
}

static int adev_close(hw_device_t *device)
{
    FNLOG();

    free(device);
    return 0;
}

static int adev_open(const hw_module_t* module, const char* name,
                     hw_device_t** device)
{
    struct a2dp_audio_device *adev;
    int ret;

    INFO(" adev_open in A2dp_hw module");
    FNLOG();

    if (strcmp(name, AUDIO_HARDWARE_INTERFACE) != 0)
    {
        ERROR("interface %s not matching [%s]", name, AUDIO_HARDWARE_INTERFACE);
        return -EINVAL;
    }

    adev = calloc(1, sizeof(struct a2dp_audio_device));

    if (!adev)
        return -ENOMEM;

    adev->device.common.tag = HARDWARE_DEVICE_TAG;
    adev->device.common.version = AUDIO_DEVICE_API_VERSION_2_0;
    adev->device.common.module = (struct hw_module_t *) module;
    adev->device.common.close = adev_close;

    adev->device.init_check = adev_init_check;
    adev->device.set_voice_volume = adev_set_voice_volume;
    adev->device.set_master_volume = adev_set_master_volume;
    adev->device.set_mode = adev_set_mode;
    adev->device.set_mic_mute = adev_set_mic_mute;
    adev->device.get_mic_mute = adev_get_mic_mute;
    adev->device.set_parameters = adev_set_parameters;
    adev->device.get_parameters = adev_get_parameters;
    adev->device.get_input_buffer_size = adev_get_input_buffer_size;
    adev->device.open_output_stream = adev_open_output_stream;
    adev->device.close_output_stream = adev_close_output_stream;
    adev->device.open_input_stream = adev_open_input_stream;
    adev->device.close_input_stream = adev_close_input_stream;
    adev->device.dump = adev_dump;

    adev->output = NULL;


    *device = &adev->device.common;

    return 0;
}

static struct hw_module_methods_t hal_module_methods = {
    .open = adev_open,
};

struct audio_module HAL_MODULE_INFO_SYM = {
    .common = {
        .tag = HARDWARE_MODULE_TAG,
        .version_major = 1,
        .version_minor = 0,
        .id = AUDIO_HARDWARE_MODULE_ID,
        .name = "A2DP Audio HW HAL",
        .author = "The Android Open Source Project",
        .methods = &hal_module_methods,
    },
};
