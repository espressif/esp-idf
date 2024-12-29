/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef nvs_page_hpp
#define nvs_page_hpp

#include "nvs.h"
#include "nvs_types.hpp"
#include <cstdint>
#include <type_traits>
#include <cstring>
#include <algorithm>
#include "compressed_enum_table.hpp"
#include "intrusive_list.h"
#include "nvs_item_hash_list.hpp"
#include "nvs_memory_management.hpp"
#include "partition.hpp"
#include "nvs_constants.h"

namespace nvs
{


class Page : public intrusive_list_node<Page>, public ExceptionlessAllocatable
{
public:
    static const uint32_t PSB_INIT = NVS_CONST_PSB_INIT;
    static const uint32_t PSB_FULL = NVS_CONST_PSB_FULL;
    static const uint32_t PSB_FREEING = NVS_CONST_PSB_FREEING;
    static const uint32_t PSB_CORRUPT = NVS_CONST_PSB_CORRUPT;

    static const uint32_t ESB_WRITTEN = NVS_CONST_ESB_WRITTEN;
    static const uint32_t ESB_ERASED = NVS_CONST_ESB_ERASED;

    static const uint32_t SEC_SIZE;

    static const size_t ENTRY_SIZE  = NVS_CONST_ENTRY_SIZE;
    static const size_t ENTRY_COUNT = NVS_CONST_ENTRY_COUNT;
    static const uint32_t INVALID_ENTRY = NVS_CONST_INVALID_ENTRY;

    static const size_t CHUNK_MAX_SIZE = ENTRY_SIZE * (ENTRY_COUNT - 1);

    static const uint8_t NS_INDEX = NVS_CONST_NS_INDEX;
    static const uint8_t NS_ANY = NVS_CONST_NS_ANY;

    static const uint8_t CHUNK_ANY = Item::CHUNK_ANY;

    static const uint8_t NVS_VERSION = NVS_CONST_NVS_VERSION; // Decrement to upgrade

    enum class PageState : uint32_t {
        // All bits set, default state after flash erase. Page has not been initialized yet.
        UNINITIALIZED = NVS_CONST_PAGE_STATE_UNINITIALIZED,

        // Page is initialized, and will accept writes.
        ACTIVE        = NVS_CONST_PAGE_STATE_ACTIVE,

        // Page is marked as full and will not accept new writes.
        FULL          = NVS_CONST_PAGE_STATE_FULL,

        // Data is being moved from this page to a new one.
        FREEING       = NVS_CONST_PAGE_STATE_FREEING,

        // Page was found to be in a corrupt and unrecoverable state.
        // Instead of being erased immediately, it will be kept for diagnostics and data recovery.
        // It will be erased once we run out out free pages.
        CORRUPT       = NVS_CONST_PAGE_STATE_CORRUPT,

        // Page object wasn't loaded from flash memory
        INVALID       = 0
    };

    Page();

    PageState state() const
    {
        return mState;
    }

    esp_err_t load(Partition *partition, uint32_t sectorNumber);

    esp_err_t getSeqNumber(uint32_t& seqNumber) const;

    esp_err_t setSeqNumber(uint32_t seqNumber);

    esp_err_t setVersion(uint8_t version);

    esp_err_t writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize, uint8_t chunkIdx = CHUNK_ANY);

    esp_err_t readItem(uint8_t nsIndex, ItemType datatype, const char* key, void* data, size_t dataSize, uint8_t chunkIdx = CHUNK_ANY, VerOffset chunkStart = VerOffset::VER_ANY);

    esp_err_t cmpItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize, uint8_t chunkIdx = CHUNK_ANY, VerOffset chunkStart = VerOffset::VER_ANY);

    esp_err_t eraseItem(uint8_t nsIndex, ItemType datatype, const char* key, uint8_t chunkIdx = CHUNK_ANY, VerOffset chunkStart = VerOffset::VER_ANY);

    esp_err_t findItem(uint8_t nsIndex, ItemType datatype, const char* key, uint8_t chunkIdx = CHUNK_ANY, VerOffset chunkStart = VerOffset::VER_ANY);

    esp_err_t findItem(uint8_t nsIndex, ItemType datatype, const char* key, size_t &itemIndex, Item& item, uint8_t chunkIdx = CHUNK_ANY, VerOffset chunkStart = VerOffset::VER_ANY);

    esp_err_t eraseEntryAndSpan(size_t index);

    template<typename T>
    esp_err_t writeItem(uint8_t nsIndex, const char* key, const T& value)
    {
        return writeItem(nsIndex, itemTypeOf(value), key, &value, sizeof(value));
    }

    template<typename T>
    esp_err_t readItem(uint8_t nsIndex, const char* key, T& value)
    {
        return readItem(nsIndex, itemTypeOf(value), key, &value, sizeof(value));
    }

    template<typename T>
    esp_err_t cmpItem(uint8_t nsIndex, const char* key, const T& value)
    {
        return cmpItem(nsIndex, itemTypeOf(value), key, &value, sizeof(value));
    }

    template<typename T>
    esp_err_t eraseItem(uint8_t nsIndex, const char* key)
    {
        return eraseItem(nsIndex, itemTypeOf<T>(), key);
    }

    size_t getUsedEntryCount() const
    {
        return mUsedEntryCount;
    }

    size_t getErasedEntryCount() const
    {
        return mErasedEntryCount;
    }
    size_t getVarDataTailroom() const ;

    esp_err_t markFull();

    esp_err_t markFreeing();

    esp_err_t copyItems(Page& other);

    esp_err_t erase();

    void debugDump() const;

    esp_err_t calcEntries(nvs_stats_t &nvsStats);

protected:

    class Header
    {
    public:
        Header()
        {
            std::fill_n(mReserved, sizeof(mReserved)/sizeof(mReserved[0]), UINT8_MAX);
        }

        PageState mState;       // page state
        uint32_t mSeqNumber;    // sequence number of this page
        uint8_t mVersion;       // nvs format version
        uint8_t mReserved[19];  // unused, must be 0xff
        uint32_t mCrc32;        // crc of everything except mState

        uint32_t calculateCrc32();
    };

    enum class EntryState {
        EMPTY   = NVS_CONST_ENTRY_STATE_EMPTY, // 0b11, default state after flash erase
        WRITTEN = NVS_CONST_ENTRY_STATE_WRITTEN, // entry was written
        ERASED  = NVS_CONST_ENTRY_STATE_ERASED, // entry was written and then erased
        ILLEGAL = NVS_CONST_ENTRY_STATE_ILLEGAL, // only possible if flash is inconsistent
        INVALID = NVS_CONST_ENTRY_STATE_INVALID // entry is in inconsistent state (write started but ESB_WRITTEN has not been set yet)
    };

    esp_err_t mLoadEntryTable();

    esp_err_t initialize();

    esp_err_t alterEntryState(size_t index, EntryState state);

    esp_err_t alterEntryRangeState(size_t begin, size_t end, EntryState state);

    esp_err_t alterPageState(PageState state);

    esp_err_t readEntry(size_t index, Item& dst) const;

    esp_err_t writeEntry(const Item& item);

    esp_err_t writeEntryData(const uint8_t* data, size_t size);

    esp_err_t updateFirstUsedEntry(size_t index, size_t span);

    static constexpr size_t getAlignmentForType(ItemType type)
    {
        return static_cast<uint8_t>(type) & 0x0f;
    }

    esp_err_t getEntryAddress(size_t entry, uint32_t *address) const
    {
        NVS_ASSERT_OR_RETURN(entry < ENTRY_COUNT, ESP_FAIL);
        *address =  mBaseAddress + ENTRY_DATA_OFFSET + static_cast<uint32_t>(entry) * ENTRY_SIZE;
        return ESP_OK;
    }

    static const char* pageStateToName(PageState ps);


protected:
    uint32_t mBaseAddress = 0;
    PageState mState = PageState::INVALID;
    uint32_t mSeqNumber = UINT32_MAX;
    uint8_t mVersion = NVS_VERSION;
    typedef CompressedEnumTable<EntryState, 2, ENTRY_COUNT> TEntryTable;
    TEntryTable mEntryTable;
    size_t mNextFreeEntry = INVALID_ENTRY;
    size_t mFirstUsedEntry = INVALID_ENTRY;
    uint16_t mUsedEntryCount = 0;
    uint16_t mErasedEntryCount = 0;

    /**
     * This hash list stores hashes of namespace index, key, and ChunkIndex for quick lookup when searching items.
     */
    HashList mHashList;

    Partition *mPartition;

    static const uint32_t HEADER_OFFSET = NVS_CONST_PAGE_HEADER_OFFSET;
    static const uint32_t ENTRY_TABLE_OFFSET = NVS_CONST_PAGE_ENTRY_TABLE_OFFSET;
    static const uint32_t ENTRY_DATA_OFFSET = NVS_CONST_PAGE_ENTRY_DATA_OFFSET;

    static_assert(sizeof(Header) == 32, "header size must be 32 bytes");
    static_assert(ENTRY_TABLE_OFFSET % 32 == 0, "entry table offset should be aligned");
    static_assert(ENTRY_DATA_OFFSET % 32 == 0, "entry data offset should be aligned");

}; // class Page

} // namespace nvs


#endif /* nvs_page_hpp */
