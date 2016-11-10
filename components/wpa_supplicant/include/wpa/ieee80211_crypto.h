/*-
 * Copyright (c) 2001 Atsushi Onoe
 * Copyright (c) 2002-2008 Sam Leffler, Errno Consulting
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */

/*
 * copyright (c) 2010-2011  Espressif System
 */
#ifndef _NET80211_IEEE80211_CRYPTO_H_
#define _NET80211_IEEE80211_CRYPTO_H_

//#include "pp/esf_buf.h"

/*
 * 802.11 protocol crypto-related definitions.
 */
#define	IEEE80211_KEYBUF_SIZE	16
#define	IEEE80211_MICBUF_SIZE	(8+8)	/* space for both tx+rx keys */

/*
 * Old WEP-style key.  Deprecated.
 */

#if 0
struct ieee80211_rsnparms {
	uint8_t		rsn_mcastcipher;	/* mcast/group cipher */
	uint8_t		rsn_mcastkeylen;	/* mcast key length */
	uint8_t		rsn_ucastcipher;	/* selected unicast cipher */
	uint8_t		rsn_ucastkeylen;	/* unicast key length */
	uint8_t		rsn_keymgmt;		/* selected key mgmt algo */
	uint16_t	rsn_caps;		/* capabilities */
};
#endif //0000

/*
 * Template for a supported cipher.  Ciphers register with the
 * crypto code and are typically loaded as separate modules
 * (the null cipher is always present).
 * XXX may need refcnts
 */

/*
 * Crypto key state.  There is sufficient room for all supported
 * ciphers (see below).  The underlying ciphers are handled
 * separately through loadable cipher modules that register with
 * the generic crypto support.  A key has a reference to an instance
 * of the cipher; any per-key state is hung off wk_private by the
 * cipher when it is attached.  Ciphers are automatically called
 * to detach and cleanup any such state when the key is deleted.
 *
 * The generic crypto support handles encap/decap of cipher-related
 * frame contents for both hardware- and software-based implementations.
 * A key requiring software crypto support is automatically flagged and
 * the cipher is expected to honor this and do the necessary work.
 * Ciphers such as TKIP may also support mixed hardware/software
 * encrypt/decrypt and MIC processing.
 */
typedef uint16_t ieee80211_keyix;	/* h/w key index */

struct ieee80211_key {
	uint8_t		wk_keylen;	/* key length in bytes */
	uint8_t		wk_pad;
	uint16_t	wk_flags;
#define	IEEE80211_KEY_XMIT	0x0001	/* key used for xmit */
#define	IEEE80211_KEY_RECV	0x0002	/* key used for recv */
#define	IEEE80211_KEY_GROUP	0x0004	/* key used for WPA group operation */
#define	IEEE80211_KEY_SWENCRYPT	0x0010	/* host-based encrypt */
#define	IEEE80211_KEY_SWDECRYPT	0x0020	/* host-based decrypt */
#define	IEEE80211_KEY_SWENMIC	0x0040	/* host-based enmic */
#define	IEEE80211_KEY_SWDEMIC	0x0080	/* host-based demic */
#define	IEEE80211_KEY_DEVKEY	0x0100	/* device key request completed */
#define	IEEE80211_KEY_CIPHER0	0x1000	/* cipher-specific action 0 */
#define	IEEE80211_KEY_CIPHER1	0x2000	/* cipher-specific action 1 */
#define IEEE80211_KEY_EMPTY     0x0000
	ieee80211_keyix	wk_keyix;	/* h/w key index */
	ieee80211_keyix	wk_rxkeyix;	/* optional h/w rx key index */
	uint8_t		wk_key[IEEE80211_KEYBUF_SIZE+IEEE80211_MICBUF_SIZE];
#define	wk_txmic	wk_key+IEEE80211_KEYBUF_SIZE+0	/* XXX can't () right */
#define	wk_rxmic	wk_key+IEEE80211_KEYBUF_SIZE+8	/* XXX can't () right */
					/* key receive sequence counter */
	uint64_t	wk_keyrsc[IEEE80211_TID_SIZE];
	uint64_t	wk_keytsc;	/* key transmit sequence counter */
	const struct ieee80211_cipher *wk_cipher;
	//void		*wk_private;	/* private cipher state */
	//uint8_t		wk_macaddr[IEEE80211_ADDR_LEN]; //JLU: no need ...
};
#define	IEEE80211_KEY_COMMON 		/* common flags passed in by apps */\
	(IEEE80211_KEY_XMIT | IEEE80211_KEY_RECV | IEEE80211_KEY_GROUP)
#define	IEEE80211_KEY_DEVICE		/* flags owned by device driver */\
	(IEEE80211_KEY_DEVKEY|IEEE80211_KEY_CIPHER0|IEEE80211_KEY_CIPHER1)

#define	IEEE80211_KEY_SWCRYPT \
	(IEEE80211_KEY_SWENCRYPT | IEEE80211_KEY_SWDECRYPT)
#define	IEEE80211_KEY_SWMIC	(IEEE80211_KEY_SWENMIC | IEEE80211_KEY_SWDEMIC)

//#define	IEEE80211_KEYIX_NONE	((ieee80211_keyix) -1)

/*
 * NB: these values are ordered carefully; there are lots of
 * of implications in any reordering.  Beware that 4 is used
 * only to indicate h/w TKIP MIC support in driver capabilities;
 * there is no separate cipher support (it's rolled into the
 * TKIP cipher support).
 */
#define	IEEE80211_CIPHER_NONE		0	/* pseudo value */
#define	IEEE80211_CIPHER_TKIP		1
#define	IEEE80211_CIPHER_AES_OCB	2
#define	IEEE80211_CIPHER_AES_CCM	3
#define	IEEE80211_CIPHER_TKIPMIC	4	/* TKIP MIC capability */
#define	IEEE80211_CIPHER_CKIP		5
#define	IEEE80211_CIPHER_WEP		6
#define	IEEE80211_CIPHER_WEP40		7
#define	IEEE80211_CIPHER_WEP104		8


#define	IEEE80211_CIPHER_MAX		(IEEE80211_CIPHER_NONE+2)

/* capability bits in ic_cryptocaps/iv_cryptocaps */
#define	IEEE80211_CRYPTO_NONE		(1<<IEEE80211_CIPHER_NONE)
#define	IEEE80211_CRYPTO_WEP		(1<<IEEE80211_CIPHER_WEP)
#define	IEEE80211_CRYPTO_WEP40		(1<<IEEE80211_CIPHER_WEP40)
#define	IEEE80211_CRYPTO_WEP104	(1<<IEEE80211_CIPHER_WEP104)
#define	IEEE80211_CRYPTO_TKIP		(1<<IEEE80211_CIPHER_TKIP)
#define	IEEE80211_CRYPTO_AES_OCB	(1<<IEEE80211_CIPHER_AES_OCB)
#define	IEEE80211_CRYPTO_AES_CCM	(1<<IEEE80211_CIPHER_AES_CCM)
#define	IEEE80211_CRYPTO_TKIPMIC	(1<<IEEE80211_CIPHER_TKIPMIC)
#define	IEEE80211_CRYPTO_CKIP		(1<<IEEE80211_CIPHER_CKIP)

struct ieee80211_cipher {
	u_int	ic_cipher;		/* IEEE80211_CIPHER_* */
	u_int	ic_header;		/* size of privacy header (bytes) */
	u_int	ic_trailer;		/* size of privacy trailer (bytes) */
	u_int	ic_miclen;		/* size of mic trailer (bytes) */
//	int	(*ic_setkey)(struct ieee80211_key *);
	int	(*ic_encap)(struct ieee80211_key *, esf_buf_t *, uint8_t);
	int	(*ic_decap)(struct ieee80211_key *, esf_buf_t *, int);
#ifdef EAGLE_SW_MIC
	int	(*ic_enmic)(struct ieee80211_key *, esf_buf_t *, int);
	int	(*ic_demic)(struct ieee80211_key *, esf_buf_t *, int);
#endif /* EAGLE_SW_CRYPTO */
};

struct ieee80211com;
struct ieee80211_conn;

#define	IEEE80211_KEY_UNDEFINED(k) \
	((k)->wk_cipher == &ieee80211_cipher_none)

struct ieee80211_key *ieee80211_crypto_encap(struct ieee80211_conn *,
		esf_buf *);

struct ieee80211_key *ieee80211_crypto_decap(struct ieee80211_conn *,
		esf_buf *, int);

#if 0 //H/W MIC
/*
 * Check and remove any MIC.
 */
static INLINE int
ieee80211_crypto_demic(struct ieee80211vap *vap, struct ieee80211_key *k,
	esf_buf *m, int force)
{
	const struct ieee80211_cipher *cip = k->wk_cipher;
	return (cip->ic_miclen > 0 ? cip->ic_demic(k, m, force) : 1);
}

/*
 * Add any MIC.
 */
static INLINE int
ieee80211_crypto_enmic(struct ieee80211vap *vap,
	struct ieee80211_key *k, esf_buf *m, int force)
{
	const struct ieee80211_cipher *cip = k->wk_cipher;
	return (cip->ic_miclen > 0 ? cip->ic_enmic(k, m, force) : 1);
}
#endif //0000

/* 
 * Setup crypto support for a device/shared instance.
 */
void ieee80211_crypto_attach(struct ieee80211com *ic);

/* 
 * Reset key state to an unused state.  The crypto
 * key allocation mechanism insures other state (e.g.
 * key data) is properly setup before a key is used.
 */
static inline void
ieee80211_crypto_resetkey(struct ieee80211_key *k)
{
	k->wk_cipher = NULL;
	k->wk_flags = IEEE80211_KEY_XMIT | IEEE80211_KEY_RECV;
}

/*
 * Crypt-related notification methods.
 */
//void	ieee80211_notify_replay_failure(const struct ieee80211_frame *, const struct ieee80211_key *,
//		uint64_t rsc, int tid);
//void	ieee80211_notify_michael_failure(const struct ieee80211_frame *, u_int keyix);

#endif /* _NET80211_IEEE80211_CRYPTO_H_ */
