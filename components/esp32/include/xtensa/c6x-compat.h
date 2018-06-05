/*
 * Copyright (c) 2006-2010 Tensilica Inc. ALL RIGHTS RESERVED.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef __C6X_COMPAT__H
#define __C6X_COMPAT__H

/* Unimplemented functions _gmpy, _gmpy4, _xormpy, _lssub, _cmpy, _cmpyr,
   _cmpyr1, _ddotpl2r, _ddotph2r */


typedef long long C6X_COMPAT_LONG40;


#define _memd8(a) (*((double*)(a)))
#define _memd8_const(a) (*((const double*)(a)))

#define _amemd8(a) (*((double*)(a)))
#define _amemd8_const(a) (*((const double*)(a)))

#define _mem8(a) (*((unsigned long long*)(a)))
#define _mem8_const(a) (*((const unsigned long long*)(a)))

#define _mem4(a) (*((unsigned*)(a)))
#define _mem4_const(a) (*((const unsigned*)(a)))
#define _amem4_const(a) (*((const unsigned*)(a)))

/*  NOTE: To emulate a C6X properly you should define global variables
    for your Xtensa with these names.  Some of the emulation routines
    will set these values.  */

extern int _carry;
extern int _overflow;

//   Utility routines


#define TESTBIT(x,n) (((x) >> (n)) & 1)

#define NSA_BITS 32

static inline unsigned int norm_shift_amt_U_and_non_U(int is_signed, int inp) {
int j=0, k=0;
int x=inp;
if (is_signed) {
    /* Invert signed val if negative */
    x= TESTBIT(x,(NSA_BITS-1))? ~x: x;
    x= (x&1)|(x<<1);		/* Shift up to return count-1 */
    if (x ==0) 
      return NSA_BITS-1;
  }
  if (x ==0) 
    return NSA_BITS;
  /* Now count leading zeros */
  for (j=0, k=NSA_BITS-1; k>=0; j++, k--) {
    if (TESTBIT(x,k)) 
      return j;
  }
  return NSA_BITS;
}



static inline long long
orig_L40_set( long long L40_var1) {
   long long L40_var_out;
   
   L40_var_out =  L40_var1 & 0x000000ffffffffffLL;

   if( L40_var1 & 0x8000000000LL)
      L40_var_out = L40_var_out | 0xffffff0000000000LL;

   return( L40_var_out);
}



static inline signed long long
util_saturate_n_no_state(signed long long t, int n)
{
  signed long long maxv, minv;
  maxv = (1LL << (n-1)) - 1;
  minv = (-1LL << (n-1));
  if (t > maxv) {
    t = maxv;
  } else if (t < minv) {
    t = minv;
  }
  return t;
}


static inline signed long long
util_saturate_n_sgn(signed long long t, int n)
{
  signed long long result;
  signed long long maxv, minv;
  maxv = (1LL << (n-1)) - 1;
  minv = (-1LL << (n-1));
  if (t > 0) {
    result = maxv;
    _overflow = 1;
  } else if (t < 0) {
    result = minv;
    _overflow = 1;
  } else {
    result = 0;
  }
  return result;
}




/* well-behaved signed shift right (left on negative) with
   saturation */
static inline signed long long
util_shift_right_saturate_n(signed long long t, int shval, int n)
{
  /* n should be <= 62 */
  long long result;

  signed long long mask;
  int actual_shift = shval;
  long long shft = actual_shift > 0 ? actual_shift : -actual_shift;

  if (t == 0 || actual_shift == 0)
    return t;

  if (actual_shift >= n) {
    return (t < 0) ? -1 : 0;
  }
  if (actual_shift <= -n) {
    return util_saturate_n_sgn(t, n);
  }
  if (actual_shift > 0) {
    return t >> actual_shift;
  }
  /* actual_shift < 0. Check for saturation after shift. */
  mask = (-1LL << (n-shft-1));
  if (t > 0 && ((mask & t) != 0)) {
    return util_saturate_n_sgn(t, n);
  }
  if (t < 0 && ((mask & t) != mask)) {
    return util_saturate_n_sgn(t, n);
  }
  result = t << shft;

  return result;
}


/* Implemented c6x standard C compatibility functions (alphabetical
   order) */


static inline int _abs(int src1) {
	if ((unsigned) src1 == (unsigned) 0x80000000) {
		return 0x7fffffff;
	}
	return abs(src1);
}


static inline int _abs2(int src1) {
	short s1[2],r[2];
	int result;
	*((int*)s1) = src1;
	if ((unsigned short) s1[1] == (unsigned short) 0x8000) r[1] = 0x7fff;
	   else r[1] = abs(s1[1]);
	if ((unsigned short) s1[0] == (unsigned short) 0x8000) r[0] = 0x7fff;
	   else r[0] = abs(s1[0]);
	result = *(int*)r;
	return result;
	}




static inline int _add2(int src1, int src2) {
	short s1[2], s2[2], r[2];
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r[0] = s1[0] + s2[0];
	r[1] = s1[1] + s2[1];
	result = *(int*)r;
	return result;
}

static inline int _add4(int src1, int src2) {
	char c1[4], c2[4], r[4];
	int result;
	*((int*)c1) = src1;
	*((int*)c2) = src2;
	r[0] = c1[0] + c2[0];
	r[1] = c1[1] + c2[1];
	r[2] = c1[2] + c2[2];
	r[3] = c1[3] + c2[3];
	result = *(int*)r;
	return result;
}



static inline long long _addsub(unsigned int src1, unsigned int src2)
{
  
  int res_lo;
  int res_hi;
 
  res_hi = src1+src2;
  res_lo = src1-src2;
  return (((unsigned long long) res_hi) << 32) | ((unsigned int) res_lo) ;
}

  
static inline long long _addsub2(unsigned int src1, unsigned int src2)
{
  short s1[2], s2[2], ra[2], rs[2];
  int res_lo;
  int res_hi;

  *((int*)s1) = src1;
  *((int*)s2) = src2;
  ra[0] = s1[0] + s2[0];
  ra[1] = s1[1] + s2[1];
  rs[0] = s1[0] - s2[0];
  rs[1] = s1[1] - s2[1];
  
  res_hi = *(int*)ra;
  res_lo = *(int*)rs;
  return (((unsigned long long) res_hi) << 32) | ((unsigned int) res_lo) ;
}


static inline int _avg2(int src1, int src2) {
  int low = (((int)1 +  (short) src1 + (short) src2) >> 1) & 0XFFFF;
  int high1 = src1 >> 16;
  int high2 = src2 >> 16;
  int high = ((high1 + high2 + 1) >> 1)<< 16;
  return high | low;
}



static inline unsigned int _avgu4(unsigned int src1, unsigned int src2) {
unsigned int res0 = ((src1 & 0xFF) + (src2 & 0xFF) + 1) >> 1;
  unsigned int res1 = (((src1 & 0xFF00) >> 8) + ((src2 & 0xFF00) >> 8) + 1) >> 1;
  unsigned int res2 = (((src1 & 0xFF0000) >> 16) + ((src2 & 0xFF0000) >> 16) + 1) >> 1;
  unsigned int res3 = (((src1 & 0xFF000000) >> 24) + ((src2 & 0xFF000000) >> 24) + 1) >> 1;
  return (res3 << 24) | (res2 << 16) | (res1 << 8) | res0;
}


static inline int TEN_popc (unsigned char b)
{
  int i, result = 0;
  for (i = 0; i <  8; i++){
    if (b & 0x1)
      result++;
    b >>= 1;
  }
  return result;
}

static inline unsigned int _bitc4(unsigned int src1)
{
  unsigned int res0 = TEN_popc(src1 & 0xFF);
  unsigned int res1 = TEN_popc((src1 & 0xFF00) >> 8);
  unsigned int res2 = TEN_popc((src1 & 0xFF0000) >> 16);
  unsigned int res3 = TEN_popc((src1 & 0xFF000000) >> 24);
  return (res3 << 24) | (res2 << 16) | (res1 << 8) | res0;
}

static inline unsigned int _bitr(unsigned int src) {
	int i;
	unsigned r = 0;
	for (i = 0; i< 32; ++i) {
		r = r | (((src >> i) & 1)<<(31-i));
	}
	return r;
}


static inline unsigned int _clr(unsigned int src2,  int csta,  int cstb)
{
  csta &= 0x1f;
  cstb &= 0x1f;
  if (csta > cstb)
    return src2;
  else {
    unsigned int mask =  (((1 <<  (cstb -  csta)) <<  1) -  1) <<  csta;
    return src2 & (~mask);
  }
}

static inline unsigned int _clrr(unsigned int src2, int src1)
{
  unsigned int csta = (src1 >> 5) & 0x1f;
  unsigned int cstb = src1 & 0x1f;
  if (csta > cstb)
    return src2;
  else {
    unsigned int mask =  (((1 <<  (cstb -  csta)) <<  1) -  1) <<  csta;
    return src2 & (~mask);
  }
}




static inline int _cmpeq2(int src1, int src2) {
	short s1[2], s2[2];
	int r0, r1;
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r0 = s1[0] == s2[0] ? 1 : 0;
	r1 = s1[1] == s2[1] ? 1 : 0;
	result = (r1 << 1) | r0;
	return result;
}

static inline int _cmpeq4(int src1, int src2) {
	char s1[4], s2[4];
	int r0, r1, r2, r3;
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r0 = s1[0] == s2[0] ? 1 : 0;
	r1 = s1[1] == s2[1] ? 1 : 0;
	r2 = s1[2] == s2[2] ? 1 : 0;
	r3 = s1[3] == s2[3] ? 1 : 0;
	result = (r3 << 3) | (r2 << 2) | (r1 << 1) | r0;
	return result;
}


static inline int _cmpgt2(int src1, int src2) {
	short s1[2], s2[2];
	int r1, r0;
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r0 = s1[0] > s2[0] ? 1 : 0;
	r1 = s1[1] > s2[1] ? 1 : 0;
	result = (r1<<1) | r0;
	return result;
}


static inline unsigned int _cmpgtu4(unsigned int src1, unsigned int src2) {
  unsigned int s1_0 = (src1 & 0xFF);
  unsigned int s1_1 = (src1 & 0xFF00) >> 8;
  unsigned int s1_2 = (src1 & 0xFF0000) >> 16;
  unsigned int s1_3 = (src1 & 0xFF000000) >> 24;

  unsigned int s2_0 = (src2 & 0xFF);
  unsigned int s2_1 = (src2 & 0xFF00) >> 8;
  unsigned int s2_2 = (src2 & 0xFF0000) >> 16;
  unsigned int s2_3 = (src2 & 0xFF000000) >> 24;

  unsigned int result = 0;

  if (s1_0 > s2_0)
    result |= 0x1;

  if (s1_1 > s2_1)
    result |= 0x2;

  if (s1_2 > s2_2)
    result |= 0x4;

  if (s1_3 > s2_3)
    result |= 0x8;

  return result;
}




static inline long long _ddotp4(unsigned int src1, unsigned int src2) {
  unsigned int res0, res1;
  short s1_0 = (src1 & 0xffff);
  short s1_1 = (src1 & 0xfff0000) >> 16;

  unsigned short s2_0 = (src2 & 0xff);
  unsigned short s2_1 = (src2 & 0xff00) >> 8;
  unsigned short s2_2 = (src2 & 0xff0000) >> 16;
  unsigned short s2_3 = (src2 & 0xff000000) >> 24;

  res0 = ((int)s1_0) * s2_0 + ((int)s1_1) * s2_1;
  res1 = ((int)s1_0) * s2_2 + ((int)s1_1) * s2_3;

  return (res1 << 16) | res0;
}


static inline long long _ddotph2(long long src1_o_src1_e, unsigned int src2)
{

  unsigned int src1_o = src1_o_src1_e >> 32;  
  unsigned int src1_e = src1_o_src1_e & 0xFFFFFFFF;  
  short ls1_o = src1_o & 0XFFFF;
  short hs1_o = src1_o >> 16;
//  short ls1_e = src1_e & 0XFFFF;
  short hs1_e = src1_e >> 16;
  short ls2 = src2 & 0XFFFF;
  short hs2 = src2 >> 16;

  unsigned long long res_hi = ls2 * ls1_o + hs2 * hs1_o;
  unsigned int res_lo = ls1_o * hs2 + hs1_e * ls2;
  return (res_hi << 32) | res_lo;
}


static inline long long _ddotpl2(long long src1_o_src1_e, unsigned int src2)
{
  unsigned int src1_o = src1_o_src1_e >> 32;  
  unsigned int src1_e = src1_o_src1_e & 0xFFFFFFFF;  
  short ls1_o = src1_o & 0XFFFF;
//  short hs1_o = src1_o >> 16;
  short ls1_e = src1_e & 0XFFFF;
  short hs1_e = src1_e >> 16;
  short ls2 = src2 & 0XFFFF;
  short hs2 = src2 >> 16;

  unsigned long long res_hi = ls2 * hs1_e + hs2 * ls1_o;
  unsigned res_lo = hs1_e * hs2 + ls1_e * ls2;
  return (res_hi << 32) | res_lo;
}


static inline unsigned int _deal(unsigned int src)
{
  int i;
  unsigned short lo = 0, hi = 0;
  for (i = 0; i < 32; i+= 2) {
    lo >>= 1;
    lo |= (src & 0x1) << 15;
    src >>= 1;
    hi >>= 1;
    hi |= (src & 0x1) << 15;
    src >>= 1;
  }
  return (hi << 16) | lo;
}


static inline long long _dmv(unsigned int src1, unsigned int src2)
{
  return (((long long) src1) << 32) | src2;
}


static inline int _dotpn2(int src1, int src2) {
short int s1_h = src1>>16;
	short int s1_l = src1;
	short int s2_h = src2>>16;
	short int s2_l = src2;
	return s1_h * s2_h - s1_l * s2_l;
}


static inline int _dotp2(int src1, int src2) {
	short int s1_h = src1>>16;
	short int s1_l = src1;
	short int s2_h = src2>>16;
	short int s2_l = src2;
	return s1_h * s2_h + s1_l * s2_l;
}



static inline int _dotpnrsu2(int src1, unsigned int src2)
{
  short ls1 = src1 & 0XFFFF;
  unsigned short ls2 = src2 & 0XFFFF;
  short hs1 = src1 >> 16;
  unsigned short hs2 = src2 >> 16;

  int result = (((long long) (int)(hs1 * hs2)) - ((long long) (int)(ls1 * ls2)) +  (1 << 15)) >> 16;
  return result;
}



static inline int _dotprsu2(int src1, unsigned int src2) {
  short ls1 = src1 & 0XFFFF;
  unsigned short ls2 =  (src2 & 0XFFFF);
  short hs1 = src1 >> 16;
  unsigned short hs2 =  (src2 >> 16);

  int result = (((long long) (int) (ls1 * ls2)) + ((long long) (int) (hs1 * hs2)) +  (1LL << 15)) >> 16;
  return result;
}







static inline int _dotpsu4(int src1, unsigned int src2) {
  int result;
  signed char s1_0 = (src1 & 0xff);
  signed char s1_1 = (src1 & 0xff00) >> 8;
  signed char s1_2 = (src1 & 0xff0000) >> 16;
  signed char s1_3 = (src1 & 0xff000000) >> 24;

  unsigned int s2_0 = (src2 & 0xff);
  unsigned int s2_1 = (src2 & 0xff00) >> 8;
  unsigned int s2_2 = (src2 & 0xff0000) >> 16;
  unsigned int s2_3 = (src2 & 0xff000000) >> 24;

  result = s1_0 * s2_0 + s1_1 * s2_1 + s1_2 * s2_2 + s1_3 * s2_3;
  return result;
}


static inline unsigned int _dotpu4(unsigned int src1, unsigned int src2) {
	unsigned char v1_0 = src1 & 0xff;
	unsigned char v1_1 = (src1>>8) & 0xff;
	unsigned char v1_2 = (src1>>16) & 0xff;
	unsigned char v1_3 = (src1>>24) & 0xff;

	unsigned char v2_0 = src2 & 0xff;
	unsigned char v2_1 = (src2>>8) & 0xff;
	unsigned char v2_2 = (src2>>16) & 0xff;
	unsigned char v2_3 = (src2>>24) & 0xff;

	unsigned v = v1_0 * v2_0  + v1_1 * v2_1 + v1_2 * v2_2 + v1_3 * v2_3;
	return v;
}


static inline long long _dpack2(unsigned int src1, unsigned int src2){
unsigned short s1[2], s2[2];
*((int*)s1) = src1;
*((int*)s2) = src2;
return ((unsigned long long) s1[1] << 48) | ((unsigned long long) s2[1] << 32) | ((unsigned long long) s1[0] << 16) | ((unsigned long long) s2[0]);
}


static inline long long _dpackx2(unsigned int src1, unsigned int src2){
unsigned short s1[2], s2[2];
*((int*)s1) = src1;
*((int*)s2) = src2;
return ((unsigned long long) s2[0] << 48) | ((unsigned long long) s1[1] << 32) | ((unsigned long long) s1[0] << 16) | ((unsigned long long) s2[1]);
}

static inline int _ext(int src2, unsigned int csta, unsigned int cstb)
{
  return (src2 << csta) >> cstb;
}

static inline int _extr(int src2, int src1)
{
  unsigned int csta = (src1 >> 5) & 0x1f;
  unsigned int cstb = src1 & 0x1f;
  return (src2 << csta) >> cstb;
}

static inline unsigned int _extu(unsigned int src2, unsigned int csta, unsigned int cstb)
{
  return (src2 << csta) >> cstb;
}

static inline unsigned int _extur(unsigned int src2, int src1)
{
  unsigned int csta = (src1 >> 5) & 0x1f;
  unsigned int cstb = src1 & 0x1f;
  return (src2 << csta) >> cstb;
}


static inline unsigned long long _hi(double src) {
	unsigned long long v;
	*(double*)&v = src;
	return v>>32;
}

static inline unsigned int _hill (long long src)
{
  return (unsigned int) (src >> 32);
}



static inline double _itod(unsigned hi, unsigned lo) {
	double v;
	unsigned long long ll = ((((unsigned long long)(hi))<<32) | (unsigned long long)((unsigned)lo)); 
	*((unsigned long long *)&v) = ll;
	return v;
}


static inline long long _itoll(unsigned int src2, unsigned int src1)
{
  return (((long long) src2) << 32) | src1;
}


static inline C6X_COMPAT_LONG40 _labs(C6X_COMPAT_LONG40 src2)
{
  long long maxv = (1LL << (40 -1)) - 1;
  long long minv = (-1LL << (40 - 1));
  C6X_COMPAT_LONG40 lres =  orig_L40_set(src2);

  lres = lres < 0 ? -lres : lres;
  if (lres > maxv) lres = maxv;
  else if (lres < minv) lres = minv;
  
  return lres;
}


static inline C6X_COMPAT_LONG40 _ldotp2(int src1, int src2) {
return (C6X_COMPAT_LONG40) _dotp2(src1, src2);
}


static inline unsigned int _lmbd(unsigned int src1, unsigned int src2)
{
  return norm_shift_amt_U_and_non_U(0,(((int) (src1 << 31)) >> 31) ^ (~src2));
}


static inline unsigned int _lnorm(C6X_COMPAT_LONG40 src2) {
if (src2 == 0)
    return 39;
  else {
    int hi = (int)(src2 >> 32);
    int lo = (int)src2;
    

    long long temp = (unsigned long long)(unsigned)lo | (unsigned long long)hi << 32;
    temp = orig_L40_set(temp);

    if (temp == 0) return 0;
    int cnt = 0;
    while (((temp >> 39) & 1) == ((temp >> 38) & 1)) {
       temp <<= 1;
       cnt++;
       }
  return cnt;
  }
}


static inline unsigned long long _lo(double src) {
	unsigned long long v;
	*(double*)&v = src;
	return v;
}


static inline unsigned int _loll (long long src)
{
  return (unsigned int) src;
}


static inline C6X_COMPAT_LONG40 _lsadd(int src1, C6X_COMPAT_LONG40 src2)
{
  long long maxv = (1LL << (40 -1)) - 1;
  long long minv = (-1LL << (40 - 1));
  int hi = (int)(src2 >> 32);
  int lo = (int)src2;
  long long src2_int =  (unsigned long long)(unsigned)lo | (unsigned long long)hi << 32;


  long long src2_int2 =  orig_L40_set(src2_int);
  
  long long res = src1 + src2_int2;

  if (res > maxv) { 
	res = maxv;
	_overflow = 1;
	}
  else if (res < minv) {
	res = minv;
	_overflow = 1;
	}

  long long res2 = orig_L40_set(res);

  res2 = (signed char)(res2 >> 32);
  
  C6X_COMPAT_LONG40 lres = (((C6X_COMPAT_LONG40) res2) << 32) | ((unsigned int)res);
  return lres;
}



static inline int _max2 (int src1, int src2) {
	short s1[2], s2[2], r[2];
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r[0] = s1[0] > s2[0] ? s1[0] : s2[0];
	r[1] = s1[1] > s2[1] ? s1[1] : s2[1];
	result = *(int*)r;
	return result;
}






static inline unsigned int _maxu4(unsigned int src1, unsigned int src2) {
  unsigned int res0, res1, res2, res3;
  unsigned int s1_0 = res0 = (src1 & 0xFF);
  unsigned int s1_1 = res1 = (src1 & 0xFF00) >> 8;
  unsigned int s1_2 = res2 = (src1 & 0xFF0000) >> 16;
  unsigned int s1_3 = res3 = (src1 & 0xFF000000) >> 24;

  unsigned int s2_0 = (src2 & 0xFF);
  unsigned int s2_1 = (src2 & 0xFF00) >> 8;
  unsigned int s2_2 = (src2 & 0xFF0000) >> 16;
  unsigned int s2_3 = (src2 & 0xFF000000) >> 24;

//  unsigned int res = 0;

  if (s1_0 < s2_0)
    res0 = s2_0;

  if (s1_1 < s2_1)
    res1 = s2_1;

  if (s1_2 < s2_2)
    res2 = s2_2;

  if (s1_3 < s2_3)
    res3 = s2_3;

  return (res3 << 24) | (res2 << 16) | (res1 << 8) | res0;


}

static inline int _min2(int src1, int src2) {
	short s1[2], s2[2], r[2];
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r[0] = s1[0] < s2[0] ? s1[0] : s2[0];
	r[1] = s1[1] < s2[1] ? s1[1] : s2[1];
	result = *(int*)r;
	return result;
}


static inline unsigned int _minu4(unsigned int src1, unsigned int src2) {
unsigned int res0, res1, res2, res3;
  unsigned int s1_0 = res0 = (src1 & 0xFF);
  unsigned int s1_1 = res1 = (src1 & 0xFF00) >> 8;
  unsigned int s1_2 = res2 = (src1 & 0xFF0000) >> 16;
  unsigned int s1_3 = res3 = (src1 & 0xFF000000) >> 24;

  unsigned int s2_0 = (src2 & 0xFF);
  unsigned int s2_1 = (src2 & 0xFF00) >> 8;
  unsigned int s2_2 = (src2 & 0xFF0000) >> 16;
  unsigned int s2_3 = (src2 & 0xFF000000) >> 24;

//  unsigned int res = 0;

  if (s1_0 > s2_0)
    res0 = s2_0;

  if (s1_1 > s2_1)
    res1 = s2_1;

  if (s1_2 > s2_2)
    res2 = s2_2;

  if (s1_3 > s2_3)
    res3 = s2_3;

  return (res3 << 24) | (res2 << 16) | (res1 << 8) | res0;
}


static inline int _mpy(int src1, int src2) {
return (short) src1 * (short) src2;
}


static inline int _mpyh(int src1, int src2) {
return (short) (src1 >> 16) * (short) (src2 >> 16);
}


static inline long long _mpyhill (int src1,  int src2)
{
  short s1 = src1 >> 16;
  return ((long long) src2) * s1;
}

static inline int _mpyhir(int src1, int src2)
{
  short s1 = src1 >> 16;
  long long result = ((long long) src2) * s1 + (1 << 14);
  result >>= 15;
  return result;
}


static inline int _mpyhl(int src1, int src2) {
return (short) (src1 >> 16) * (short) (src2);
}

static inline unsigned int _mpyhlu(unsigned int src1, unsigned int src2) {
return (unsigned short) (src1 >> 16) * (unsigned short) (src2);
}

static inline int _mpyhslu(int src1, unsigned int src2) {
return (short) (src1 >> 16) * (unsigned short) src2;
}


static inline int _mpyhsu(int src1, unsigned int src2) {
return (short) (src1 >>16) * (unsigned short) (src2 >>16);
}


static inline unsigned int _mpyhu(unsigned int src1, unsigned int src2) {
return (unsigned short) (src1 >>16) * (unsigned short) (src2 >> 16);
}


static inline int _mpyhuls(unsigned int src1, int src2) {
return (unsigned short) (src1 >>16) * (signed short) (src2);
}


static inline int _mpyhus(unsigned int src1, int src2) {
return (unsigned short) (src1 >> 16) * (short) (src2 >>16);
}



static inline long long _mpyidll (int src1, int src2)
{
  return (long long) src1 * src2;
}


static inline int _mpylh(int src1, int src2) {
return (signed short) (src1 & 0xffff) * (signed short) (src2 >> 16);
}

static inline unsigned int _mpylhu(unsigned int src1, unsigned int src2) {
return (unsigned short) src1 * (unsigned short) (src2 >> 16);
}


static inline long long _mpylill (int src1,  int src2)
{
  return ((long long) src2) * ((short)src1);
}



static inline int _mpylir(int src1, int src2)
{
  short s1 = src1;
  long long result = ((long long) src2) * s1 + (1 << 14);
  result >>= 15;
  return result;
}


static inline int _mpylshu(int src1, unsigned int src2) {
return (short) src1 * (unsigned short) (src2 >> 16);
}


static inline int _mpyluhs(unsigned int src1, int src2) {
return (unsigned short) src1 * (short) (src2 >> 16);
}



static inline int _mpysu(int src1, unsigned int src2) {
return (short) src1 * (unsigned short) src2;
}



static inline long long _mpysu4ll (int src1,  unsigned int src2) {
  unsigned short res0, res1, res2, res3;
  signed char s1_0 = (src1 & 0xff);
  signed char s1_1 = (src1 & 0xff00) >> 8;
  signed char s1_2 = (src1 & 0xff0000) >> 16;
  signed char s1_3 = (src1 & 0xff000000) >> 24;

  unsigned short s2_0 = (src2 & 0xff);
  unsigned short s2_1 = (src2 & 0xff00) >> 8;
  unsigned short s2_2 = (src2 & 0xff0000) >> 16;
  unsigned short s2_3 = (src2 & 0xff000000) >> 24;

  res0 = s1_0 * s2_0;
  res1 = s1_1 * s2_1;
  res2 = s1_2 * s2_2;
  res3 = s1_3 * s2_3;

  return (((unsigned long long) res3) << 48)
    | (((unsigned long long) res2) << 32)
    | (((unsigned long long) res1) << 16)
    | res0;
}

static inline unsigned int _mpyu(unsigned int src1, unsigned int src2) {
	unsigned v = (unsigned short)src1 * (unsigned short)src2;
	return v;
}

static inline int _mpyus(unsigned int src1, int src2) {
return (unsigned short) src1 * (short) src2;
}

static inline long long _mpyu4ll (unsigned int src1,  unsigned int src2) {
  unsigned short res0, res1, res2, res3;
  unsigned char s1_0 = (src1 & 0xff);
  unsigned char s1_1 = (src1 & 0xff00) >> 8;
  unsigned char s1_2 = (src1 & 0xff0000) >> 16;
  unsigned char s1_3 = (src1 & 0xff000000) >> 24;

  unsigned short s2_0 = (src2 & 0xff);
  unsigned short s2_1 = (src2 & 0xff00) >> 8;
  unsigned short s2_2 = (src2 & 0xff0000) >> 16;
  unsigned short s2_3 = (src2 & 0xff000000) >> 24;

  res0 = s1_0 * s2_0;
  res1 = s1_1 * s2_1;
  res2 = s1_2 * s2_2;
  res3 = s1_3 * s2_3;

  return (((unsigned long long) res3) << 48)
    | (((unsigned long long) res2) << 32)
    | (((unsigned long long) res1) << 16)
    | res0;
}


static inline long long _mpy2ir(unsigned int src1, unsigned int src2)
{
  if ((src1 == 0x8000) && (src2 == 0x80000000)) {
    _overflow = 1;
    return 0;
  }
  else {
    short ls1 = src1 & 0xffff;
    short hs1 = src1 >> 16;
    unsigned long long hi = (((long long) hs1) * (int) src2 + (1 << 14)) >> 15;
    unsigned long long lo = ((((long long) ls1) * (int) src2 + (1 << 14)) >> 15) & 0xFFFFFFFF;
    return (hi << 32) | lo;
  }
}


static inline long long _mpy2ll (int src1,  int src2) {
  short ls1 = src1 & 0xffff;
  short hs1 = src1 >> 16;
  short ls2 = src2 & 0xffff;
  short hs2 = src2 >> 16;

  unsigned long long hi = hs1 * hs2;
  unsigned long long lo = (ls1 * ls2) & 0xFFFFFFFF;

  return (hi << 32) | lo;
  
}


static inline int _mpy32(int src1, int src2)
{
  return src1 * src2;
}


static inline long long _mpy32ll(int src1, int src2)
{
  return ((long long) src1) * src2;
}

static inline long long _mpy32su(int src1, unsigned int src2)
{
  return ((long long) src1) * ((int) src2);
}

static inline long long _mpy32u(unsigned int src1, unsigned int src2)
{
  return ((long long) ((int) src1)) * ((long long) ((int) src2));
}

static inline long long _mpy32us(unsigned int src1, int src2)
{
  return ((int) src1) * ((long long) src2);
}

static inline int _mvd (int src2)
{
  return src2;
}


static inline unsigned int _norm(int src2)
{
  return norm_shift_amt_U_and_non_U(1,src2);
}


static inline unsigned int _pack2 (unsigned int src1, unsigned int src2) {
	short s1[2], s2[2], r[2];
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r[0] = s2[0];
	r[1] = s1[0];
	result = *(int*)r;
	return result;
}


static inline int _packh2 (unsigned int src1, unsigned int src2) {
	unsigned v0 = src1 & 0xffff0000;
	unsigned v1 = src2 >> 16;
	unsigned v = v0|v1;
	return v;
	
}

static inline unsigned int _packh4 (unsigned int src1, unsigned int src2) {
	unsigned v3 = (src1 >> 24) & 0xff;
	unsigned v2 = (src1 >> 8) & 0xff;
	unsigned v1 = (src2 >> 24) & 0xff;
	unsigned v0 = (src2 >> 8) & 0xff;
	unsigned v = (v3<<24) | (v2<<16) | (v1 << 8) | v0;
	return v;
}

static inline unsigned int _packhl2 (unsigned int src1,  unsigned int src2) {
	unsigned v0 = src1 & 0xffff0000;
	unsigned v1 = src2 & 0x0000ffff;
	unsigned v = v0|v1;
	return v;
}

static inline unsigned int _packlh2 (unsigned int src1,  unsigned int src2) {
	unsigned v0 = src1 << 16;
	unsigned v1 = (src2 >> 16) & 0xffff;
	unsigned v = v0|v1;
	return v;
}




static inline unsigned int _packl4 (unsigned int src1, unsigned int src2) {
	unsigned v3 = (src1 >> 16) & 0xff;
	unsigned v2 = (src1) & 0xff;
	unsigned v1 = (src2 >> 16) & 0xff;
	unsigned v0 = (src2) & 0xff;
	unsigned v = (v3<<24) | (v2<<16) | (v1 << 8) | v0;
	return v;
}




static inline unsigned int _rpack2 (unsigned int src1, unsigned int src2) {
int s1 = (int) src1;
int s2 = (int) src2;
s1 = util_shift_right_saturate_n (s1, -1, 32);
s2 = util_shift_right_saturate_n (s2, -1, 32);
return (unsigned int) (s1 & 0xffff0000) | (unsigned int) ((s2 & 0xffff0000) >>16);
}


static inline unsigned int _rotl (unsigned int src1, unsigned int src2)
{
  src2 &= 0x1f;
  return (src1 << src2) | (src1 >> (32 - src2));
}


static inline int _sadd(int src1, int src2) {
signed long long res;
signed long long maxv, minv;
maxv = (1LL << (32-1)) - 1;
minv = (-1LL << (32-1));
res = (long long) src1 + (long long) src2;
if (res > maxv) {
	res = maxv;
	_overflow = 1;
	}
else if (res < minv ) {
	res = minv;
	_overflow = 1;
	}
return (int) res;
}

static inline long long _saddsub(unsigned int src1, unsigned int src2) {
int radd;
signed long long rsub;

signed long long maxv, minv;
maxv = (1LL << (32-1)) - 1;
minv = (-1LL << (32-1));

radd = (int) src1 + (int) src2;

//   saturate on subtract, not add


rsub = (long long) ((int) src1) - (long long) ((int) src2);
if (rsub > maxv) {
	rsub = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (rsub < minv ) {
	rsub = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}

return  (((unsigned long long) radd) << 32) |  ( rsub & 0x00000000ffffffff ) ;
}



static inline long long _saddsub2(unsigned int src1, unsigned int src2) {
signed int radd[2];
signed int rsub[2];
signed short s1[2], s2[2];

signed int maxv, minv;
maxv = (1L << (16-1)) - 1;
minv = (-1L << (16-1));

*((int*)s1) = src1;
*((int*)s2) = src2;

radd[0] =  (int) s1[0] + (int) s2[0];
radd[1] =  (int) s1[1] + (int) s2[1];

rsub[0] =  (int) s1[0] - (int) s2[0];
rsub[1] =  (int) s1[1] - (int) s2[1];

if (radd[0] > maxv) {
	radd[0] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (radd[0] < minv ) {
	radd[0] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}

if (radd[1] > maxv) {
	radd[1] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (radd[1] < minv ) {
	radd[1] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}


if (rsub[0] > maxv) {
	rsub[0] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (rsub[0] < minv ) {
	rsub[0] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}

if (rsub[1] > maxv) {
	rsub[1] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (rsub[1] < minv ) {
	rsub[1] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}


return  ((((unsigned long long) radd[1]) & 0x000000000000ffff) << 48) | 
        ((((unsigned long long) radd[0]) & 0x000000000000ffff) << 32) | 
        ((((unsigned long long) rsub[1]) & 0x000000000000ffff) << 16) |
        ((((unsigned long long) rsub[0]) & 0x000000000000ffff));
}



static inline  int _sadd2(int src1, int src2) {
signed short s1[2], s2[2];
signed int r[2], maxv, minv;

maxv = (1L << (16-1)) - 1;
minv = (-1L << (16-1));


*((int*)s1) = src1;
*((int*)s2) = src2;

r[0] =  (int) s1[0] + (int) s2[0];
r[1] =  (int) s1[1] + (int) s2[1];

if (r[0] > maxv) {
	r[0] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (r[0] < minv ) {
	r[0] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
if (r[1] > maxv) {
	r[1] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (r[1] < minv ) {
	r[1] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}

return ((r[1] & 0xffff) << 16 ) | (r[0] & 0xffff) ;
}


static inline int _saddus2(unsigned int src1, int src2) {
int res0, res1;
  unsigned int s1_0 = (src1 & 0xffff);
  unsigned int s1_1 = (src1 & 0xffff0000) >> 16;

  short s2_0 = (src2 & 0xffff);
  short s2_1 = (src2 & 0xffff0000) >> 16;

  res0 = s1_0 + s2_0;
  res1 = s1_1 + s2_1;

  if (res0 >= 0x10000)
    res0 = 0xffff;
  else if (res0 < 0)
    res0 = 0;
  
  if (res1 >= 0x10000)
    res1 = 0xffff;
  else if (res1 < 0)
    res1 = 0;
  
  return (res1 << 16) | res0;
}


static inline unsigned int _saddu4(unsigned int src1, unsigned int src2) {
unsigned int res0, res1, res2, res3;
  unsigned int s1_0 = (src1 & 0xff);
  unsigned int s1_1 = (src1 & 0xff00) >> 8;
  unsigned int s1_2 = (src1 & 0xff0000) >> 16;
  unsigned int s1_3 = (src1 & 0xff000000) >> 24;

  unsigned int s2_0 = (src2 & 0xff);
  unsigned int s2_1 = (src2 & 0xff00) >> 8;
  unsigned int s2_2 = (src2 & 0xff0000) >> 16;
  unsigned int s2_3 = (src2 & 0xff000000) >> 24;

  res0 = s1_0 + s2_0;
  res1 = s1_1 + s2_1;
  res2 = s1_2 + s2_2;
  res3 = s1_3 + s2_3;

  if (res0 >= 0x100)
    res0 = 0xff;
  
  if (res1 >= 0x100)
    res1 = 0xff;
  
  if (res2 >= 0x100)
    res2 = 0xff;
  
  if (res3 >= 0x100)
    res3 = 0xff;

  return (res3 << 24) | (res2 << 16) | (res1 << 8) | res0;

}



static inline int _sat(C6X_COMPAT_LONG40 src2)
{
  long long maxv = (1LL << (32-1)) - 1;
  long long minv = (-1LL << (32-1));

  int hi = (int)(src2 >> 32);
  int lo = (int)src2;
  long long temp = (unsigned long long)(unsigned)lo | (unsigned long long)hi << 32;
  temp = orig_L40_set(temp);
  
  if (temp > maxv) {
	temp = maxv;
	_overflow = 1;
	}
  else if (temp < minv) {
	temp = minv;
	_overflow = 1;
	}
  return (int) temp;
}

static inline unsigned int _set(unsigned int src2, unsigned int csta, unsigned int cstb)
{
  csta &= 0x1f;
  cstb &= 0x1f;
  if (csta > cstb)
    return src2;
  else {
    unsigned int mask =  (((1 <<  (cstb -  csta)) <<  1) -  1) <<  csta;
    return src2 | mask;
  }
}

static inline unsigned int _setr(unsigned int src2, int src1)
{
  unsigned int csta = (src1 >> 5) & 0x1f;
  unsigned int cstb = src1 & 0x1f;
  if (csta > cstb)
    return src2;
  else {
    unsigned int mask =  (((1 <<  (cstb -  csta)) <<  1) -  1) <<  csta;
    return src2 | mask;
  }
}


static inline unsigned int _shfl (unsigned int src2)
{
  unsigned short lo = src2;
  unsigned short hi = src2 >> 16;
  unsigned int result = 0;
  int i;
  for (i = 0; i < 32; i+= 2) {
    result >>= 1;
    result |= (lo & 0x1) << 31;
    lo >>= 1;
    result >>= 1;
    result |= (hi & 0x1) << 31;
    hi >>= 1;
  }
  return result;
}

static inline long long _shfl3 (unsigned int src1, unsigned int src2)
{
  unsigned short lo = src2;
  unsigned short hi = src1 >> 16;
  unsigned short mid = src1;
  unsigned long long result = 0;
  int i;
  for (i = 0; i < 32; i+= 2) {
    result >>= 1;
    result |= ((unsigned long long) (lo & 0x1)) << 47;
    lo >>= 1;
    result >>= 1;
    result |= ((unsigned long long) (mid & 0x1)) << 47;
    mid >>= 1;
    result >>= 1;
    result |= ((unsigned long long) (hi & 0x1)) << 47;
    hi >>= 1;
  }
  return result;
}



static inline unsigned int _shlmb (unsigned int src1, unsigned int src2)
{
  return (src2 << 8) | (src1 >> 24);
}

static inline unsigned int _shrmb (unsigned int src1, unsigned int src2)
{
  return (src2 >> 8) | (src1 << 24);
}


static inline unsigned int _shru2 (unsigned int src1, unsigned int src2) {
unsigned short hs1 = src1 >> 16;
  unsigned short ls1 = src1 & 0xFFFF;
  hs1 >>= src2;
  ls1 >>= src2;
  return (hs1 << 16) | ls1;
}


static inline int _shr2 (int src1, unsigned int src2) {
  short s1[2], result[2];
  *((int*)s1) = src1;
  src2 = src2 & 31;
  result[0] = (int)s1[0] >> src2;
  result[1] = (int)s1[1] >> src2;

  return *(int*)result;
}


static inline int _smpy (int src1, int src2) {
unsigned long long result;
result =  (((short) src1 * (short) src2) << 1);

if ((result & 0xffffffff) == 0x80000000){
    result = 0x7fffffff;
    _overflow = 1;
  }
return (int) (result);
}

static inline int _smpyh (int src1, int src2) {
unsigned long long result;
result =  ((short) (src1 >> 16) * (short) (src2 >> 16)) << 1;
if ((result & 0xffffffff) == 0x80000000){
    result = 0x7fffffff;
    _overflow = 1;
  }
return (int) (result);
}

static inline int _smpyhl (int src1, int src2) {
unsigned long long result;
result = ((short) (src1 >> 16) * (short) (src2)) << 1;
if ((result & 0xffffffff) == 0x80000000){
    result = 0x7fffffff;
    _overflow = 1;
  }
return (int) (result);
}

static inline int _smpylh (int src1, int src2) {
unsigned long long result;
result = ((short) (src1) * (short) (src2 >> 16)) << 1;
if ((result & 0xffffffff) == 0x80000000){
    result = 0x7fffffff;
    _overflow = 1;
  }
return (int) (result);
}

static inline long long _smpy2ll (int src1,  int src2) {
  short ls1 = src1 & 0XFFFF;
  short hs1 = src1 >> 16;
  short ls2 = src2 & 0XFFFF;
  short hs2 = src2 >> 16;

  unsigned long long hi = (hs1 * hs2) << 1;
  unsigned long long lo = ((ls1 * ls2) << 1) & 0xFFFFFFFF;
  if ((hi & 0xffffffff) == 0x80000000){
    hi = 0x7fffffff;
    _overflow = 1;
  }

  if ((lo & 0xffffffff) == 0x80000000){
    lo = 0x7fffffff;
    _overflow = 1;
  }

  return (hi << 32) | lo;
}




static inline int _smpy32(int src1, int src2)
{
  long long res = (long long) src1 * src2;
  res <<= 1;
  res >>= 32;
  return res;
}

static inline unsigned char TEN_satu8 (short src)
{
  if (src > 0xff)
    return 0xff;
  else if (src < 0)
    return 0;
  else
    return src;
}

static inline int _spack2 (int src1, int src2) {
short s1 = (short) util_saturate_n_no_state(src1,16);
short s2 = (short) util_saturate_n_no_state(src2,16);
return  ( (unsigned int) s1 << 16) | (((int) s2) & 0xFFFF);
}


static inline unsigned int _spacku4 (int src1, int src2) {
  short lolo = src2;
  short lohi = src2 >> 16;
  short hilo = src1;
  short hihi = src1 >> 16;

  lolo = TEN_satu8(lolo);
  lohi = TEN_satu8(lohi);
  hilo = TEN_satu8(hilo);
  hihi = TEN_satu8(hihi);

  return (((unsigned int) hihi) <<  24) | (((unsigned int) hilo) << 16) | (lohi << 8) | lolo;
}



static inline int _sshl (int src1, unsigned int src2) {
short local2 = (short)(src2 & 0x7FFF);
return (int) util_shift_right_saturate_n(src1, -local2, 32);
}




static inline int _sshvl (int src2, int src1) {
  short s1;
  if (src1 > 31)
    s1 = 31;
  else if (src1 < -31)
    s1 = -31;
  else
    s1 = src1;

  return (int) util_shift_right_saturate_n(src2, -s1, 32);
}





static inline int _sshvr (int src2, int src1) {
short s1;
  if (src1 > 31)
    s1 = 31;
  else if (src1 < -31)
    s1 = -31;
  else
    s1 = src1;
  return (int) util_shift_right_saturate_n(src2, s1, 32);
}




static inline int _ssub(int src1, int src2) {
signed long long res;
signed long long maxv, minv;
maxv = (1LL << (32-1)) - 1;
minv = (-1LL << (32-1));
res = (long long) src1 - (long long) src2;
if (res > maxv) {
	res = maxv;
	_overflow = 1;
	}
else if (res < minv ) {
	res = minv;
	_overflow = 1;
	}
return (int) res;
}

static inline int _ssub2(int src1, int src2) {
signed short s1[2], s2[2];
signed int r[2], maxv, minv;

maxv = (1L << (16-1)) - 1;
minv = (-1L << (16-1));


*((int*)s1) = src1;
*((int*)s2) = src2;

r[0] =  (int) s1[0] - (int) s2[0];
r[1] =  (int) s1[1] - (int) s2[1];

if (r[0] > maxv) {
	r[0] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (r[0] < minv ) {
	r[0] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
if (r[1] > maxv) {
	r[1] = maxv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}
else if (r[1] < minv ) {
	r[1] = minv;
        /* NOTE:   TI c6x does NOT set the overflow register even if results saturate */
	/*  _overflow = 1; */
	}

return ((r[1] & 0xffff) << 16 ) | (r[0] & 0xffff) ;
}


static inline int _subabs4 (int src1, int src2) {
  int res0, res1, res2, res3;
  unsigned int s1_0 = (src1 & 0xff);
  unsigned int s1_1 = (src1 & 0xff00) >> 8;
  unsigned int s1_2 = (src1 & 0xff0000) >> 16;
  unsigned int s1_3 = (src1 & 0xff000000) >> 24;

  unsigned int s2_0 = (src2 & 0xff);
  unsigned int s2_1 = (src2 & 0xff00) >> 8;
  unsigned int s2_2 = (src2 & 0xff0000) >> 16;
  unsigned int s2_3 = (src2 & 0xff000000) >> 24;

  res0 = s1_0 - s2_0;
  res1 = s1_1 - s2_1;
  res2 = s1_2 - s2_2;
  res3 = s1_3 - s2_3;

  if (res0 < 0)
    res0 = -res0;
  
  if (res1 < 0)
    res1 = -res1;
  
  if (res2 < 0)
    res2 = -res2;
  
  if (res3 < 0)
    res3 = -res3;

  return (res3 << 24) | (res2 << 16) | (res1 << 8) | res0;
}


static inline unsigned int _subc (unsigned int src1, unsigned int src2)
{
  if ( src1 >=  src2)
    return ((src1 - src2) <<  1) +  1;
  else
    return src1 << 1;
}



static inline int _sub2(int src1, int src2) {
	short s1[2], s2[2], r[2];
	int result;
	*((int*)s1) = src1;
	*((int*)s2) = src2;
	r[0] = s1[0] - s2[0];
	r[1] = s1[1] - s2[1];
	result = *(int*)r;
	return result;
}


static inline int _sub4(int src1, int src2) {
	char c1[4], c2[4], r[4];
	int result;
	*((int*)c1) = src1;
	*((int*)c2) = src2;
	r[0] = c1[0] - c2[0];
	r[1] = c1[1] - c2[1];
	r[2] = c1[2] - c2[2];
	r[3] = c1[3] - c2[3];
	result = *(int*)r;
	return result;
}


static inline int _swap4 (unsigned int src1) {
	unsigned char v0 = src1;
	unsigned char v1 = src1 >> 8;
	unsigned char v2 = src1 >> 16;
	unsigned char v3 = src1 >> 24;
	unsigned v = v0<<8 | v1 | v2<<24 | v3<<16;
	return v;
}

static inline unsigned int _unpkhu4 (unsigned int src1) {
	unsigned v0 = src1>>24;
	unsigned v1 = (src1>>16) & 0xff;
	return (v0<<16) | v1;
}

static inline unsigned int _unpklu4 (unsigned int src1) {
	unsigned v1 = (src1>>8) & 0xff;
	unsigned v0 = (src1) & 0xff;
	return (v1<<16) | v0;
}




static inline unsigned int _xpnd2 (unsigned int src1) {
      int v0 = (src1 & 0x1) ? 0x0000ffff : 0x00000000;
      int v1 = (src1 & 0x2) ? 0xffff0000 : 0x00000000;
      return v0|v1;
}

static inline unsigned int _xpnd4 (unsigned int src1) {
      int v0 = (src1 & 0x1) ? 0x000000ff : 0x00000000;
      int v1 = (src1 & 0x2) ? 0x0000ff00 : 0x00000000;
      int v2 = (src1 & 0x4) ? 0x00ff0000 : 0x00000000;
      int v3 = (src1 & 0x8) ? 0xff000000 : 0x00000000;
      int r = v0|v1|v2|v3;
      return r;
}



//     end of Implemented in alphabetical order


#endif /* __C6X_COMPAT__H */
