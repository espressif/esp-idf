/*
 * stdlib.h
 *
 * Definitions for common types, variables, and functions.
 */

#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <machine/ieeefp.h>
#include "_ansi.h"

#define __need_size_t
#define __need_wchar_t
#define __need_NULL
#include <stddef.h>

#include <sys/reent.h>
#include <sys/cdefs.h>
#include <machine/stdlib.h>
#ifndef __STRICT_ANSI__
#include <alloca.h>
#endif

#ifdef __CYGWIN__
#include <cygwin/stdlib.h>
#endif

_BEGIN_STD_C

typedef struct 
{
  int quot; /* quotient */
  int rem; /* remainder */
} div_t;

typedef struct 
{
  long quot; /* quotient */
  long rem; /* remainder */
} ldiv_t;

#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
typedef struct
{
  long long int quot; /* quotient */
  long long int rem; /* remainder */
} lldiv_t;
#endif

#ifndef __compar_fn_t_defined
#define __compar_fn_t_defined
typedef int (*__compar_fn_t) (const _PTR, const _PTR);
#endif

#ifndef NULL
#define NULL 0
#endif

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#define RAND_MAX __RAND_MAX

int	_EXFUN(__locale_mb_cur_max,(_VOID));

#define MB_CUR_MAX __locale_mb_cur_max()

_VOID	_EXFUN(abort,(_VOID) _ATTRIBUTE ((__noreturn__)));
int	_EXFUN(abs,(int));
int	_EXFUN(atexit,(_VOID (*__func)(_VOID)));
double	_EXFUN(atof,(const char *__nptr));
#ifndef __STRICT_ANSI__
float	_EXFUN(atoff,(const char *__nptr));
#endif
int	_EXFUN(atoi,(const char *__nptr));
int	_EXFUN(_atoi_r,(struct _reent *, const char *__nptr));
long	_EXFUN(atol,(const char *__nptr));
long	_EXFUN(_atol_r,(struct _reent *, const char *__nptr));
_PTR	_EXFUN(bsearch,(const _PTR __key,
		       const _PTR __base,
		       size_t __nmemb,
		       size_t __size,
		       __compar_fn_t _compar));
_PTR	_EXFUN_NOTHROW(calloc,(size_t __nmemb, size_t __size));
div_t	_EXFUN(div,(int __numer, int __denom));
_VOID	_EXFUN(exit,(int __status) _ATTRIBUTE ((__noreturn__)));
_VOID	_EXFUN_NOTHROW(free,(_PTR));
char *  _EXFUN(getenv,(const char *__string));
char *	_EXFUN(_getenv_r,(struct _reent *, const char *__string));
char *	_EXFUN(_findenv,(_CONST char *, int *));
char *	_EXFUN(_findenv_r,(struct _reent *, _CONST char *, int *));
#ifndef __STRICT_ANSI__
extern char *suboptarg;			/* getsubopt(3) external variable */
int	_EXFUN(getsubopt,(char **, char * const *, char **));
#endif
long	_EXFUN(labs,(long));
ldiv_t	_EXFUN(ldiv,(long __numer, long __denom));
_PTR	_EXFUN_NOTHROW(malloc,(size_t __size));
int	_EXFUN(mblen,(const char *, size_t));
int	_EXFUN(_mblen_r,(struct _reent *, const char *, size_t, _mbstate_t *));
int	_EXFUN(mbtowc,(wchar_t *__restrict, const char *__restrict, size_t));
int	_EXFUN(_mbtowc_r,(struct _reent *, wchar_t *__restrict, const char *__restrict, size_t, _mbstate_t *));
int	_EXFUN(wctomb,(char *, wchar_t));
int	_EXFUN(_wctomb_r,(struct _reent *, char *, wchar_t, _mbstate_t *));
size_t	_EXFUN(mbstowcs,(wchar_t *__restrict, const char *__restrict, size_t));
size_t	_EXFUN(_mbstowcs_r,(struct _reent *, wchar_t *__restrict, const char *__restrict, size_t, _mbstate_t *));
size_t	_EXFUN(wcstombs,(char *__restrict, const wchar_t *__restrict, size_t));
size_t	_EXFUN(_wcstombs_r,(struct _reent *, char *__restrict, const wchar_t *__restrict, size_t, _mbstate_t *));
#ifndef __STRICT_ANSI__
#ifndef _REENT_ONLY
char *	_EXFUN(mkdtemp,(char *));
int	_EXFUN(mkostemp,(char *, int));
int	_EXFUN(mkostemps,(char *, int, int));
int	_EXFUN(mkstemp,(char *));
int	_EXFUN(mkstemps,(char *, int));
#if (__GNUC__ < 4) || defined(__XTENSA__)
char *	_EXFUN(mktemp,(char *));
#else
char *	_EXFUN(mktemp,(char *) _ATTRIBUTE ((__warning__ ("the use of `mktemp' is dangerous; use `mkstemp' instead"))));
#endif
#endif
char *	_EXFUN(_mkdtemp_r, (struct _reent *, char *));
int	_EXFUN(_mkostemp_r, (struct _reent *, char *, int));
int	_EXFUN(_mkostemps_r, (struct _reent *, char *, int, int));
int	_EXFUN(_mkstemp_r, (struct _reent *, char *));
int	_EXFUN(_mkstemps_r, (struct _reent *, char *, int));
#if (__GNUC__ < 4) || defined(__XTENSA__)
char *	_EXFUN(_mktemp_r, (struct _reent *, char *)); 
#else
char *	_EXFUN(_mktemp_r, (struct _reent *, char *) _ATTRIBUTE ((__warning__ ("the use of `mktemp' is dangerous; use `mkstemp' instead"))));
#endif
#endif
_VOID	_EXFUN(qsort,(_PTR __base, size_t __nmemb, size_t __size, __compar_fn_t _compar));
int	_EXFUN(rand,(_VOID));
_PTR	_EXFUN_NOTHROW(realloc,(_PTR __r, size_t __size));
#ifndef __STRICT_ANSI__
_PTR	_EXFUN(reallocf,(_PTR __r, size_t __size));
char *	_EXFUN(realpath, (const char *__restrict path, char *__restrict resolved_path));
#endif
_VOID	_EXFUN(srand,(unsigned __seed));
double	_EXFUN(strtod,(const char *__restrict __n, char **__restrict __end_PTR));
double	_EXFUN(_strtod_r,(struct _reent *,const char *__restrict __n, char **__restrict __end_PTR));
#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
float	_EXFUN(strtof,(const char *__restrict __n, char **__restrict __end_PTR));
#endif
#ifndef __STRICT_ANSI__
/* the following strtodf interface is deprecated...use strtof instead */
# ifndef strtodf 
#  define strtodf strtof
# endif
#endif
long	_EXFUN(strtol,(const char *__restrict __n, char **__restrict __end_PTR, int __base));
long	_EXFUN(_strtol_r,(struct _reent *,const char *__restrict __n, char **__restrict __end_PTR, int __base));
unsigned long _EXFUN(strtoul,(const char *__restrict __n, char **__restrict __end_PTR, int __base));
unsigned long _EXFUN(_strtoul_r,(struct _reent *,const char *__restrict __n, char **__restrict __end_PTR, int __base));

int	_EXFUN(system,(const char *__string));

#ifndef __STRICT_ANSI__
long    _EXFUN(a64l,(const char *__input));
char *  _EXFUN(l64a,(long __input));
char *  _EXFUN(_l64a_r,(struct _reent *,long __input));
int	_EXFUN(on_exit,(_VOID (*__func)(int, _PTR),_PTR __arg));
#endif /* ! __STRICT_ANSI__ */
#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
_VOID	_EXFUN(_Exit,(int __status) _ATTRIBUTE ((__noreturn__)));
#endif
#ifndef __STRICT_ANSI__
int	_EXFUN(putenv,(char *__string));
int	_EXFUN(_putenv_r,(struct _reent *, char *__string));
_PTR	_EXFUN(_reallocf_r,(struct _reent *, _PTR, size_t));
int	_EXFUN(setenv,(const char *__string, const char *__value, int __overwrite));
int	_EXFUN(_setenv_r,(struct _reent *, const char *__string, const char *__value, int __overwrite));

char *	_EXFUN(gcvt,(double,int,char *));
char *	_EXFUN(gcvtf,(float,int,char *));
char *	_EXFUN(fcvt,(double,int,int *,int *));
char *	_EXFUN(fcvtf,(float,int,int *,int *));
char *	_EXFUN(ecvt,(double,int,int *,int *));
char *	_EXFUN(ecvtbuf,(double, int, int*, int*, char *));
char *	_EXFUN(fcvtbuf,(double, int, int*, int*, char *));
char *	_EXFUN(ecvtf,(float,int,int *,int *));
char *	_EXFUN(dtoa,(double, int, int, int *, int*, char**));
#endif
char *	_EXFUN(__itoa,(int, char *, int));
char *	_EXFUN(__utoa,(unsigned, char *, int));
#ifndef __STRICT_ANSI__
char *	_EXFUN(itoa,(int, char *, int));
char *	_EXFUN(utoa,(unsigned, char *, int));
int	_EXFUN(rand_r,(unsigned *__seed));

double _EXFUN(drand48,(_VOID));
double _EXFUN(_drand48_r,(struct _reent *));
double _EXFUN(erand48,(unsigned short [3]));
double _EXFUN(_erand48_r,(struct _reent *, unsigned short [3]));
long   _EXFUN(jrand48,(unsigned short [3]));
long   _EXFUN(_jrand48_r,(struct _reent *, unsigned short [3]));
_VOID  _EXFUN(lcong48,(unsigned short [7]));
_VOID  _EXFUN(_lcong48_r,(struct _reent *, unsigned short [7]));
long   _EXFUN(lrand48,(_VOID));
long   _EXFUN(_lrand48_r,(struct _reent *));
long   _EXFUN(mrand48,(_VOID));
long   _EXFUN(_mrand48_r,(struct _reent *));
long   _EXFUN(nrand48,(unsigned short [3]));
long   _EXFUN(_nrand48_r,(struct _reent *, unsigned short [3]));
unsigned short *
       _EXFUN(seed48,(unsigned short [3]));
unsigned short *
       _EXFUN(_seed48_r,(struct _reent *, unsigned short [3]));
_VOID  _EXFUN(srand48,(long));
_VOID  _EXFUN(_srand48_r,(struct _reent *, long));
#endif /* ! __STRICT_ANSI__ */
#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
long long _EXFUN(atoll,(const char *__nptr));
#endif
#ifndef __STRICT_ANSI__
long long _EXFUN(_atoll_r,(struct _reent *, const char *__nptr));
#endif /* ! __STRICT_ANSI__ */
#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
long long _EXFUN(llabs,(long long));
lldiv_t	_EXFUN(lldiv,(long long __numer, long long __denom));
long long _EXFUN(strtoll,(const char *__restrict __n, char **__restrict __end_PTR, int __base));
#endif
#ifndef __STRICT_ANSI__
long long _EXFUN(_strtoll_r,(struct _reent *, const char *__restrict __n, char **__restrict __end_PTR, int __base));
#endif /* ! __STRICT_ANSI__ */
#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
unsigned long long _EXFUN(strtoull,(const char *__restrict __n, char **__restrict __end_PTR, int __base));
#endif
#ifndef __STRICT_ANSI__
unsigned long long _EXFUN(_strtoull_r,(struct _reent *, const char *__restrict __n, char **__restrict __end_PTR, int __base));

#ifndef __CYGWIN__
_VOID	_EXFUN(cfree,(_PTR));
int	_EXFUN(unsetenv,(const char *__string));
int	_EXFUN(_unsetenv_r,(struct _reent *, const char *__string));
#endif

#ifdef __rtems__
int _EXFUN(posix_memalign,(void **, size_t, size_t));
#endif

#endif /* ! __STRICT_ANSI__ */

char *	_EXFUN(_dtoa_r,(struct _reent *, double, int, int, int *, int*, char**));
#ifndef __CYGWIN__
_PTR	_EXFUN_NOTHROW(_malloc_r,(struct _reent *, size_t));
_PTR	_EXFUN_NOTHROW(_calloc_r,(struct _reent *, size_t, size_t));
_VOID	_EXFUN_NOTHROW(_free_r,(struct _reent *, _PTR));
_PTR	_EXFUN_NOTHROW(_realloc_r,(struct _reent *, _PTR, size_t));
_VOID	_EXFUN(_mstats_r,(struct _reent *, char *));
#endif
int	_EXFUN(_system_r,(struct _reent *, const char *));

_VOID	_EXFUN(__eprintf,(const char *, const char *, unsigned int, const char *));

/* There are two common qsort_r variants.  If you request
   _BSD_SOURCE, you get the BSD version; otherwise you get the GNU
   version.  We want that #undef qsort_r will still let you
   invoke the underlying function, but that requires gcc support. */
#ifdef _BSD_SOURCE
# ifdef __GNUC__
_VOID	_EXFUN(qsort_r,(_PTR __base, size_t __nmemb, size_t __size, _PTR __thunk, int (*_compar)(_PTR, const _PTR, const _PTR)))
             __asm__ (__ASMNAME ("__bsd_qsort_r"));
# else
_VOID	_EXFUN(__bsd_qsort_r,(_PTR __base, size_t __nmemb, size_t __size, _PTR __thunk, int (*_compar)(_PTR, const _PTR, const _PTR)));
#  define qsort_r __bsd_qsort_r
# endif
#elif __GNU_VISIBLE
_VOID	_EXFUN(qsort_r,(_PTR __base, size_t __nmemb, size_t __size, int (*_compar)(const _PTR, const _PTR, _PTR), _PTR __thunk));
#endif

/* On platforms where long double equals double.  */
#ifdef _HAVE_LONG_DOUBLE
#if !defined(__STRICT_ANSI__) || \
  (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L) || \
  (defined(__cplusplus) && __cplusplus >= 201103L)
extern long double strtold (const char *__restrict, char **__restrict);
#endif
#endif /* _HAVE_LONG_DOUBLE */

_END_STD_C

#endif /* _STDLIB_H_ */
