/*
 * Simultaneous authentication of equals
 * Copyright (c) 2012-2013, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */
#ifdef CONFIG_WPA3_SAE

#ifndef SAE_H
#define SAE_H

#include "esp_err.h"
#include "utils/includes.h"
#include "utils/common.h"
#include "utils/wpa_debug.h"

#define SAE_KCK_LEN 32
#define SAE_PMK_LEN 32
#define SAE_PMK_LEN_MAX 64
#define SAE_PMKID_LEN 16
#define SAE_KEYSEED_KEY_LEN 32
#define SAE_MAX_PRIME_LEN 512
#define SAE_MAX_ECC_PRIME_LEN 66
#define SAE_MAX_HASH_LEN 64
#define SAE_COMMIT_MAX_LEN (2 + 3 * SAE_MAX_PRIME_LEN + 255)
#ifdef CONFIG_SAE_PK
#define SAE_CONFIRM_MAX_LEN ((2 + SAE_MAX_HASH_LEN) + 1500)
#else /* CONFIG_SAE_PK */
#define SAE_CONFIRM_MAX_LEN (2 + SAE_MAX_HASH_LEN)
#endif /* CONFIG_SAE_PK */
#define SAE_PK_M_LEN 16

/* Special value returned by sae_parse_commit() */
#define SAE_SILENTLY_DISCARD 65535

struct sae_pk {
	struct wpabuf *m;
	struct crypto_ec_key *key;
	int group;
	struct wpabuf *pubkey; /* DER encoded subjectPublicKey */
};

struct sae_temporary_data {
	u8 kck[SAE_MAX_HASH_LEN];
	size_t kck_len;
	struct crypto_bignum *own_commit_scalar;
	struct crypto_bignum *own_commit_element_ffc;
	struct crypto_ec_point *own_commit_element_ecc;
	struct crypto_bignum *peer_commit_element_ffc;
	struct crypto_ec_point *peer_commit_element_ecc;
	struct crypto_ec_point *pwe_ecc;
	struct crypto_bignum *pwe_ffc;
	struct crypto_bignum *sae_rand;
	struct crypto_ec *ec;
	int prime_len;
	const struct dh_group *dh;
	const struct crypto_bignum *prime;
	const struct crypto_bignum *order;
	struct crypto_bignum *prime_buf;
	struct crypto_bignum *order_buf;
	struct wpabuf *anti_clogging_token;
	char *pw_id;
	int order_len;
	struct wpabuf *own_rejected_groups;
	struct wpabuf *peer_rejected_groups;
	unsigned int own_addr_higher:1;

#ifdef CONFIG_SAE_PK
	u8 kek[SAE_MAX_HASH_LEN];
	size_t kek_len;
	const struct sae_pk *ap_pk;
	u8 own_addr[ETH_ALEN];
	u8 peer_addr[ETH_ALEN];
	u8 fingerprint[SAE_MAX_HASH_LEN];
	size_t fingerprint_bytes;
	size_t fingerprint_bits;
	size_t lambda;
	unsigned int sec;
	u8 ssid[32];
	size_t ssid_len;
#endif
};

struct sae_pt {
	struct sae_pt *next;
	int group;
	struct crypto_ec *ec;
	struct crypto_ec_point *ecc_pt;

	const struct dh_group *dh;
	struct crypto_bignum *ffc_pt;
#ifdef CONFIG_SAE_PK
	u8 ssid[32];
	size_t ssid_len;
#endif /* CONFIG_SAE_PK */
};

enum {
	SAE_MSG_COMMIT = 1,
	SAE_MSG_CONFIRM = 2,
};

enum sae_state {
	SAE_NOTHING, SAE_COMMITTED, SAE_CONFIRMED, SAE_ACCEPTED
};

struct sae_data {
	enum sae_state state;
	u16 send_confirm;
	u8 pmk[SAE_PMK_LEN];
	size_t pmk_len;
	u8 pmkid[SAE_PMKID_LEN];

	struct crypto_bignum *peer_commit_scalar;
	struct crypto_bignum *peer_commit_scalar_accepted;
	int group;
	unsigned int sync; /* protocol instance variable: Sync */
	u16 rc; /* protocol instance variable: Rc (received send-confirm) */
	struct sae_temporary_data *tmp;
	unsigned int h2e:1;
	unsigned int pk:1;
};

int sae_set_group(struct sae_data *sae, int group);
void sae_clear_temp_data(struct sae_data *sae);
void sae_clear_data(struct sae_data *sae);

int sae_prepare_commit(const u8 *addr1, const u8 *addr2,
		       const u8 *password, size_t password_len,
		       struct sae_data *sae);
int sae_prepare_commit_pt(struct sae_data *sae, const struct sae_pt *pt,
			  const u8 *addr1, const u8 *addr2,
			  int *rejected_groups, const struct sae_pk *pk);
int sae_process_commit(struct sae_data *sae);
int sae_write_commit(struct sae_data *sae, struct wpabuf *buf,
		     const struct wpabuf *token, const char *identifier);
u16 sae_parse_commit(struct sae_data *sae, const u8 *data, size_t len,
		     const u8 **token, size_t *token_len, int *allowed_groups,
		     int h2e);
int sae_write_confirm(struct sae_data *sae, struct wpabuf *buf);
int sae_check_confirm(struct sae_data *sae, const u8 *data, size_t len);
u16 sae_group_allowed(struct sae_data *sae, int *allowed_groups, u16 group);
const char * sae_state_txt(enum sae_state state);
size_t sae_ecc_prime_len_2_hash_len(size_t prime_len);
size_t sae_ffc_prime_len_2_hash_len(size_t prime_len);
struct sae_pt * sae_derive_pt(int *groups, const u8 *ssid, size_t ssid_len,
			      const u8 *password, size_t password_len,
			      const char *identifier);
struct crypto_ec_point *
sae_derive_pwe_from_pt_ecc(const struct sae_pt *pt,
			   const u8 *addr1, const u8 *addr2);
struct crypto_bignum *
sae_derive_pwe_from_pt_ffc(const struct sae_pt *pt,
			   const u8 *addr1, const u8 *addr2);
void sae_deinit_pt(struct sae_pt *pt);

/* sae_pk.c */
#ifdef CONFIG_SAE_PK
bool sae_pk_valid_password(const char *pw);
#else /* CONFIG_SAE_PK */
static inline bool sae_pk_valid_password(const char *pw)
{
	return false;
}
#endif /* CONFIG_SAE_PK */
char * sae_pk_base32_encode(const u8 *src, size_t len_bits);
u8 * sae_pk_base32_decode(const char *src, size_t len, size_t *out_len);
int sae_pk_set_password(struct sae_data *sae, const char *password);
void sae_deinit_pk(struct sae_pk *pk);
struct sae_pk * sae_parse_pk(const char *val);
int sae_write_confirm_pk(struct sae_data *sae, struct wpabuf *buf);
int sae_check_confirm_pk(struct sae_data *sae, const u8 *ies, size_t ies_len);
int sae_hash(size_t hash_len, const u8 *data, size_t len, u8 *hash);
u32 sae_pk_get_be19(const u8 *buf);
void sae_pk_buf_shift_left_19(u8 *buf, size_t len);

#endif /* SAE_H */
#endif /* CONFIG_WPA3_SAE */
