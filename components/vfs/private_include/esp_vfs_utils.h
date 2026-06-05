/*
 * SPDX-FileCopyrightText: 2025-2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

/*
 * Early-return helpers for VFS syscalls (errno via reent).
 * `r` must be a struct _reent * in scope, as for CHECK_AND_CALL macros.
 */
#define VFS_RETURN_ERR(r, err, ret) \
    do { \
        __errno_r(r) = (err); \
        return (ret); \
    } while (0)

#define VFS_RETURN_ON_NULL_PTR(r, ptr, err, ret) \
    do { \
        if ((ptr) == NULL) { \
            VFS_RETURN_ERR(r, err, ret); \
        } \
    } while (0)

#define VFS_RETURN_ON_NULL_IOBUF(r, buf, size, ret) \
    do { \
        if ((size) != 0 && (buf) == NULL) { \
            VFS_RETURN_ERR(r, EINVAL, ret); \
        } \
    } while (0)

#define VFS_RETURNV_ERR(r, err) \
    do { \
        __errno_r(r) = (err); \
        return; \
    } while (0)

#define VFS_RETURNV_ON_NULL_PTR(r, ptr, err) \
    do { \
        if ((ptr) == NULL) { \
            VFS_RETURNV_ERR(r, err); \
        } \
    } while (0)

/*
 * Using huge multi-line macros is never nice, but in this case
 * the only alternative is to repeat this chunk of code (with different function names)
 * for each syscall being implemented. Given that this define is contained within a single
 * file, this looks like a good tradeoff.
 *
 * First we check if syscall is implemented by VFS (corresponding member is not NULL),
 * then call the right flavor of the method (e.g. open or open_p) depending on
 * ESP_VFS_FLAG_CONTEXT_PTR flag. If ESP_VFS_FLAG_CONTEXT_PTR is set, context is passed
 * in as first argument and _p variant is used for the call.
 * It is enough to check just one of them for NULL, as both variants are part of a union.
 */
#define CHECK_AND_CALL(ret, r, pvfs, func, ...) \
    if (pvfs->vfs->func ## _p == NULL) { \
        VFS_RETURN_ERR(r, ENOSYS, -1); \
    } \
    if (pvfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        ret = (*pvfs->vfs->func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        ret = (*pvfs->vfs->func)(__VA_ARGS__);\
    }

#define CHECK_AND_CALL_SUBCOMPONENT(ret, r, pvfs, component, func, ...) \
    if (pvfs->vfs->component == NULL || pvfs->vfs->component->func ## _p == NULL) { \
        VFS_RETURN_ERR(r, ENOSYS, -1); \
    } \
    if (pvfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        ret = (*pvfs->vfs->component->func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        ret = (*pvfs->vfs->component->func)(__VA_ARGS__);\
    }

#define CHECK_AND_CALLV(r, pvfs, func, ...) \
    if (pvfs->vfs->func ## _p == NULL) { \
        VFS_RETURNV_ERR(r, ENOSYS); \
    } \
    if (pvfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        (*pvfs->vfs->func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        (*pvfs->vfs->func)(__VA_ARGS__);\
    }

#define CHECK_AND_CALL_SUBCOMPONENTV(r, pvfs, component, func, ...) \
    if (pvfs->vfs->component == NULL || pvfs->vfs->component->func ## _p == NULL) { \
        VFS_RETURNV_ERR(r, ENOSYS); \
    } \
    if (pvfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        (*pvfs->vfs->component->func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        (*pvfs->vfs->component->func)(__VA_ARGS__);\
    }

#define CHECK_AND_CALLP(ret, r, pvfs, func, ...) \
    if (pvfs->vfs->func ## _p == NULL) { \
        VFS_RETURN_ERR(r, ENOSYS, NULL); \
    } \
    if (pvfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        ret = (*pvfs->vfs->func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        ret = (*pvfs->vfs->func)(__VA_ARGS__);\
    }

#define CHECK_AND_CALL_SUBCOMPONENTP(ret, r, pvfs, component, func, ...) \
    if (pvfs->vfs->component == NULL || pvfs->vfs->component->func ## _p == NULL) { \
        VFS_RETURN_ERR(r, ENOSYS, NULL); \
    } \
    if (pvfs->flags & ESP_VFS_FLAG_CONTEXT_PTR) { \
        ret = (*pvfs->vfs->component->func ## _p)(pvfs->ctx, __VA_ARGS__); \
    } else { \
        ret = (*pvfs->vfs->component->func)(__VA_ARGS__);\
    }

#define CHECK_VFS_READONLY_FLAG(flags) \
    if (flags & ESP_VFS_FLAG_READONLY_FS) { \
        VFS_RETURN_ERR(r, EROFS, -1); \
    }
