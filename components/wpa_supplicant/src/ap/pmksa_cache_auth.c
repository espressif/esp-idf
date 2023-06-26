/*
 * hostapd - PMKSA cache for IEEE 802.11i RSN
 * Copyright (c) 2004-2008, 2012-2015, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#include "utils/includes.h"

#include "utils/common.h"
#include "utils/eloop.h"
#include "sta_info.h"
#include "ap_config.h"
#include "pmksa_cache_auth.h"
#include "common/eapol_common.h"
#include "common/ieee802_11_defs.h"
#include "ap/pmksa_cache_auth.h"
#include "ap/ieee802_1x.h"

static const int pmksa_cache_max_entries = 10;
static const int dot11RSNAConfigPMKLifetime = 8640000;


struct rsn_pmksa_cache {
#define PMKID_HASH_SIZE 128
#define PMKID_HASH(pmkid) (unsigned int) ((pmkid)[0] & 0x7f)
	struct rsn_pmksa_cache_entry *pmkid[PMKID_HASH_SIZE];
	struct rsn_pmksa_cache_entry *pmksa;
	int pmksa_count;

	void (*free_cb)(struct rsn_pmksa_cache_entry *entry, void *ctx);
	void *ctx;
};

static void pmksa_cache_set_expiration(struct rsn_pmksa_cache *pmksa);


static void _pmksa_cache_free_entry(struct rsn_pmksa_cache_entry *entry)
{
	os_free(entry->vlan_desc);
	os_free(entry->identity);
	wpabuf_free(entry->cui);
	bin_clear_free(entry, sizeof(*entry));
}


void pmksa_cache_free_entry(struct rsn_pmksa_cache *pmksa,
			    struct rsn_pmksa_cache_entry *entry)
{
	struct rsn_pmksa_cache_entry *pos, *prev;
	unsigned int hash;

	pmksa->pmksa_count--;
	pmksa->free_cb(entry, pmksa->ctx);

	/* unlink from hash list */
	hash = PMKID_HASH(entry->pmkid);
	pos = pmksa->pmkid[hash];
	prev = NULL;
	while (pos) {
		if (pos == entry) {
			if (prev != NULL)
				prev->hnext = entry->hnext;
			else
				pmksa->pmkid[hash] = entry->hnext;
			break;
		}
		prev = pos;
		pos = pos->hnext;
	}

	/* unlink from entry list */
	pos = pmksa->pmksa;
	prev = NULL;
	while (pos) {
		if (pos == entry) {
			if (prev != NULL)
				prev->next = entry->next;
			else
				pmksa->pmksa = entry->next;
			break;
		}
		prev = pos;
		pos = pos->next;
	}

	_pmksa_cache_free_entry(entry);
}


/**
 * pmksa_cache_auth_flush - Flush all PMKSA cache entries
 * @pmksa: Pointer to PMKSA cache data from pmksa_cache_auth_init()
 */
void pmksa_cache_auth_flush(struct rsn_pmksa_cache *pmksa)
{
	while (pmksa->pmksa) {
		wpa_printf(MSG_DEBUG, "RSN: Flush PMKSA cache entry for "
			   MACSTR, MAC2STR(pmksa->pmksa->spa));
		pmksa_cache_free_entry(pmksa, pmksa->pmksa);
	}
}


static void pmksa_cache_expire(void *eloop_ctx, void *timeout_ctx)
{
	struct rsn_pmksa_cache *pmksa = eloop_ctx;
	struct os_reltime now;

	os_get_reltime(&now);
	while (pmksa->pmksa && pmksa->pmksa->expiration <= now.sec) {
		wpa_printf(MSG_DEBUG, "RSN: expired PMKSA cache entry for "
			   MACSTR, MAC2STR(pmksa->pmksa->spa));
		pmksa_cache_free_entry(pmksa, pmksa->pmksa);
	}

	pmksa_cache_set_expiration(pmksa);
}


static void pmksa_cache_set_expiration(struct rsn_pmksa_cache *pmksa)
{
	int sec;
	struct os_reltime now;

	eloop_cancel_timeout(pmksa_cache_expire, pmksa, NULL);
	if (pmksa->pmksa == NULL)
		return;
	os_get_reltime(&now);
	sec = pmksa->pmksa->expiration - now.sec;
	if (sec < 0)
		sec = 0;
	eloop_register_timeout(sec + 1, 0, pmksa_cache_expire, pmksa, NULL);
}


static void pmksa_cache_from_eapol_data(struct rsn_pmksa_cache_entry *entry,
					struct eapol_state_machine *eapol)
{
	if (eapol == NULL)
		return;
}


static void pmksa_cache_link_entry(struct rsn_pmksa_cache *pmksa,
				   struct rsn_pmksa_cache_entry *entry)
{
	struct rsn_pmksa_cache_entry *pos, *prev;
	int hash;

	/* Add the new entry; order by expiration time */
	pos = pmksa->pmksa;
	prev = NULL;
	while (pos) {
		if (pos->expiration > entry->expiration)
			break;
		prev = pos;
		pos = pos->next;
	}
	if (prev == NULL) {
		entry->next = pmksa->pmksa;
		pmksa->pmksa = entry;
	} else {
		entry->next = prev->next;
		prev->next = entry;
	}

	hash = PMKID_HASH(entry->pmkid);
	entry->hnext = pmksa->pmkid[hash];
	pmksa->pmkid[hash] = entry;

	pmksa->pmksa_count++;
	if (prev == NULL)
		pmksa_cache_set_expiration(pmksa);
	wpa_printf(MSG_DEBUG, "RSN: added PMKSA cache entry for " MACSTR,
		   MAC2STR(entry->spa));
	wpa_hexdump(MSG_DEBUG, "RSN: added PMKID", entry->pmkid, PMKID_LEN);
}


/**
 * pmksa_cache_auth_add - Add a PMKSA cache entry
 * @pmksa: Pointer to PMKSA cache data from pmksa_cache_auth_init()
 * @pmk: The new pairwise master key
 * @pmk_len: PMK length in bytes, usually PMK_LEN (32)
 * @pmkid: Calculated PMKID
 * @kck: Key confirmation key or %NULL if not yet derived
 * @kck_len: KCK length in bytes
 * @aa: Authenticator address
 * @spa: Supplicant address
 * @session_timeout: Session timeout
 * @eapol: Pointer to EAPOL state machine data
 * @akmp: WPA_KEY_MGMT_* used in key derivation
 * Returns: Pointer to the added PMKSA cache entry or %NULL on error
 *
 * This function create a PMKSA entry for a new PMK and adds it to the PMKSA
 * cache. If an old entry is already in the cache for the same Supplicant,
 * this entry will be replaced with the new entry. PMKID will be calculated
 * based on the PMK.
 */
struct rsn_pmksa_cache_entry *
pmksa_cache_auth_add(struct rsn_pmksa_cache *pmksa,
		     const u8 *pmk, size_t pmk_len, const u8 *pmkid,
		     const u8 *kck, size_t kck_len,
		     const u8 *aa, const u8 *spa, int session_timeout,
		     struct eapol_state_machine *eapol, int akmp)
{
	struct rsn_pmksa_cache_entry *entry;

	entry = pmksa_cache_auth_create_entry(pmk, pmk_len, pmkid, kck, kck_len,
					      aa, spa, session_timeout, eapol,
					      akmp);

	if (pmksa_cache_auth_add_entry(pmksa, entry) < 0)
		return NULL;

	return entry;
}


/**
 * pmksa_cache_auth_create_entry - Create a PMKSA cache entry
 * @pmk: The new pairwise master key
 * @pmk_len: PMK length in bytes, usually PMK_LEN (32)
 * @pmkid: Calculated PMKID
 * @kck: Key confirmation key or %NULL if not yet derived
 * @kck_len: KCK length in bytes
 * @aa: Authenticator address
 * @spa: Supplicant address
 * @session_timeout: Session timeout
 * @eapol: Pointer to EAPOL state machine data
 * @akmp: WPA_KEY_MGMT_* used in key derivation
 * Returns: Pointer to the added PMKSA cache entry or %NULL on error
 *
 * This function creates a PMKSA entry.
 */
struct rsn_pmksa_cache_entry *
pmksa_cache_auth_create_entry(const u8 *pmk, size_t pmk_len, const u8 *pmkid,
			      const u8 *kck, size_t kck_len, const u8 *aa,
			      const u8 *spa, int session_timeout,
			      struct eapol_state_machine *eapol, int akmp)
{
	struct rsn_pmksa_cache_entry *entry;
	struct os_reltime now;

	if (pmk_len > PMK_LEN_MAX)
		return NULL;

	if (wpa_key_mgmt_suite_b(akmp) && !kck)
		return NULL;

	entry = os_zalloc(sizeof(*entry));
	if (entry == NULL)
		return NULL;
	os_memcpy(entry->pmk, pmk, pmk_len);
	entry->pmk_len = pmk_len;
	if (pmkid)
		os_memcpy(entry->pmkid, pmkid, PMKID_LEN);
	else if (akmp == WPA_KEY_MGMT_IEEE8021X_SUITE_B_192)
		rsn_pmkid_suite_b_192(kck, kck_len, aa, spa, entry->pmkid);
	else if (wpa_key_mgmt_suite_b(akmp))
		rsn_pmkid_suite_b(kck, kck_len, aa, spa, entry->pmkid);
	else
		rsn_pmkid(pmk, pmk_len, aa, spa, entry->pmkid, akmp);
	os_get_reltime(&now);
	entry->expiration = now.sec;
	if (session_timeout > 0)
		entry->expiration += session_timeout;
	else
		entry->expiration += dot11RSNAConfigPMKLifetime;
	entry->akmp = akmp;
	os_memcpy(entry->spa, spa, ETH_ALEN);
	pmksa_cache_from_eapol_data(entry, eapol);

	return entry;
}


/**
 * pmksa_cache_auth_add_entry - Add a PMKSA cache entry
 * @pmksa: Pointer to PMKSA cache data from pmksa_cache_auth_init()
 * @entry: Pointer to PMKSA cache entry
 *
 * This function adds PMKSA cache entry to the PMKSA cache. If an old entry is
 * already in the cache for the same Supplicant, this entry will be replaced
 * with the new entry. PMKID will be calculated based on the PMK.
 */
int pmksa_cache_auth_add_entry(struct rsn_pmksa_cache *pmksa,
			       struct rsn_pmksa_cache_entry *entry)
{
	struct rsn_pmksa_cache_entry *pos;

	if (entry == NULL)
		return -1;

	/* Replace an old entry for the same STA (if found) with the new entry
	 */
	pos = pmksa_cache_auth_get(pmksa, entry->spa, NULL);
	if (pos)
		pmksa_cache_free_entry(pmksa, pos);

	if (pmksa->pmksa_count >= pmksa_cache_max_entries && pmksa->pmksa) {
		/* Remove the oldest entry to make room for the new entry */
		wpa_printf(MSG_DEBUG, "RSN: removed the oldest PMKSA cache "
			   "entry (for " MACSTR ") to make room for new one",
			   MAC2STR(pmksa->pmksa->spa));
		pmksa_cache_free_entry(pmksa, pmksa->pmksa);
	}

	pmksa_cache_link_entry(pmksa, entry);

	return 0;
}


/**
 * pmksa_cache_auth_deinit - Free all entries in PMKSA cache
 * @pmksa: Pointer to PMKSA cache data from pmksa_cache_auth_init()
 */
void pmksa_cache_auth_deinit(struct rsn_pmksa_cache *pmksa)
{
	struct rsn_pmksa_cache_entry *entry, *prev;
	int i;

	if (pmksa == NULL)
		return;

	entry = pmksa->pmksa;
	while (entry) {
		prev = entry;
		entry = entry->next;
		_pmksa_cache_free_entry(prev);
	}
	eloop_cancel_timeout(pmksa_cache_expire, pmksa, NULL);
	pmksa->pmksa_count = 0;
	pmksa->pmksa = NULL;
	for (i = 0; i < PMKID_HASH_SIZE; i++)
		pmksa->pmkid[i] = NULL;
	os_free(pmksa);
}


/**
 * pmksa_cache_auth_get - Fetch a PMKSA cache entry
 * @pmksa: Pointer to PMKSA cache data from pmksa_cache_auth_init()
 * @spa: Supplicant address or %NULL to match any
 * @pmkid: PMKID or %NULL to match any
 * Returns: Pointer to PMKSA cache entry or %NULL if no match was found
 */
struct rsn_pmksa_cache_entry *
pmksa_cache_auth_get(struct rsn_pmksa_cache *pmksa,
		     const u8 *spa, const u8 *pmkid)
{
	struct rsn_pmksa_cache_entry *entry;

	if (pmkid) {
		for (entry = pmksa->pmkid[PMKID_HASH(pmkid)]; entry;
		     entry = entry->hnext) {
			if ((spa == NULL ||
			     os_memcmp(entry->spa, spa, ETH_ALEN) == 0) &&
			    os_memcmp(entry->pmkid, pmkid, PMKID_LEN) == 0)
				return entry;
		}
	} else {
		for (entry = pmksa->pmksa; entry; entry = entry->next) {
			if (spa == NULL ||
			    os_memcmp(entry->spa, spa, ETH_ALEN) == 0)
				return entry;
		}
	}

	return NULL;
}


/**
 * pmksa_cache_auth_init - Initialize PMKSA cache
 * @free_cb: Callback function to be called when a PMKSA cache entry is freed
 * @ctx: Context pointer for free_cb function
 * Returns: Pointer to PMKSA cache data or %NULL on failure
 */
struct rsn_pmksa_cache *
pmksa_cache_auth_init(void (*free_cb)(struct rsn_pmksa_cache_entry *entry,
				      void *ctx), void *ctx)
{
	struct rsn_pmksa_cache *pmksa;

	pmksa = os_zalloc(sizeof(*pmksa));
	if (pmksa) {
		pmksa->free_cb = free_cb;
		pmksa->ctx = ctx;
	}

	return pmksa;
}


/**
 * pmksa_cache_auth_list - Dump text list of entries in PMKSA cache
 * @pmksa: Pointer to PMKSA cache data from pmksa_cache_auth_init()
 * @buf: Buffer for the list
 * @len: Length of the buffer
 * Returns: Number of bytes written to buffer
 *
 * This function is used to generate a text format representation of the
 * current PMKSA cache contents for the ctrl_iface PMKSA command.
 */
int pmksa_cache_auth_list(struct rsn_pmksa_cache *pmksa, char *buf, size_t len)
{
	int i, ret;
	char *pos = buf;
	struct rsn_pmksa_cache_entry *entry;
	struct os_reltime now;

	os_get_reltime(&now);
	ret = os_snprintf(pos, buf + len - pos,
			  "Index / SPA / PMKID / expiration (in seconds) / opportunistic\n");
	if (os_snprintf_error(buf + len - pos, ret))
		return pos - buf;
	pos += ret;
	i = 0;
	entry = pmksa->pmksa;
	while (entry) {
		ret = os_snprintf(pos, buf + len - pos, "%d " MACSTR " ",
				  i, MAC2STR(entry->spa));
		if (os_snprintf_error(buf + len - pos, ret))
			return pos - buf;
		pos += ret;
		pos += wpa_snprintf_hex(pos, buf + len - pos, entry->pmkid,
					PMKID_LEN);
		ret = os_snprintf(pos, buf + len - pos, " %d %d\n",
				  (int) (entry->expiration - now.sec),
				  entry->opportunistic);
		if (os_snprintf_error(buf + len - pos, ret))
			return pos - buf;
		pos += ret;
		entry = entry->next;
	}
	return pos - buf;
}
