// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#ifndef nvs_page_hpp
#define nvs_page_hpp

#include "nvs.h"
#include "nvs_types.hpp"
#include <cstdint>
#include <type_traits>
#include <cstring>
#include <algorithm>
#include "esp_spi_flash.h"
#include "compressed_enum_table.hpp"
#include "intrusive_list.h"
#include "nvs_item_hash_list.hpp"

namespace nvs
{

class CachedFindInfo
{
public:
    CachedFindInfo() { }
    CachedFindInfo(uint8_t nsIndex, ItemType type, const char* key) :
        mKeyPtr(key),
        mNsIndex(nsIndex),
        mType(type)
    {
    }

    bool operator==(const CachedFindInfo& other) const
    {
        return mKeyPtr != nullptr && mKeyPtr == other.mKeyPtr && mType == other.mType && mNsIndex == other.mNsIndex;
    }

    void setItemIndex(uint32_t index)
    {
        mItemIndex = index;
    }
    uint32_t itemIndex() const
    {
        return mItemIndex;
    }

protected:
    uint32_t mItemIndex = 0;
    const char* mKeyPtr = nullptr;
    uint8_t mNsIndex = 0;
    ItemType mType;

};

class Page : public intrusive_list_node<Page>
{
public:
    static const uint32_t PSB_INIT = 0x1;
    static const uint32_t PSB_FULL = 0x2;
    static const uint32_t PSB_FREEING = 0x4;
    static const uint32_t PSB_CORRUPT = 0x8;

    static const uint32_t ESB_WRITTEN = 0x1;
    static const uint32_t ESB_ERASED = 0x2;

    static const uint32_t SEC_SIZE = SPI_FLASH_SEC_SIZE;

    static const size_t ENTRY_SIZE  = 32;
    static const size_t ENTRY_COUNT = 126;
    static const uint32_t INVALID_ENTRY = 0xffffffff;

    static const uint8_t NS_INDEX = 0;
    static const uint8_t NS_ANY = 255;

    enum class PageState : uint32_t {
        // All bits set, default state after flash erase. Page has not been initialized yet.
        UNINITIALIZED = 0xffffffff,

        // Page is initialized, and will accept writes.
        ACTIVE        = UNINITIALIZED & ~PSB_INIT,

        // Page is marked as full and will not accept new writes.
        FULL          = ACTIVE & ~PSB_FULL,

        // Data is being moved from this page to a new one.
        FREEING       = FULL & ~PSB_FREEING,

        // Page was found to be in a corrupt and unrecoverable state.
        // Instead of being erased immediately, it will be kept for diagnostics and data recovery.
        // It will be erased once we run out out free pages.
        CORRUPT       = FREEING & ~PSB_CORRUPT,

        // Page object wasn't loaded from flash memory
        INVALID       = 0
    };

    PageState state() const
    {
        return mState;
    }

    esp_err_t load(uint32_t sectorNumber);

    esp_err_t getSeqNumber(uint32_t& seqNumber) const;

    esp_err_t setSeqNumber(uint32_t seqNumber);

    esp_err_t writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize);

    esp_err_t readItem(uint8_t nsIndex, ItemType datatype, const char* key, void* data, size_t dataSize);

    esp_err_t eraseItem(uint8_t nsIndex, ItemType datatype, const char* key);

    esp_err_t findItem(uint8_t nsIndex, ItemType datatype, const char* key);

    esp_err_t findItem(uint8_t nsIndex, ItemType datatype, const char* key, size_t &itemIndex, Item& item);

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


    esp_err_t markFull();

    esp_err_t markFreeing();

    esp_err_t moveItem(Page& other);

    esp_err_t erase();

    void invalidateCache();

    void debugDump() const;

protected:

    class Header
    {
    public:
        Header()
        {
            std::fill_n(mReserved, sizeof(mReserved)/sizeof(mReserved[0]), UINT32_MAX);
        }

        PageState mState;       // page state
        uint32_t mSeqNumber;    // sequence number of this page
        uint32_t mReserved[5];  // unused, must be 0xffffffff
        uint32_t mCrc32;        // crc of everything except mState

        uint32_t calculateCrc32();
    };

    enum class EntryState {
        EMPTY   = 0x3, // 0b11, default state after flash erase
        WRITTEN = EMPTY & ~ESB_WRITTEN, // entry was written
        ERASED  = WRITTEN & ~ESB_ERASED, // entry was written and then erased
        INVALID = 0x4 // entry is in inconsistent state (write started but ESB_WRITTEN has not been set yet)
    };

    esp_err_t mLoadEntryTable();

    esp_err_t initialize();

    esp_err_t alterEntryState(size_t index, EntryState state);

    esp_err_t alterEntryRangeState(size_t begin, size_t end, EntryState state);

    esp_err_t alterPageState(PageState state);

    esp_err_t readEntry(size_t index, Item& dst) const;

    esp_err_t writeEntry(const Item& item);
    
    esp_err_t writeEntryData(const uint8_t* data, size_t size);

    esp_err_t eraseEntryAndSpan(size_t index);

    void updateFirstUsedEntry(size_t index, size_t span);

    static constexpr size_t getAlignmentForType(ItemType type)
    {
        return static_cast<uint8_t>(type) & 0x0f;
    }

    uint32_t getEntryAddress(size_t entry) const
    {
        assert(entry < ENTRY_COUNT);
        return mBaseAddress + ENTRY_DATA_OFFSET + static_cast<uint32_t>(entry) * ENTRY_SIZE;
    }
    
    static const char* pageStateToName(PageState ps);


protected:
    uint32_t mBaseAddress = 0;
    PageState mState = PageState::INVALID;
    uint32_t mSeqNumber = UINT32_MAX;
    typedef CompressedEnumTable<EntryState, 2, ENTRY_COUNT> TEntryTable;
    TEntryTable mEntryTable;
    size_t mNextFreeEntry = INVALID_ENTRY;
    size_t mFirstUsedEntry = INVALID_ENTRY;
    uint16_t mUsedEntryCount = 0;
    uint16_t mErasedEntryCount = 0;

    CachedFindInfo mFindInfo;
    HashList mHashList;

    static const uint32_t HEADER_OFFSET = 0;
    static const uint32_t ENTRY_TABLE_OFFSET = HEADER_OFFSET + 32;
    static const uint32_t ENTRY_DATA_OFFSET = ENTRY_TABLE_OFFSET + 32;

    static_assert(sizeof(Header) == 32, "header size must be 32 bytes");
    static_assert(ENTRY_TABLE_OFFSET % 32 == 0, "entry table offset should be aligned");
    static_assert(ENTRY_DATA_OFFSET % 32 == 0, "entry data offset should be aligned");

}; // class Page

} // namespace nvs


#endif /* nvs_page_hpp */
