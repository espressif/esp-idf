/* Error numbers for Xtensa ISS semihosting.  */

/* Copyright (c) 2003 by Tensilica Inc.  ALL RIGHTS RESERVED.
   These coded instructions, statements, and computer programs are the
   copyrighted works and confidential proprietary information of Tensilica Inc.
   They may not be modified, copied, reproduced, distributed, or disclosed to
   third parties in any manner, medium, or form, in whole or in part, without
   the prior written consent of Tensilica Inc.  */

#ifndef _SIMCALL_ERRNO_H
#define _SIMCALL_ERRNO_H

/* Define the error numbers (using the default newlib values) with prefixes
   so they can be used in ISS without conflicting with the host values.  */

#define	_SIMC_EPERM	1
#define	_SIMC_ENOENT	2
#define	_SIMC_ESRCH	3
#define	_SIMC_EINTR	4
#define	_SIMC_EIO	5
#define	_SIMC_ENXIO	6
#define	_SIMC_E2BIG	7
#define	_SIMC_ENOEXEC	8
#define	_SIMC_EBADF	9
#define	_SIMC_ECHILD	10
#define	_SIMC_EAGAIN	11
#define	_SIMC_ENOMEM	12
#define	_SIMC_EACCES	13
#define	_SIMC_EFAULT	14
#define	_SIMC_ENOTBLK	15
#define	_SIMC_EBUSY	16
#define	_SIMC_EEXIST	17
#define	_SIMC_EXDEV	18
#define	_SIMC_ENODEV	19
#define	_SIMC_ENOTDIR	20
#define	_SIMC_EISDIR	21
#define	_SIMC_EINVAL	22
#define	_SIMC_ENFILE	23
#define	_SIMC_EMFILE	24
#define	_SIMC_ENOTTY	25
#define	_SIMC_ETXTBSY	26
#define	_SIMC_EFBIG	27
#define	_SIMC_ENOSPC	28
#define	_SIMC_ESPIPE	29
#define	_SIMC_EROFS	30
#define	_SIMC_EMLINK	31
#define	_SIMC_EPIPE	32
#define	_SIMC_EDOM	33
#define	_SIMC_ERANGE	34
#define	_SIMC_ENOMSG	35
#define	_SIMC_EIDRM	36
#define	_SIMC_ECHRNG	37
#define	_SIMC_EL2NSYNC	38
#define	_SIMC_EL3HLT	39
#define	_SIMC_EL3RST	40
#define	_SIMC_ELNRNG	41
#define	_SIMC_EUNATCH	42
#define	_SIMC_ENOCSI	43
#define	_SIMC_EL2HLT	44
#define	_SIMC_EDEADLK	45
#define	_SIMC_ENOLCK	46
#define _SIMC_EBADE	50
#define _SIMC_EBADR	51
#define _SIMC_EXFULL	52
#define _SIMC_ENOANO	53
#define _SIMC_EBADRQC	54
#define _SIMC_EBADSLT	55
#define _SIMC_EDEADLOCK	56
#define _SIMC_EBFONT	57
#define _SIMC_ENOSTR	60
#define _SIMC_ENODATA	61
#define _SIMC_ETIME	62
#define _SIMC_ENOSR	63
#define _SIMC_ENONET	64
#define _SIMC_ENOPKG	65
#define _SIMC_EREMOTE	66
#define _SIMC_ENOLINK	67
#define _SIMC_EADV	68
#define _SIMC_ESRMNT	69
#define	_SIMC_ECOMM	70
#define _SIMC_EPROTO	71
#define	_SIMC_EMULTIHOP	74
#define	_SIMC_ELBIN	75
#define	_SIMC_EDOTDOT	76
#define _SIMC_EBADMSG	77
#define _SIMC_EFTYPE	79
#define _SIMC_ENOTUNIQ	80
#define _SIMC_EBADFD	81
#define _SIMC_EREMCHG	82
#define _SIMC_ELIBACC	83
#define _SIMC_ELIBBAD	84
#define _SIMC_ELIBSCN	85
#define _SIMC_ELIBMAX	86
#define _SIMC_ELIBEXEC	87
#define _SIMC_ENOSYS	88
#define	_SIMC_ENMFILE	89
#define _SIMC_ENOTEMPTY 90
#define _SIMC_ENAMETOOLONG 91
#define _SIMC_ELOOP 92
#define _SIMC_EOPNOTSUPP 95
#define _SIMC_EPFNOSUPPORT 96
#define _SIMC_ECONNRESET 104
#define _SIMC_ENOBUFS 105
#define _SIMC_EAFNOSUPPORT 106
#define _SIMC_EPROTOTYPE 107
#define _SIMC_ENOTSOCK 108
#define _SIMC_ENOPROTOOPT 109
#define _SIMC_ESHUTDOWN 110
#define _SIMC_ECONNREFUSED 111
#define _SIMC_EADDRINUSE 112
#define _SIMC_ECONNABORTED 113
#define _SIMC_ENETUNREACH 114
#define _SIMC_ENETDOWN 115
#define _SIMC_ETIMEDOUT 116
#define _SIMC_EHOSTDOWN 117
#define _SIMC_EHOSTUNREACH 118
#define _SIMC_EINPROGRESS 119
#define _SIMC_EALREADY 120
#define _SIMC_EDESTADDRREQ 121
#define _SIMC_EMSGSIZE 122
#define _SIMC_EPROTONOSUPPORT 123
#define _SIMC_ESOCKTNOSUPPORT 124
#define _SIMC_EADDRNOTAVAIL 125
#define _SIMC_ENETRESET 126
#define _SIMC_EISCONN 127
#define _SIMC_ENOTCONN 128
#define _SIMC_ETOOMANYREFS 129
#define	_SIMC_EPROCLIM 130
#define _SIMC_EUSERS 131
#define _SIMC_EDQUOT 132
#define _SIMC_ESTALE 133
#define _SIMC_ENOTSUP 134
#define _SIMC_ENOMEDIUM 135
#define	_SIMC_ENOSHARE 136
#define	_SIMC_ECASECLASH 137
#define _SIMC_EILSEQ 138
#define _SIMC_EOVERFLOW 139

#endif /* ! _SIMCALL_ERRNO_H */
