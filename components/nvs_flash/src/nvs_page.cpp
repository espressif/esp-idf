/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_page.hpp"
#include <inttypes.h>
#include <esp_rom_crc.h>
#include <cstdio>
#include <cstring>
#include "nvs_internal.h"

namespace nvs
{

Page::Page() : mPartition(nullptr) { }

uint32_t Page::Header::calculateCrc32()
{
    return esp_rom_crc32_le(0xffffffff,
                    reinterpret_cast<uint8_t*>(this) + offsetof(Header, mSeqNumber),
                    offsetof(Header, mCrc32) - offsetof(Header, mSeqNumber));
}

esp_err_t Page::load(Partition *partition, uint32_t sectorNumber)
{
    if (partition == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    mPartition = partition;
    mBaseAddress = sectorNumber * SEC_SIZE;
    mUsedEntryCount = 0;
    mErasedEntryCount = 0;

    Header header;
    auto rc = mPartition->read_raw(mBaseAddress, &header, sizeof(header));
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }
    if (header.mState == PageState::UNINITIALIZED) {
        mState = header.mState;
        // check if the whole page is really empty
        // reading the whole page takes ~40 times less than erasing it
        const int BLOCK_SIZE = 128;
        uint32_t* block = new (std::nothrow) uint32_t[BLOCK_SIZE];

        if (!block) return ESP_ERR_NO_MEM;

        for (uint32_t i = 0; i < SPI_FLASH_SEC_SIZE; i += 4 * BLOCK_SIZE) {
            rc = mPartition->read_raw(mBaseAddress + i, block, 4 * BLOCK_SIZE);
            if (rc != ESP_OK) {
                mState = PageState::INVALID;
                delete[] block;
                return rc;
            }
            if (std::any_of(block, block + BLOCK_SIZE, [](uint32_t val) -> bool { return val != 0xffffffff; })) {
                // page isn't as empty after all, mark it as corrupted
                mState = PageState::CORRUPT;
                break;
            }
        }
        delete[] block;
    } else if (header.mCrc32 != header.calculateCrc32()) {
        header.mState = PageState::CORRUPT;
    } else {
        mState = header.mState;
        mSeqNumber = header.mSeqNumber;
        if(header.mVersion < NVS_VERSION) {
            return ESP_ERR_NVS_NEW_VERSION_FOUND;
        } else {
            mVersion = header.mVersion;
        }
    }

    switch (mState) {
    case PageState::UNINITIALIZED:
        break;

    case PageState::FULL:
    case PageState::ACTIVE:
    case PageState::FREEING:
        return mLoadEntryTable();
        break;

    default:
        mState = PageState::CORRUPT;
        break;
    }

    return ESP_OK;
}

esp_err_t Page::writeEntry(const Item& item)
{
    uint32_t phyAddr;
    esp_err_t err = getEntryAddress(mNextFreeEntry, &phyAddr);
    if (err != ESP_OK) {
        return err;
    }
    err = mPartition->write(phyAddr, &item, sizeof(item));


    if (err != ESP_OK) {
        mState = PageState::INVALID;
        return err;
    }

    err = alterEntryState(mNextFreeEntry, EntryState::WRITTEN);
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
    NVS_ASSERT_OR_RETURN(size % ENTRY_SIZE == 0, ESP_FAIL);
    NVS_ASSERT_OR_RETURN(mNextFreeEntry != INVALID_ENTRY, ESP_FAIL);
    NVS_ASSERT_OR_RETURN(mFirstUsedEntry != INVALID_ENTRY, ESP_FAIL);
    const uint16_t count = size / ENTRY_SIZE;

    uint32_t phyAddr;
    esp_err_t rc = getEntryAddress(mNextFreeEntry, &phyAddr);
    if (rc == ESP_OK) {
        rc = mPartition->write(phyAddr, data, size);
    }
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

esp_err_t Page::writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize, uint8_t chunkIdx)
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

    if (dataSize > Page::CHUNK_MAX_SIZE) {
        return ESP_ERR_NVS_VALUE_TOO_LONG;
    }

    if ((!isVariableLengthType(datatype)) && dataSize > 8) {
        return ESP_ERR_INVALID_ARG;
    }

    size_t totalSize = ENTRY_SIZE;
    size_t entriesCount = 1;
    if (isVariableLengthType(datatype)) {
        size_t roundedSize = (dataSize + ENTRY_SIZE - 1) & ~(ENTRY_SIZE - 1);
        totalSize += roundedSize;
        entriesCount += roundedSize / ENTRY_SIZE;
    }

    // primitive types should fit into one entry
    NVS_ASSERT_OR_RETURN(totalSize == ENTRY_SIZE ||
       isVariableLengthType(datatype), ESP_ERR_NVS_VALUE_TOO_LONG);

    if (mNextFreeEntry == INVALID_ENTRY || mNextFreeEntry + entriesCount > ENTRY_COUNT) {
        // page will not fit this amount of data
        return ESP_ERR_NVS_PAGE_FULL;
    }

    // write first item
    size_t span = (totalSize + ENTRY_SIZE - 1) / ENTRY_SIZE;
    item = Item(nsIndex, datatype, span, key, chunkIdx);
    err = mHashList.insert(item, mNextFreeEntry);

    if (err != ESP_OK) {
        return err;
    }

    if (!isVariableLengthType(datatype)) {
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
        item.varLength.reserved = 0xffff;
        item.crc32 = item.calculateCrc32();
        err = writeEntry(item);
        if (err != ESP_OK) {
            return err;
        }

        size_t rest = dataSize % ENTRY_SIZE;
        size_t left = dataSize - rest;
        if (left > 0) {
            err = writeEntryData(static_cast<const uint8_t*>(data), left);
            if (err != ESP_OK) {
                return err;
            }
        }

        size_t tail = rest;
        if (tail > 0) {
            std::fill_n(item.rawData, ENTRY_SIZE, 0xff);
            memcpy(item.rawData, static_cast<const uint8_t*>(data) + left, tail);
            err = writeEntry(item);
            if (err != ESP_OK) {
                return err;
            }
        }

    }
    return ESP_OK;
}

esp_err_t Page::readItem(uint8_t nsIndex, ItemType datatype, const char* key, void* data, size_t dataSize, uint8_t chunkIdx, VerOffset chunkStart)
{
    size_t index = 0;
    Item item;

    if (mState == PageState::INVALID) {
        return ESP_ERR_NVS_INVALID_STATE;
    }

    esp_err_t rc = findItem(nsIndex, datatype, key, index, item, chunkIdx, chunkStart);
    if (rc != ESP_OK) {
        return rc;
    }

    if (!isVariableLengthType(datatype)) {
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

esp_err_t Page::cmpItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize, uint8_t chunkIdx, VerOffset chunkStart)
{
    size_t index = 0;
    Item item;

    if (mState == PageState::INVALID) {
        return ESP_ERR_NVS_INVALID_STATE;
    }

    esp_err_t rc = findItem(nsIndex, datatype, key, index, item, chunkIdx, chunkStart);
    if (rc != ESP_OK) {
        return rc;
    }

    if (!isVariableLengthType(datatype)) {
        if (dataSize != getAlignmentForType(datatype)) {
            return ESP_ERR_NVS_TYPE_MISMATCH;
        }

        if (memcmp(data, item.data, dataSize)) {
            return ESP_ERR_NVS_CONTENT_DIFFERS;
        }
        return ESP_OK;
    }

    if (dataSize < static_cast<size_t>(item.varLength.dataSize)) {
        return ESP_ERR_NVS_INVALID_LENGTH;
    }

    const uint8_t* dst = reinterpret_cast<const uint8_t*>(data);
    size_t left = item.varLength.dataSize;
    for (size_t i = index + 1; i < index + item.span; ++i) {
        Item ditem;
        rc = readEntry(i, ditem);
        if (rc != ESP_OK) {
            return rc;
        }
        size_t willCopy = ENTRY_SIZE;
        willCopy = (left < willCopy)?left:willCopy;
        if (memcmp(dst, ditem.rawData, willCopy)) {
            return ESP_ERR_NVS_CONTENT_DIFFERS;
        }
        left -= willCopy;
        dst += willCopy;
    }
    if (Item::calculateCrc32(reinterpret_cast<const uint8_t*>(data), item.varLength.dataSize) != item.varLength.dataCrc32) {
        return ESP_ERR_NVS_NOT_FOUND;
    }

    return ESP_OK;
}

esp_err_t Page::eraseItem(uint8_t nsIndex, ItemType datatype, const char* key, uint8_t chunkIdx, VerOffset chunkStart)
{
    size_t index = 0;
    Item item;
    esp_err_t rc = findItem(nsIndex, datatype, key, index, item, chunkIdx, chunkStart);
    if (rc != ESP_OK) {
        return rc;
    }
    return eraseEntryAndSpan(index);
}

esp_err_t Page::findItem(uint8_t nsIndex, ItemType datatype, const char* key, uint8_t chunkIdx, VerOffset chunkStart)
{
    size_t index = 0;
    Item item;
    return findItem(nsIndex, datatype, key, index, item, chunkIdx, chunkStart);
}

esp_err_t Page::eraseEntryAndSpan(size_t index)
{
    uint32_t seq_num;
    getSeqNumber(seq_num);

    EntryState state;
    esp_err_t err = mEntryTable.get(index, &state);
    if (err != ESP_OK) {
        return err;
    }

    size_t span = 1;
    if (state == EntryState::WRITTEN) {
        Item item;
        auto rc = readEntry(index, item);
        if (rc != ESP_OK) {
            return rc;
        }
        if (item.calculateCrc32() != item.crc32) {
            mHashList.erase(index);
            rc = alterEntryState(index, EntryState::ERASED);
            --mUsedEntryCount;
            ++mErasedEntryCount;
            if (rc != ESP_OK) {
                return rc;
            }
        } else {
            mHashList.erase(index);
            span = item.span;
            for (ptrdiff_t i = index + span - 1; i >= static_cast<ptrdiff_t>(index); --i) {
                rc = mEntryTable.get(i, &state);
                if (rc != ESP_OK) {
                    return rc;
                }
                if (state == EntryState::WRITTEN) {
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
        auto rc = updateFirstUsedEntry(index, span);
        if (rc != ESP_OK) {
            return rc;
        }
    }

    if (index + span > mNextFreeEntry) {
        mNextFreeEntry = index + span;
    }

    return ESP_OK;
}

esp_err_t Page::updateFirstUsedEntry(size_t index, size_t span)
{
    NVS_ASSERT_OR_RETURN(index == mFirstUsedEntry, ESP_FAIL);
    mFirstUsedEntry = INVALID_ENTRY;
    size_t end = mNextFreeEntry;
    EntryState state;
    esp_err_t err;
    if (end > ENTRY_COUNT) {
        end = ENTRY_COUNT;
    }
    for (size_t i = index + span; i < end; ++i) {
        err = mEntryTable.get(i, &state);
        if (err != ESP_OK) {
            return err;
        }
        if (state == EntryState::WRITTEN) {
            mFirstUsedEntry = i;
            break;
        }
    }
    return ESP_OK;
}

esp_err_t Page::copyItems(Page& other)
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
    size_t readEntryIndex = mFirstUsedEntry;
    EntryState state;
    esp_err_t err;

    while (readEntryIndex < ENTRY_COUNT) {
        err = mEntryTable.get(readEntryIndex, &state);
        if (err != ESP_OK) {
            return err;
        }
        if (state != EntryState::WRITTEN) {
            NVS_ASSERT_OR_RETURN(readEntryIndex != mFirstUsedEntry, ESP_FAIL);
            readEntryIndex++;
            continue;
        }
        err = readEntry(readEntryIndex, entry);
        if (err != ESP_OK) {
            return err;
        }

        err = other.mHashList.insert(entry, other.mNextFreeEntry);
        if (err != ESP_OK) {
            return err;
        }

        err = other.writeEntry(entry);
        if (err != ESP_OK) {
            return err;
        }
        size_t span = entry.span;
        size_t end = readEntryIndex + span;

        NVS_ASSERT_OR_RETURN(end <= ENTRY_COUNT, ESP_FAIL);

        for (size_t i = readEntryIndex + 1; i < end; ++i) {
            readEntry(i, entry);
            err = other.writeEntry(entry);
            if (err != ESP_OK) {
                return err;
            }
        }
        readEntryIndex = end;

    }
    return ESP_OK;
}

esp_err_t Page::mLoadEntryTable()
{
    // for states where we actually care about data in the page, read entry state table
    if (mState == PageState::ACTIVE ||
            mState == PageState::FULL ||
            mState == PageState::FREEING) {
        auto rc = mPartition->read_raw(mBaseAddress + ENTRY_TABLE_OFFSET, mEntryTable.data(),
                                 mEntryTable.byteSize());
        if (rc != ESP_OK) {
            mState = PageState::INVALID;
            return rc;
        }
    }

    EntryState state;
    esp_err_t err;
    mErasedEntryCount = 0;
    mUsedEntryCount = 0;
    for (size_t i = 0; i < ENTRY_COUNT; ++i) {
        err = mEntryTable.get(i, &state);
        if (err != ESP_OK) {
            return err;
        }
        if (state == EntryState::WRITTEN) {
            if (mFirstUsedEntry == INVALID_ENTRY) {
                mFirstUsedEntry = i;
            }
            ++mUsedEntryCount;
        } else if (state == EntryState::ERASED) {
            ++mErasedEntryCount;
        }
    }

    // for PageState::ACTIVE, we may have more data written to this page
    // as such, we need to figure out where the first unused entry is
    if (mState == PageState::ACTIVE) {
        for (size_t i = 0; i < ENTRY_COUNT; ++i) {
            err = mEntryTable.get(i, &state);
            if (err != ESP_OK) {
                return err;
            }
            if (state == EntryState::EMPTY) {
                mNextFreeEntry = i;
                break;
            }
        }

        // however, if power failed after some data was written into the entry.
        // but before the entry state table was altered, the entry locacted via
        // entry state table may actually be half-written.
        // this is easy to check by reading EntryHeader (i.e. first word)
        while (mNextFreeEntry < ENTRY_COUNT) {
            uint32_t entryAddress;
            err = getEntryAddress(mNextFreeEntry, &entryAddress);
            if (err != ESP_OK) {
                return err;
            }
            uint32_t header;
            auto rc = mPartition->read_raw(entryAddress, &header, sizeof(header));
            if (rc != ESP_OK) {
                mState = PageState::INVALID;
                return rc;
            }
            if (header != 0xffffffff) {
                auto oldState = state;
                rc = mEntryTable.get(mNextFreeEntry, &oldState);
                if (rc != ESP_OK) {
                    return rc;
                }
                err = alterEntryState(mNextFreeEntry, EntryState::ERASED);
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
            err = mEntryTable.get(i, &state);
            if (err != ESP_OK) {
                return err;
            }
            if (state == EntryState::ERASED) {
                lastItemIndex = INVALID_ENTRY;
                continue;
            }

            if (state == EntryState::ILLEGAL) {
                lastItemIndex = INVALID_ENTRY;
                auto err = eraseEntryAndSpan(i);
                if (err != ESP_OK) {
                    mState = PageState::INVALID;
                    return err;
                }
                continue;
            }

            lastItemIndex = i;

            auto err = readEntry(i, item);
            if (err != ESP_OK) {
                mState = PageState::INVALID;
                return err;
            }

            if (item.crc32 != item.calculateCrc32()) {
                err = eraseEntryAndSpan(i);
                if (err != ESP_OK) {
                    mState = PageState::INVALID;
                    return err;
                }
                continue;
            }

            err = mHashList.insert(item, i);
            if (err != ESP_OK) {
                mState = PageState::INVALID;
                return err;
            }

            // search for potential duplicate item
            size_t duplicateIndex = mHashList.find(0, item);

            if (isVariableLengthType(item.datatype)) {
                span = item.span;
                bool needErase = false;
                for (size_t j = i; j < i + span; ++j) {
                    err = mEntryTable.get(j, &state);
                    if (err != ESP_OK) {
                        return err;
                    }
                    if (state != EntryState::WRITTEN) {
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

            /* Note that logic for duplicate detections works fine even
             * when old-format blob is present along with new-format blob-index
             * for same key on active page. Since datatype is not used in hash calculation,
             * old-format blob will be removed.*/
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
            auto err = mEntryTable.get(i, &state);
            if (err != ESP_OK) {
                return err;
            }
            if (state != EntryState::WRITTEN) {
                continue;
            }

            err = readEntry(i, item);
            if (err != ESP_OK) {
                mState = PageState::INVALID;
                return err;
            }

            if (item.crc32 != item.calculateCrc32()) {
                err = eraseEntryAndSpan(i);
                if (err != ESP_OK) {
                    mState = PageState::INVALID;
                    return err;
                }
                continue;
            }

            NVS_ASSERT_OR_RETURN(item.span > 0, ESP_FAIL);

            err = mHashList.insert(item, i);
            if (err != ESP_OK) {
                mState = PageState::INVALID;
                return err;
            }

            size_t span = item.span;

            if (isVariableLengthType(item.datatype)) {
                for (size_t j = i + 1; j < i + span; ++j) {
                    err = mEntryTable.get(j, &state);
                    if (err != ESP_OK) {
                        return err;
                    }
                    if (state != EntryState::WRITTEN) {
                        eraseEntryAndSpan(i);
                        break;
                    }
                }
            }

            i += span - 1;
        }

    }

    return ESP_OK;
}


esp_err_t Page::initialize()
{
    NVS_ASSERT_OR_RETURN(mState == PageState::UNINITIALIZED, ESP_FAIL);
    mState = PageState::ACTIVE;
    Header header;
    header.mState = mState;
    header.mSeqNumber = mSeqNumber;
    header.mVersion = mVersion;
    header.mCrc32 = header.calculateCrc32();

    auto rc = mPartition->write_raw(mBaseAddress, &header, sizeof(header));
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
    NVS_ASSERT_OR_RETURN(index < ENTRY_COUNT, ESP_FAIL);
    esp_err_t err = mEntryTable.set(index, state);
    if (err != ESP_OK) {
        return err;
    }
    size_t wordToWrite = mEntryTable.getWordIndex(index);
    uint32_t word = mEntryTable.data()[wordToWrite];
    err = mPartition->write_raw(mBaseAddress + ENTRY_TABLE_OFFSET + static_cast<uint32_t>(wordToWrite) * 4,
            &word, sizeof(word));
    if (err != ESP_OK) {
        mState = PageState::INVALID;
        return err;
    }
    return ESP_OK;
}

esp_err_t Page::alterEntryRangeState(size_t begin, size_t end, EntryState state)
{
    NVS_ASSERT_OR_RETURN(end <= ENTRY_COUNT, ESP_FAIL);
    NVS_ASSERT_OR_RETURN(end > begin, ESP_FAIL);
    size_t wordIndex = mEntryTable.getWordIndex(end - 1);
    esp_err_t err;
    for (ptrdiff_t i = end - 1; i >= static_cast<ptrdiff_t>(begin); --i) {
        err = mEntryTable.set(i, state);
        if (err != ESP_OK){
            return err;
        }
        size_t nextWordIndex;
        if (i == static_cast<ptrdiff_t>(begin)) {
            nextWordIndex = (size_t) -1;
        } else {
            nextWordIndex = mEntryTable.getWordIndex(i - 1);
        }
        if (nextWordIndex != wordIndex) {
            uint32_t word = mEntryTable.data()[wordIndex];
            auto rc = mPartition->write_raw(mBaseAddress + ENTRY_TABLE_OFFSET + static_cast<uint32_t>(wordIndex) * 4,
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
    auto rc = mPartition->write_raw(mBaseAddress, &state_val, sizeof(state));
    if (rc != ESP_OK) {
        mState = PageState::INVALID;
        return rc;
    }
    mState = (PageState) state;
    return ESP_OK;
}

esp_err_t Page::readEntry(size_t index, Item& dst) const
{
    uint32_t phyAddr;
    esp_err_t rc = getEntryAddress(index, &phyAddr);
    if (rc != ESP_OK) {
        return rc;
    }
    rc = mPartition->read(phyAddr, &dst, sizeof(dst));
    if (rc != ESP_OK) {
        return rc;
    }
    return ESP_OK;
}

esp_err_t Page::findItem(uint8_t nsIndex, ItemType datatype, const char* key, size_t &itemIndex, Item& item, uint8_t chunkIdx, VerOffset chunkStart)
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

    // For BLOB_DATA, we may need to search for all chunk indexes, so the hash list won't help
    // mHashIndex calculates hash from nsIndex, key, chunkIdx
    // We may not use mHashList if datatype is BLOB_DATA and chunkIdx is CHUNK_ANY as CHUNK_ANY is used by BLOB_INDEX
    if (nsIndex != NS_ANY && key != NULL && (datatype != ItemType::BLOB_DATA || chunkIdx != CHUNK_ANY)) {
        size_t cachedIndex = mHashList.find(start, Item(nsIndex, datatype, 0, key, chunkIdx));
        if (cachedIndex < ENTRY_COUNT) {
            start = cachedIndex;
        } else {
            return ESP_ERR_NVS_NOT_FOUND;
        }
    }

    size_t next;
    EntryState state;
    esp_err_t rc;
    for (size_t i = start; i < end; i = next) {
        next = i + 1;
        rc = mEntryTable.get(i, &state);
        if (rc != ESP_OK) {
            return rc;
        }
        if (state != EntryState::WRITTEN) {
            continue;
        }

        rc = readEntry(i, item);
        if (rc != ESP_OK) {
            mState = PageState::INVALID;
            return rc;
        }

        auto crc32 = item.calculateCrc32();
        if (item.crc32 != crc32) {
            rc = eraseEntryAndSpan(i);
            if (rc != ESP_OK) {
                mState = PageState::INVALID;
                return rc;
            }
            continue;
        }

        if (isVariableLengthType(item.datatype)) {
            next = i + item.span;
        }

        if (nsIndex != NS_ANY && item.nsIndex != nsIndex) {
            continue;
        }

        if (key != nullptr && strncmp(key, item.key, Item::MAX_KEY_LENGTH) != 0) {
            continue;
        }
        /* For blob data, chunkIndex should match*/
        if (chunkIdx != CHUNK_ANY
                && datatype == ItemType::BLOB_DATA
                && item.chunkIndex != chunkIdx) {
            continue;
        }

        // We may search for any chunk of BLOB_DATA but find BLOB_INDEX or BLOB instead as it
        // uses default value of chunkIdx == CHUNK_ANY, then continue searching
        if (chunkIdx == CHUNK_ANY
                && datatype == ItemType::BLOB_DATA
                && item.datatype != ItemType::BLOB_DATA) {
            continue;
        }

        // We may search for BLOB but find BLOB_INDEX instead
        // In this case it is expected to return ESP_ERR_NVS_TYPE_MISMATCH
        if (chunkIdx == CHUNK_ANY
                && datatype == ItemType::BLOB
                && item.datatype == ItemType::BLOB_IDX) {
            return ESP_ERR_NVS_TYPE_MISMATCH;
        }

        // We may search for BLOB but find BLOB_DATA instead
        // Then continue
        if (chunkIdx == CHUNK_ANY
                && datatype == ItemType::BLOB
                && item.datatype == ItemType::BLOB_DATA) {
            continue;
        }

        /* Blob-index will match the <ns,key> with blob data.
         * Skip data chunks when searching for blob index*/
        if (datatype == ItemType::BLOB_IDX
                && item.chunkIndex != CHUNK_ANY) {
            continue;
        }
        /* Match the version for blob-index*/
        if (datatype == ItemType::BLOB_IDX
                && chunkStart != VerOffset::VER_ANY
                && item.blobIndex.chunkStart != chunkStart) {
            continue;
        }


        if (datatype != ItemType::ANY && item.datatype != datatype) {
            if (key == nullptr && nsIndex == NS_ANY && chunkIdx == CHUNK_ANY) {
                continue; // continue for bruteforce search on blob indices.
            }
            itemIndex = i;
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

esp_err_t Page::setVersion(uint8_t ver)
{
    if (mState != PageState::UNINITIALIZED) {
        return ESP_ERR_NVS_INVALID_STATE;
    }
    mVersion = ver;
    return ESP_OK;
}

esp_err_t Page::erase()
{
    auto rc = mPartition->erase_range(mBaseAddress, SPI_FLASH_SEC_SIZE);
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

size_t Page::getVarDataTailroom() const
{
    if (mState == PageState::UNINITIALIZED) {
        return CHUNK_MAX_SIZE;
    } else if (mState == PageState::FULL) {
        return 0;
    }
    /* Skip one entry for blob data item processing the data */
    return ((mNextFreeEntry < (ENTRY_COUNT-1)) ? ((ENTRY_COUNT - mNextFreeEntry - 1) * ENTRY_SIZE): 0);
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
    printf("state=%" PRIx32 " (%s) addr=%" PRIx32 " seq=%" PRIu32 "\nfirstUsed=%" PRIu32 " nextFree=%" PRIu32 " used=%" PRIu16 " erased=%" PRIu16 "\n",
        static_cast<uint32_t>(mState), pageStateToName(mState), mBaseAddress, mSeqNumber, static_cast<uint32_t>(mFirstUsedEntry), static_cast<uint32_t>(mNextFreeEntry), mUsedEntryCount, mErasedEntryCount);
    size_t skip = 0;
    for (size_t i = 0; i < ENTRY_COUNT; ++i) {
        printf("%3d: ", static_cast<int>(i));
        EntryState state;
        if (mEntryTable.get(i, &state) != ESP_OK) {
            printf("Failed to read entry state\n");
            return;
        }
        if (state == EntryState::EMPTY) {
            printf("E\n");
        } else if (state == EntryState::ERASED) {
            printf("X\n");
        } else if (state == EntryState::WRITTEN) {
            Item item;
            readEntry(i, item);
            if (skip == 0) {
                printf("W ns=%2" PRIu8 " type=%2" PRIu8 " span=%3" PRIu8 " key=\"%s\" chunkIdx=%" PRIu8 " len=%" PRIi32 "\n",
                    item.nsIndex, static_cast<uint8_t>(item.datatype), item.span, item.key, item.chunkIndex, (item.span != 1)?(static_cast<int32_t>(item.varLength.dataSize)):(-1));
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

esp_err_t Page::calcEntries(nvs_stats_t &nvsStats)
{
    NVS_ASSERT_OR_RETURN(mState != PageState::FREEING, ESP_FAIL);

    nvsStats.total_entries += ENTRY_COUNT;

    switch (mState) {
        case PageState::UNINITIALIZED:
        case PageState::CORRUPT:
            nvsStats.free_entries += ENTRY_COUNT;
            break;

        case PageState::FULL:
        case PageState::ACTIVE:
            nvsStats.used_entries += mUsedEntryCount;
            nvsStats.free_entries += ENTRY_COUNT - mUsedEntryCount; // it's equivalent free + erase entries.
            break;

        case PageState::INVALID:
            return ESP_ERR_INVALID_STATE;
            break;

        default:
            assert(false && "Unhandled state");
            break;
    }
    return ESP_OK;
}

} // namespace nvs
