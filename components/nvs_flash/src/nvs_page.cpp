// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "nvs_page.hpp"
#if defined(ESP_PLATFORM)
#include <rom/crc.h>
#else
#include "crc.h"
#endif
#include <cstdio>
#include <cstring>

namespace nvs
{

uint32_t Page::Header::calculateCrc32()
{
    return crc32_le(0xffffffff,
                    reinterpret_cast<uint8_t*>(this) + offsetof(Header, mSeqNumber),
                    offsetof(Header, mCrc32) - offsetof(Header, mSeqNumber));
}

esp_err_t Page::load(uint32_t sectorNumber)
{
    mBaseAddress = sectorNumber * SEC_SIZE;
    mUsedEntryCount = 0;
    mErasedEntryCount = 0;

    Header header;
    auto rc = spi_flash_read(mBaseAddress, &header, sizeof(header));
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }
    if (header.mState == PageState::UNINITIALIZED) {
        mState = header.mState;
        // check if the whole page is really empty
        // reading the whole page takes ~40 times less than erasing it
        uint32_t line[8];
        for (uint32_t i = 0; i < SPI_FLASH_SEC_SIZE; i += sizeof(line)) {
            rc = spi_flash_read(mBaseAddress + i, line, sizeof(line));
            if (rc != ESP_OK) {
                mState = PageState::INVALID;
                return rc;
            }
            if (std::any_of(line, line + 4, [](uint32_t val) -> bool { return val != 0xffffffff; })) {
                // page isn't as empty after all, mark it as corrupted
                mState = PageState::CORRUPT;
                break;
            }
        }
    } else if (header.mCrc32 != header.calculateCrc32()) {
        header.mState = PageState::CORRUPT;
    } else {
        mState = header.mState;
        mSeqNumber = header.mSeqNumber;
    }

    switch (mState) {
    case PageState::UNINITIALIZED:
        break;

    case PageState::FULL:
    case PageState::ACTIVE:
    case PageState::FREEING:
        mLoadEntryTable();
        break;

    default:
        mState = PageState::CORRUPT;
        break;
    }

    return ESP_OK;
}

esp_err_t Page::writeEntry(const Item& item)
{
    auto rc = spi_flash_write(getEntryAddress(mNextFreeEntry), &item, sizeof(item));
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }

    auto err = alterEntryState(mNextFreeEntry, EntryState::WRITTEN);
    if (err != ESP_OK) {
        return err;
    }

    if (mFirstUsedEntry == INVALID_ENTRY) {
        mFirstUsedEntry = mNextFreeEntry;
    }

    ++mUsedEntryCount;
    ++mNextFreeEntry;

    return ESP_OK;
}
    
esp_err_t Page::writeEntryData(const uint8_t* data, size_t size)
{
    assert(size % ENTRY_SIZE == 0);
    assert(mNextFreeEntry != INVALID_ENTRY);
    assert(mFirstUsedEntry != INVALID_ENTRY);
    const uint16_t count = size / ENTRY_SIZE;
    
    const uint8_t* buf = data;
    
#ifdef ESP_PLATFORM
    /* On the ESP32, data can come from DROM, which is not accessible by spi_flash_write
     * function. To work around this, we copy the data to heap if it came from DROM.
     * Hopefully this won't happen very often in practice. For data from DRAM, we should
     * still be able to write it to flash directly.
     * TODO: figure out how to make this platform-specific check nicer (probably by introducing
     * a platform-specific flash layer).
     */
    if ((uint32_t) data < 0x3ff00000) {
        buf = (uint8_t*) malloc(size);
        if (!buf) {
            return ESP_ERR_NO_MEM;
        }
        memcpy((void*)buf, data, size);
    }
#endif //ESP_PLATFORM
    auto rc = spi_flash_write(getEntryAddress(mNextFreeEntry), buf, size);
#ifdef ESP_PLATFORM
    if (buf != data) {
        free((void*)buf);
    }
#endif //ESP_PLATFORM
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }
    auto err = alterEntryRangeState(mNextFreeEntry, mNextFreeEntry + count, EntryState::WRITTEN);
    if (err != ESP_OK) {
        return err;
    }
    mUsedEntryCount += count;
    mNextFreeEntry += count;
    return ESP_OK;
}

esp_err_t Page::writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize)
{
    Item item;
    esp_err_t err;
    
    if (mState == PageState::INVALID) {
        return ESP_ERR_NVS_INVALID_STATE;
    }
    
    if (mState == PageState::UNINITIALIZED) {
        err = initialize();
        if (err != ESP_OK) {
            return err;
        }
    }

    if (mState == PageState::FULL) {
        return ESP_ERR_NVS_PAGE_FULL;
    }

    const size_t keySize = strlen(key);
    if (keySize > Item::MAX_KEY_LENGTH) {
        return ESP_ERR_NVS_KEY_TOO_LONG;
    }
    
    if (dataSize > Page::BLOB_MAX_SIZE) {
        return ESP_ERR_NVS_VALUE_TOO_LONG;
    }

    size_t totalSize = ENTRY_SIZE;
    size_t entriesCount = 1;
    if (datatype == ItemType::SZ || datatype == ItemType::BLOB) {
        size_t roundedSize = (dataSize + ENTRY_SIZE - 1) & ~(ENTRY_SIZE - 1);
        totalSize += roundedSize;
        entriesCount += roundedSize / ENTRY_SIZE;
    }

    // primitive types should fit into one entry
    assert(totalSize == ENTRY_SIZE || datatype == ItemType::BLOB || datatype == ItemType::SZ);

    if (mNextFreeEntry == INVALID_ENTRY || mNextFreeEntry + entriesCount > ENTRY_COUNT) {
        // page will not fit this amount of data
        return ESP_ERR_NVS_PAGE_FULL;
    }

    // write first item
    size_t span = (totalSize + ENTRY_SIZE - 1) / ENTRY_SIZE;
    item = Item(nsIndex, datatype, span, key);
    mHashList.insert(item, mNextFreeEntry);

    if (datatype != ItemType::SZ && datatype != ItemType::BLOB) {
        memcpy(item.data, data, dataSize);
        item.crc32 = item.calculateCrc32();
        err = writeEntry(item);
        if (err != ESP_OK) {
            return err;
        }
    } else {
        const uint8_t* src = reinterpret_cast<const uint8_t*>(data);
        item.varLength.dataCrc32 = Item::calculateCrc32(src, dataSize);
        item.varLength.dataSize = dataSize;
        item.varLength.reserved2 = 0xffff;
        item.crc32 = item.calculateCrc32();
        err = writeEntry(item);
        if (err != ESP_OK) {
            return err;
        }

        size_t left = dataSize / ENTRY_SIZE * ENTRY_SIZE;
        if (left > 0) {
            err = writeEntryData(static_cast<const uint8_t*>(data), left);
            if (err != ESP_OK) {
                return err;
            }
        }
        
        size_t tail = dataSize - left;
        if (tail > 0) {
            std::fill_n(item.rawData, ENTRY_SIZE / 4, 0xffffffff);
            memcpy(item.rawData, static_cast<const uint8_t*>(data) + left, tail);
            err = writeEntry(item);
            if (err != ESP_OK) {
                return err;
            }
        }
    
    }
    return ESP_OK;
}

esp_err_t Page::readItem(uint8_t nsIndex, ItemType datatype, const char* key, void* data, size_t dataSize)
{
    size_t index = 0;
    Item item;
    
    if (mState == PageState::INVALID) {
        return ESP_ERR_NVS_INVALID_STATE;
    }
    
    esp_err_t rc = findItem(nsIndex, datatype, key, index, item);
    if (rc != ESP_OK) {
        return rc;
    }

    if (datatype != ItemType::SZ && datatype != ItemType::BLOB) {
        if (dataSize != getAlignmentForType(datatype)) {
            return ESP_ERR_NVS_TYPE_MISMATCH;
        }

        memcpy(data, item.data, dataSize);
        return ESP_OK;
    }

    if (dataSize < static_cast<size_t>(item.varLength.dataSize)) {
        return ESP_ERR_NVS_INVALID_LENGTH;
    }

    uint8_t* dst = reinterpret_cast<uint8_t*>(data);
    size_t left = item.varLength.dataSize;
    for (size_t i = index + 1; i < index + item.span; ++i) {
        Item ditem;
        rc = readEntry(i, ditem);
        if (rc != ESP_OK) {
            return rc;
        }
        size_t willCopy = ENTRY_SIZE;
        willCopy = (left < willCopy)?left:willCopy;
        memcpy(dst, ditem.rawData, willCopy);
        left -= willCopy;
        dst += willCopy;
    }
    if (Item::calculateCrc32(reinterpret_cast<uint8_t*>(data), item.varLength.dataSize) != item.varLength.dataCrc32) {
        rc = eraseEntryAndSpan(index);
        if (rc != ESP_OK) {
            return rc;
        }
        return ESP_ERR_NVS_NOT_FOUND;
    }
    return ESP_OK;
}

esp_err_t Page::eraseItem(uint8_t nsIndex, ItemType datatype, const char* key)
{
    size_t index = 0;
    Item item;
    esp_err_t rc = findItem(nsIndex, datatype, key, index, item);
    if (rc != ESP_OK) {
        return rc;
    }
    return eraseEntryAndSpan(index);
}

esp_err_t Page::findItem(uint8_t nsIndex, ItemType datatype, const char* key)
{
    size_t index = 0;
    Item item;
    return findItem(nsIndex, datatype, key, index, item);
}

esp_err_t Page::eraseEntryAndSpan(size_t index)
{
    auto state = mEntryTable.get(index);
    assert(state == EntryState::WRITTEN || state == EntryState::EMPTY);
    mHashList.erase(index);

    size_t span = 1;
    if (state == EntryState::WRITTEN) {
        Item item;
        auto rc = readEntry(index, item);
        if (rc != ESP_OK) {
            return rc;
        }
        if (item.calculateCrc32() != item.crc32) {
            rc = alterEntryState(index, EntryState::ERASED);
            --mUsedEntryCount;
            ++mErasedEntryCount;
            if (rc != ESP_OK) {
                return rc;
            }
        } else {
            span = item.span;
            for (ptrdiff_t i = index + span - 1; i >= static_cast<ptrdiff_t>(index); --i) {
                if (mEntryTable.get(i) == EntryState::WRITTEN) {
                    --mUsedEntryCount;
                }
                ++mErasedEntryCount;
            }
            if (span == 1) {
                rc = alterEntryState(index, EntryState::ERASED);
            } else {
                rc = alterEntryRangeState(index, index + span, EntryState::ERASED);
            }
            if (rc != ESP_OK) {
                return rc;
            }
        }
    } else {
        auto rc = alterEntryState(index, EntryState::ERASED);
        if (rc != ESP_OK) {
            return rc;
        }
    }

    if (index == mFirstUsedEntry) {
        updateFirstUsedEntry(index, span);
    }

    if (index + span > mNextFreeEntry) {
        mNextFreeEntry = index + span;
    }

    return ESP_OK;
}

void Page::updateFirstUsedEntry(size_t index, size_t span)
{
    assert(index == mFirstUsedEntry);
    mFirstUsedEntry = INVALID_ENTRY;
    size_t end = mNextFreeEntry;
    if (end > ENTRY_COUNT) {
        end = ENTRY_COUNT;
    }
    for (size_t i = index + span; i < end; ++i) {
        if (mEntryTable.get(i) == EntryState::WRITTEN) {
            mFirstUsedEntry = i;
            break;
        }
    }
}

esp_err_t Page::moveItem(Page& other)
{
    if (mFirstUsedEntry == INVALID_ENTRY) {
        return ESP_ERR_NVS_NOT_FOUND;
    }

    if (other.mState == PageState::UNINITIALIZED) {
        auto err = other.initialize();
        if (err != ESP_OK) {
            return err;
        }
    }

    Item entry;
    auto err = readEntry(mFirstUsedEntry, entry);
    if (err != ESP_OK) {
        return err;
    }
    other.mHashList.insert(entry, other.mNextFreeEntry);
    err = other.writeEntry(entry);
    if (err != ESP_OK) {
        return err;
    }

    size_t span = entry.span;
    size_t end = mFirstUsedEntry + span;

    assert(mFirstUsedEntry != INVALID_ENTRY || span == 1);

    for (size_t i = mFirstUsedEntry + 1; i < end; ++i) {
        readEntry(i, entry);
        err = other.writeEntry(entry);
        if (err != ESP_OK) {
            return err;
        }
    }
    return eraseEntryAndSpan(mFirstUsedEntry);
}

esp_err_t Page::mLoadEntryTable()
{
    // for states where we actually care about data in the page, read entry state table
    if (mState == PageState::ACTIVE ||
            mState == PageState::FULL ||
            mState == PageState::FREEING) {
        auto rc = spi_flash_read(mBaseAddress + ENTRY_TABLE_OFFSET, mEntryTable.data(),
                                 mEntryTable.byteSize());
        if (rc != ESP_OK) {
            mState = PageState::INVALID;
            return rc;
        }
    }

    mErasedEntryCount = 0;
    mUsedEntryCount = 0;
    for (size_t i = 0; i < ENTRY_COUNT; ++i) {
        auto s = mEntryTable.get(i);
        if (s == EntryState::WRITTEN) {
            if (mFirstUsedEntry == INVALID_ENTRY) {
                mFirstUsedEntry = i;
            }
            ++mUsedEntryCount;
        } else if (s == EntryState::ERASED) {
            ++mErasedEntryCount;
        }
    }

    // for PageState::ACTIVE, we may have more data written to this page
    // as such, we need to figure out where the first unused entry is
    if (mState == PageState::ACTIVE) {
        for (size_t i = 0; i < ENTRY_COUNT; ++i) {
            if (mEntryTable.get(i) == EntryState::EMPTY) {
                mNextFreeEntry = i;
                break;
            }
        }

        // however, if power failed after some data was written into the entry.
        // but before the entry state table was altered, the entry locacted via
        // entry state table may actually be half-written.
        // this is easy to check by reading EntryHeader (i.e. first word)
        while (mNextFreeEntry < ENTRY_COUNT) {
            uint32_t entryAddress = getEntryAddress(mNextFreeEntry);
            uint32_t header;
            auto rc = spi_flash_read(entryAddress, &header, sizeof(header));
            if (rc != ESP_OK) {
                mState = PageState::INVALID;
                return rc;
            }
            if (header != 0xffffffff) {
                auto oldState = mEntryTable.get(mNextFreeEntry);
                auto err = alterEntryState(mNextFreeEntry, EntryState::ERASED);
                if (err != ESP_OK) {
                    mState = PageState::INVALID;
                    return err;
                }
                ++mNextFreeEntry;
                if (oldState == EntryState::WRITTEN) {
                    --mUsedEntryCount;
                }
                ++mErasedEntryCount;
            }
            else {
                break;
            }
        }

        // check that all variable-length items are written or erased fully
        Item item;
        size_t lastItemIndex = INVALID_ENTRY;
        size_t end = mNextFreeEntry;
        if (end > ENTRY_COUNT) {
            end = ENTRY_COUNT;
        }
        size_t span;
        for (size_t i = 0; i < end; i += span) {
            span = 1;
            if (mEntryTable.get(i) == EntryState::ERASED) {
                lastItemIndex = INVALID_ENTRY;
                continue;
            }

            lastItemIndex = i;

            auto err = readEntry(i, item);
            if (err != ESP_OK) {
                mState = PageState::INVALID;
                return err;
            }
            
            mHashList.insert(item, i);
            
            // search for potential duplicate item
            size_t duplicateIndex = mHashList.find(0, item);

            if (item.crc32 != item.calculateCrc32()) {
                err = eraseEntryAndSpan(i);
                if (err != ESP_OK) {
                    mState = PageState::INVALID;
                    return err;
                }
                continue;
            }

            
            if (item.datatype == ItemType::BLOB || item.datatype == ItemType::SZ) {
                span = item.span;
                bool needErase = false;
                for (size_t j = i; j < i + span; ++j) {
                    if (mEntryTable.get(j) != EntryState::WRITTEN) {
                        needErase = true;
                        lastItemIndex = INVALID_ENTRY;
                        break;
                    }
                }
                if (needErase) {
                    eraseEntryAndSpan(i);
                    continue;
                }
            }
            
            if (duplicateIndex < i) {
                eraseEntryAndSpan(duplicateIndex);
            }
        }

        // check that last item is not duplicate
        if (lastItemIndex != INVALID_ENTRY) {
            size_t findItemIndex = 0;
            Item dupItem;
            if (findItem(item.nsIndex, item.datatype, item.key, findItemIndex, dupItem) == ESP_OK) {
                if (findItemIndex < lastItemIndex) {
                    auto err = eraseEntryAndSpan(findItemIndex);
                    if (err != ESP_OK) {
                        mState = PageState::INVALID;
                        return err;
                    }
                }
            }
        }
    } else if (mState == PageState::FULL || mState == PageState::FREEING) {
        // We have already filled mHashList for page in active state.
        // Do the same for the case when page is in full or freeing state.
        Item item;
        for (size_t i = mFirstUsedEntry; i < ENTRY_COUNT; ++i) {
            if (mEntryTable.get(i) != EntryState::WRITTEN) {
                continue;
            }

            auto err = readEntry(i, item);
            if (err != ESP_OK) {
                mState = PageState::INVALID;
                return err;
            }

            mHashList.insert(item, i);

            size_t span = item.span;
            i += span - 1;
        }

    }

    return ESP_OK;
}


esp_err_t Page::initialize()
{
    assert(mState == PageState::UNINITIALIZED);
    mState = PageState::ACTIVE;
    Header header;
    header.mState = mState;
    header.mSeqNumber = mSeqNumber;
    header.mCrc32 = header.calculateCrc32();

    auto rc = spi_flash_write(mBaseAddress, &header, sizeof(header));
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }

    mNextFreeEntry = 0;
    std::fill_n(mEntryTable.data(), mEntryTable.byteSize() / sizeof(uint32_t), 0xffffffff);
    return ESP_OK;
}

esp_err_t Page::alterEntryState(size_t index, EntryState state)
{
    assert(index < ENTRY_COUNT);
    mEntryTable.set(index, state);
    size_t wordToWrite = mEntryTable.getWordIndex(index);
    uint32_t word = mEntryTable.data()[wordToWrite];
    auto rc = spi_flash_write(mBaseAddress + ENTRY_TABLE_OFFSET + static_cast<uint32_t>(wordToWrite) * 4,
            &word, sizeof(word));
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }
    return ESP_OK;
}

esp_err_t Page::alterEntryRangeState(size_t begin, size_t end, EntryState state)
{
    assert(end <= ENTRY_COUNT);
    assert(end > begin);
    size_t wordIndex = mEntryTable.getWordIndex(end - 1);
    for (ptrdiff_t i = end - 1; i >= static_cast<ptrdiff_t>(begin); --i) {
        mEntryTable.set(i, state);
        size_t nextWordIndex;
        if (i == static_cast<ptrdiff_t>(begin)) {
            nextWordIndex = (size_t) -1;
        } else {
            nextWordIndex = mEntryTable.getWordIndex(i - 1);
        }
        if (nextWordIndex != wordIndex) {
            uint32_t word = mEntryTable.data()[wordIndex];
            auto rc = spi_flash_write(mBaseAddress + ENTRY_TABLE_OFFSET + static_cast<uint32_t>(wordIndex) * 4,
                    &word, 4);
            if (rc != ESP_OK) {
                return rc;
            }
        }
        wordIndex = nextWordIndex;
    }
    return ESP_OK;
}

esp_err_t Page::alterPageState(PageState state)
{
    uint32_t state_val = static_cast<uint32_t>(state);
    auto rc = spi_flash_write(mBaseAddress, &state_val, sizeof(state));
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }
    mState = (PageState) state;
    return ESP_OK;
}

esp_err_t Page::readEntry(size_t index, Item& dst) const
{
    auto rc = spi_flash_read(getEntryAddress(index), &dst, sizeof(dst));
    if (rc != ESP_OK) {
        return rc;
    }
    return ESP_OK;
}

esp_err_t Page::findItem(uint8_t nsIndex, ItemType datatype, const char* key, size_t &itemIndex, Item& item)
{
    if (mState == PageState::CORRUPT || mState == PageState::INVALID || mState == PageState::UNINITIALIZED) {
        return ESP_ERR_NVS_NOT_FOUND;
    }
    
    size_t findBeginIndex = itemIndex;
    if (findBeginIndex >= ENTRY_COUNT) {
        return ESP_ERR_NVS_NOT_FOUND;
    }

    size_t start = mFirstUsedEntry;
    if (findBeginIndex > mFirstUsedEntry && findBeginIndex < ENTRY_COUNT) {
        start = findBeginIndex;
    }

    size_t end = mNextFreeEntry;
    if (end > ENTRY_COUNT) {
        end = ENTRY_COUNT;
    }

    if (nsIndex != NS_ANY && datatype != ItemType::ANY && key != NULL) {
        size_t cachedIndex = mHashList.find(start, Item(nsIndex, datatype, 0, key));
        if (cachedIndex < ENTRY_COUNT) {
            start = cachedIndex;
        } else {
            return ESP_ERR_NVS_NOT_FOUND;
        }
    }

    size_t next;
    for (size_t i = start; i < end; i = next) {
        next = i + 1;
        if (mEntryTable.get(i) != EntryState::WRITTEN) {
            continue;
        }

        auto rc = readEntry(i, item);
        if (rc != ESP_OK) {
            mState = PageState::INVALID;
            return rc;
        }

        auto crc32 = item.calculateCrc32();
        if (item.crc32 != crc32) {
            eraseEntryAndSpan(i);
            continue;
        }

        if (item.datatype == ItemType::BLOB || item.datatype == ItemType::SZ) {
            next = i + item.span;
        }

        if (nsIndex != NS_ANY && item.nsIndex != nsIndex) {
            continue;
        }

        if (key != nullptr && strncmp(key, item.key, Item::MAX_KEY_LENGTH) != 0) {
            continue;
        }

        if (datatype != ItemType::ANY && item.datatype != datatype) {
            return ESP_ERR_NVS_TYPE_MISMATCH;
        }

        itemIndex = i;

        return ESP_OK;
    }

    return ESP_ERR_NVS_NOT_FOUND;
}

esp_err_t Page::getSeqNumber(uint32_t& seqNumber) const
{
    if (mState != PageState::UNINITIALIZED && mState != PageState::INVALID && mState != PageState::CORRUPT) {
        seqNumber = mSeqNumber;
        return ESP_OK;
    }
    return ESP_ERR_NVS_NOT_INITIALIZED;
}


esp_err_t Page::setSeqNumber(uint32_t seqNumber)
{
    if (mState != PageState::UNINITIALIZED) {
        return ESP_ERR_NVS_INVALID_STATE;
    }
    mSeqNumber = seqNumber;
    return ESP_OK;
}

esp_err_t Page::erase()
{
    auto sector = mBaseAddress / SPI_FLASH_SEC_SIZE;
    auto rc = spi_flash_erase_sector(sector);
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }
    mUsedEntryCount = 0;
    mErasedEntryCount = 0;
    mFirstUsedEntry = INVALID_ENTRY;
    mNextFreeEntry = INVALID_ENTRY;
    mState = PageState::UNINITIALIZED;
    mHashList.clear();
    return ESP_OK;
}

esp_err_t Page::markFreeing()
{
    if (mState != PageState::FULL && mState != PageState::ACTIVE) {
        return ESP_ERR_NVS_INVALID_STATE;
    }
    return alterPageState(PageState::FREEING);
}

esp_err_t Page::markFull()
{
    if (mState != PageState::ACTIVE) {
        return ESP_ERR_NVS_INVALID_STATE;
    }
    return alterPageState(PageState::FULL);
}
    
const char* Page::pageStateToName(PageState ps)
{
    switch (ps) {
        case PageState::CORRUPT:
            return "CORRUPT";
            
        case PageState::ACTIVE:
            return "ACTIVE";
            
        case PageState::FREEING:
            return "FREEING";
            
        case PageState::FULL:
            return "FULL";
            
        case PageState::INVALID:
            return "INVALID";
            
        case PageState::UNINITIALIZED:
            return "UNINITIALIZED";
            
        default:
            assert(0 && "invalid state value");
            return "";
    }
}

void Page::debugDump() const
{
    printf("state=%x (%s) addr=%x seq=%d\nfirstUsed=%d nextFree=%d used=%d erased=%d\n", (uint32_t) mState, pageStateToName(mState), mBaseAddress, mSeqNumber, static_cast<int>(mFirstUsedEntry), static_cast<int>(mNextFreeEntry), mUsedEntryCount, mErasedEntryCount);
    size_t skip = 0;
    for (size_t i = 0; i < ENTRY_COUNT; ++i) {
        printf("%3d: ", static_cast<int>(i));
        EntryState state = mEntryTable.get(i);
        if (state == EntryState::EMPTY) {
            printf("E\n");
        } else if (state == EntryState::ERASED) {
            printf("X\n");
        } else if (state == EntryState::WRITTEN) {
            Item item;
            readEntry(i, item);
            if (skip == 0) {
                printf("W ns=%2u type=%2u span=%3u key=\"%s\" len=%d\n", item.nsIndex, static_cast<unsigned>(item.datatype), item.span, item.key, (item.span != 1)?((int)item.varLength.dataSize):-1);
                if (item.span > 0 && item.span <= ENTRY_COUNT - i) {
                    skip = item.span - 1;
                } else {
                    skip = 0;
                }
            } else {
                printf("D\n");
                skip--;
            }
        }
    }
}

} // namespace nvs
