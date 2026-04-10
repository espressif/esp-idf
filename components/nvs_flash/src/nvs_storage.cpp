/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_storage.hpp"
#if __has_include(<bsd/string.h>)
// for strlcpy
#include <bsd/string.h>
#endif

#ifndef ESP_PLATFORM
// We need NO_DEBUG_STORAGE here since the integration tests on the host add some debug code.
// The unit tests, however, don't want debug code since they check the behavior via data in/output and disturb
// the order of calling mocked functions.
#ifndef NO_DEBUG_STORAGE
#include <map>
#include <sstream>
#define DEBUG_STORAGE
#endif
#endif // !ESP_PLATFORM

#include "esp_log.h"
#include "spi_flash_mmap.h"
#define TAG "nvs_storage"

#if defined(SEGGER_H) && defined(GLOBAL_H)
NVS_GUARD_SYSVIEW_MACRO_EXPANSION_PUSH();
#undef U8
#undef I8
#undef U16
#undef I16
#undef U32
#undef I32
#undef U64
#undef I64
#endif

namespace nvs
{

Storage::~Storage()
{
    clearNamespaces();
}

void Storage::clearNamespaces()
{
    mNamespaces.clearAndFreeNodes();
}

esp_err_t Storage::populateBlobIndices(TBlobIndexList& blobIdxList)
{
    for(auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;

        /* If the power went off just after writing a blob index, the duplicate detection
         * logic in pagemanager will remove the earlier index. So we should never find a
         * duplicate index at this point */

        while(p.findItem(Page::NS_ANY, ItemType::BLOB_IDX, nullptr, itemIndex, item) == ESP_OK) {
            BlobIndexNode* entry = new (std::nothrow) BlobIndexNode;

            if(!entry) return ESP_ERR_NO_MEM;

            item.getKey(entry->key, sizeof(entry->key));
            entry->nsIndex = item.nsIndex;
            entry->chunkStart = item.blobIndex.chunkStart;
            entry->chunkCount = item.blobIndex.chunkCount;
            entry->dataSize = item.blobIndex.dataSize;
            entry->observedDataSize = 0;
            entry->observedChunkCount = 0;

            blobIdxList.push_back(entry);
            itemIndex += item.span;
        }
    }

    return ESP_OK;
}

// Check BLOB_DATA entries belonging to BLOB_INDEX entries for mismatched records.
// BLOB_INDEX record is compared with information collected from BLOB_DATA records
// matched using namespace index, key and chunk version. Mismatched summary length
// or wrong number of chunks are checked. Mismatched BLOB_INDEX data are deleted
// and removed from the blobIdxList. The BLOB_DATA are left as orphans and removed
// later by the call to eraseOrphanDataBlobs().
void Storage::eraseMismatchedBlobIndexes(TBlobIndexList& blobIdxList)
{
    for(auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;
        /* Chunks with same <ns,key> and with chunkIndex in the following ranges
         * belong to same family.
         * 1) VER_0_OFFSET <= chunkIndex < VER_1_OFFSET-1 => Version0 chunks
         * 2) VER_1_OFFSET <= chunkIndex < VER_ANY => Version1 chunks
         */
        while(p.findItem(Page::NS_ANY, ItemType::BLOB_DATA, nullptr, itemIndex, item) == ESP_OK) {

            auto iter = std::find_if(blobIdxList.begin(),
                    blobIdxList.end(),
                    [=] (const BlobIndexNode& e) -> bool
                    {return (strncmp(item.key, e.key, sizeof(e.key) - 1) == 0)
                            && (item.nsIndex == e.nsIndex)
                            && (item.chunkIndex >=  static_cast<uint8_t> (e.chunkStart))
                            && (item.chunkIndex < static_cast<uint8_t> ((e.chunkStart == nvs::VerOffset::VER_0_OFFSET) ? nvs::VerOffset::VER_1_OFFSET : nvs::VerOffset::VER_ANY));});
            if(iter != std::end(blobIdxList)) {
                // accumulate the size
                iter->observedDataSize += item.varLength.dataSize;
                iter->observedChunkCount++;
            }
            itemIndex += item.span;
        }
    }

    auto iter = blobIdxList.begin();
    while(iter != blobIdxList.end())
    {
        if( (iter->observedDataSize != iter->dataSize) || (iter->observedChunkCount != iter->chunkCount) )
        {
            // Delete blob_index from flash
            // This is very rare case, so we can loop over all pages
            for(auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
                // skip pages in non eligible states
                if(it->state() == nvs::Page::PageState::CORRUPT
                    || it->state() == nvs::Page::PageState::INVALID
                    || it->state() == nvs::Page::PageState::UNINITIALIZED){
                    continue;
                }

                Page& p = *it;
                if(p.eraseItem(iter->nsIndex, nvs::ItemType::BLOB_IDX, iter->key, Page::DEFAULT_PURGE_AFTER_ERASE, 255, iter->chunkStart) == ESP_OK){
                    break;
                }
            }

            // Delete blob index from the blobIdxList
            auto tmp = iter;
            ++iter;
            blobIdxList.erase(tmp);
            delete (nvs::Storage::BlobIndexNode*)tmp;
        }
        else
        {
            // Blob index OK
            ++iter;
        }
    }
}

void Storage::eraseOrphanDataBlobs(TBlobIndexList& blobIdxList)
{
    for(auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;
        /* Chunks with same <ns,key> and with chunkIndex in the following ranges
         * belong to same family.
         * 1) VER_0_OFFSET <= chunkIndex < VER_1_OFFSET-1 => Version0 chunks
         * 2) VER_1_OFFSET <= chunkIndex < VER_ANY => Version1 chunks
         */
        while(p.findItem(Page::NS_ANY, ItemType::BLOB_DATA, nullptr, itemIndex, item) == ESP_OK) {

            auto iter = std::find_if(blobIdxList.begin(),
                    blobIdxList.end(),
                    [=] (const BlobIndexNode& e) -> bool
                    {return (strncmp(item.key, e.key, sizeof(e.key) - 1) == 0)
                            && (item.nsIndex == e.nsIndex)
                            && (item.chunkIndex >=  static_cast<uint8_t> (e.chunkStart))
                            && (item.chunkIndex < static_cast<uint8_t> (e.chunkStart) + e.chunkCount);});
            if(iter == std::end(blobIdxList)) {
                p.eraseItem(item.nsIndex, item.datatype, item.key, Page::DEFAULT_PURGE_AFTER_ERASE, item.chunkIndex);
            }

            itemIndex += item.span;
        }
    }
}

esp_err_t Storage::init(uint32_t baseSector, uint32_t sectorCount)
{
    auto err = mPageManager.load(mPartition, baseSector, sectorCount);
    if(err != ESP_OK) {
        mState = StorageState::INVALID;
        return err;
    }

    // load namespaces list
    clearNamespaces();
    std::fill_n(mNamespaceUsage.data(), mNamespaceUsage.byteSize() / 4, 0);
    for(auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;
        while(p.findItem(Page::NS_INDEX, ItemType::U8, nullptr, itemIndex, item) == ESP_OK) {
            NamespaceEntry* entry = new (std::nothrow) NamespaceEntry;

            if(!entry) {
                mState = StorageState::INVALID;
                return ESP_ERR_NO_MEM;
            }

            item.getKey(entry->mName, sizeof(entry->mName));
            err = item.getValue(entry->mIndex);
            if(err != ESP_OK) {
                delete entry;
                return err;
            }
            if(mNamespaceUsage.set(entry->mIndex, true) != ESP_OK) {
                delete entry;
                return ESP_FAIL;
            }
            mNamespaces.push_back(entry);
            itemIndex += item.span;
        }
    }
    if(mNamespaceUsage.set(0, true) != ESP_OK) {
        return ESP_FAIL;
    }
    if(mNamespaceUsage.set(255, true) != ESP_OK) {
        return ESP_FAIL;
    }

    // Populate list of multi-page index entries.
    TBlobIndexList blobIdxList;
    err = populateBlobIndices(blobIdxList);
    if(err != ESP_OK) {
        mState = StorageState::INVALID;
        return ESP_ERR_NO_MEM;
    }

    // remove blob indexes with mismatched blob data length or chunk count
    eraseMismatchedBlobIndexes(blobIdxList);

    // Remove the entries for which there is no parent multi-page index.
    eraseOrphanDataBlobs(blobIdxList);

    // Purge the blob index list
    blobIdxList.clearAndFreeNodes();

    mState = StorageState::ACTIVE;

#ifdef DEBUG_STORAGE
    debugCheck();
#endif
    return ESP_OK;
}

bool Storage::isValid() const
{
    return mState == StorageState::ACTIVE;
}

esp_err_t Storage::findItem(uint8_t nsIndex, ItemType datatype, const char* key, Page* &page, Item& item, uint8_t chunkIdx, VerOffset chunkStart, size_t* itemIndex)
{
    for(auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
        size_t tmpItemIndex = 0;
        auto err = it->findItem(nsIndex, datatype, key, tmpItemIndex, item, chunkIdx, chunkStart);
        if(err == ESP_OK) {
            page = it;
            if(itemIndex) {
                *itemIndex = tmpItemIndex;
            }
            return ESP_OK;
        }
    }
    return ESP_ERR_NVS_NOT_FOUND;
}

esp_err_t Storage::writeMultiPageBlob(uint8_t nsIndex, const char* key, const void* data, size_t dataSize, VerOffset chunkStart, const bool purgeAfterErase)
{
    uint8_t chunkCount = 0;
    TUsedPageList usedPages;
    size_t remainingSize = dataSize;
    size_t offset = 0;
    esp_err_t err = ESP_OK;

    /* Check how much maximum data can be accommodated**/
    uint32_t max_pages = mPageManager.getPageCount() - 1;

    if(max_pages > (Page::CHUNK_ANY-1)/2) {
       max_pages = (Page::CHUNK_ANY-1)/2;
    }

    if(dataSize > max_pages * Page::CHUNK_MAX_SIZE) {
        return ESP_ERR_NVS_VALUE_TOO_LONG;
    }

    do {
        Page& page = getCurrentPage();
        size_t tailroom = page.getVarDataTailroom();
        size_t chunkSize = 0;
        if(chunkCount == 0U && ((tailroom < dataSize) || (tailroom == 0 && dataSize == 0)) && tailroom < Page::CHUNK_MAX_SIZE/10) {
            /** This is the first chunk and tailroom is too small ***/
            if(page.state() != Page::PageState::FULL) {
                err = page.markFull();
                if(err != ESP_OK) {
                    return err;
                }
            }
            err = mPageManager.requestNewPage();
            if(err != ESP_OK) {
                return err;
            } else if(getCurrentPage().getVarDataTailroom() == tailroom) {
                /* We got the same page or we are not improving.*/
                return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
            } else {
                continue;
            }
        } else if(!tailroom) {
            err = ESP_ERR_NVS_NOT_ENOUGH_SPACE;
            break;
        }

        /* Split the blob into two and store the chunk of available size onto the current page */
        NVS_ASSERT_OR_RETURN(tailroom != 0, ESP_FAIL);

        chunkSize = (remainingSize > tailroom)? tailroom : remainingSize;
        remainingSize -= chunkSize;

        err = page.writeItem(nsIndex, ItemType::BLOB_DATA, key,
                static_cast<const uint8_t*> (data) + offset, chunkSize, static_cast<uint8_t> (chunkStart) + chunkCount);
        chunkCount++;

        if(err != ESP_OK) {
            NVS_ASSERT_OR_RETURN(err != ESP_ERR_NVS_PAGE_FULL, err);
            break;
        } else {
            UsedPageNode* node = new (std::nothrow) UsedPageNode();
            if(!node) {
                err = ESP_ERR_NO_MEM;
                break;
            }
            node->mPage = &page;
            usedPages.push_back(node);
            if(remainingSize || (tailroom - chunkSize) < Page::ENTRY_SIZE) {
                if(page.state() != Page::PageState::FULL) {
                    err = page.markFull();
                    if(err != ESP_OK) {
                        break;
                    }
                }
                err = mPageManager.requestNewPage();
                if(err != ESP_OK) {
                    break;
                }
            }
        }
        offset += chunkSize;
        if(!remainingSize) {
            /* All pages are stored. Now store the index.*/
            Item item;
            std::fill_n(item.data, sizeof(item.data), 0xff);
            item.blobIndex.dataSize = dataSize;
            item.blobIndex.chunkCount = chunkCount;
            item.blobIndex.chunkStart = chunkStart;

            err = getCurrentPage().writeItem(nsIndex, ItemType::BLOB_IDX, key, item.data, sizeof(item.data));
            NVS_ASSERT_OR_RETURN(err != ESP_ERR_NVS_PAGE_FULL, err);
            break;
        }
    } while(1);

    if(err != ESP_OK) {
        /* Anything failed, then we should erase all the written chunks*/
        int ii=0;
        for(auto it = std::begin(usedPages); it != std::end(usedPages); it++) {
            it->mPage->eraseItem(nsIndex, ItemType::BLOB_DATA, key, purgeAfterErase, ii++);
        }
    }
    usedPages.clearAndFreeNodes();
    return err;
}

// datatype BLOB is written as BLOB_INDEX and BLOB_DATA and is searched for previous value as BLOB_INDEX and/or BLOB
// datatype BLOB_INDEX and BLOB_DATA are not supported as input parameters, the layer above should always use BLOB
esp_err_t Storage::writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize, const bool purgeAfterErase)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    // pointer to the page where the existing item was found
    Page* findPage = nullptr;
    // index of the item in the page where the existing item was found
    size_t itemIndex = 0;
    // page sequence number helping to detect whether the page with old value was relocated during the new write
    uint32_t findPageSeqNumber = UINT32_MAX;

    // indicates the datatype representation match between the old value and the new one
    bool matchedTypePageFound = false;

    // contains the item with the old value, if found
    Item item;

    esp_err_t err = ESP_OK;

    // Try to find existing item with the same key and namespace index
    // We are performing the findItem with datatype specified (it is not ANY) to ensure the hash list lookup is done.
    if(datatype == ItemType::BLOB) {
        // Specific lookup if performed for input datatype BLOB. The searched datatype for exact match is BLOB_INDEX.
        // The BLOB_INDEX is used to store the metadata of the (originally typed) BLOB data in current V2 implementation.
        err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item, Page::CHUNK_ANY, VerOffset::VER_ANY, &itemIndex);
        if(err == ESP_OK && findPage != nullptr) {
            matchedTypePageFound = true;
        }
#ifdef CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY
        // In legacy mode, we also try to find the item as BLOB if not found as BLOB_INDEX.
        // In this mode, it is possible to have multiple active values under the same (logical) key.
        // For BLOBs (which may have different physical representations in V1 it is BLOB, in V2 it is BLOB_INDEX) it in turn means
        // that we have to check both datatypes to find the old value.
        // The general case for compatibility flag disabled is below and handles all datatypes including BLOB.
        // To save some cycles, we do not compile both findItem calls in this case.
        if(err == ESP_ERR_NVS_NOT_FOUND) {
            // If not found as BLOB_INDEX, try to find it as BLOB (legacy support).
            err = findItem(nsIndex, ItemType::BLOB, key, findPage, item, Page::CHUNK_ANY, VerOffset::VER_ANY, &itemIndex);
            if(err == ESP_OK && findPage != nullptr) {
                matchedTypePageFound = false;   // datatype does not match, we cannot extract chunkStart from the item

                // keep the sequence number of the page where the item was found for later check of relocation
                err = findPage->getSeqNumber(findPageSeqNumber);
                if(err != ESP_OK) {
                    return err;
                }
            }
        }
#endif
    } else {
        // Handle all other data types than BLOB
        err = findItem(nsIndex, datatype, key, findPage, item, Page::CHUNK_ANY, VerOffset::VER_ANY, &itemIndex);
        if(err == ESP_OK && findPage != nullptr) {
            matchedTypePageFound = true;

            // keep the sequence number of the page where the item was found for later check of relocation
            err = findPage->getSeqNumber(findPageSeqNumber);
            if(err != ESP_OK) {
                return err;
            }
        }
    }

#ifndef CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY
    // If the item was not found under assumed datatype, try to find it as ANY.
    if(findPage == nullptr) {

        // We should not find BLOB_DATA chunks as CHUNK_ANY is never used by the BLOB_DATA.
        err = findItem(nsIndex, nvs::ItemType::ANY, key, findPage, item, Page::CHUNK_ANY, VerOffset::VER_ANY, &itemIndex);
        if(err == ESP_OK && findPage != nullptr) {
            // keep the sequence number of the page where the item was found for later check of relocation
            err = findPage->getSeqNumber(findPageSeqNumber);
            if(err != ESP_OK) {
                return err;
            }
            // item was found with the same key and namespace index but data type is different
            matchedTypePageFound = false;
        }
    }
#endif

    // Here the findPage is either nullptr or points to the page where the item was found.
    // The matchedTypePageFound is true if the old value item was found and its datatype representation matches the new one.
    // This flag is used to determine if the item should be checked for same value.
    if(err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        return err;
    }

    // Handle value update
    if(datatype == ItemType::BLOB) {
        VerOffset prevStart,  nextStart;
        prevStart = nextStart = VerOffset::VER_0_OFFSET;
        if(matchedTypePageFound) {
            // Do a check that the item in question is actually being modified.
            // If it isn't, it is cheaper to purposefully not write out new data.
            // since it may invoke an erasure of flash.
            if(cmpMultiPageBlob(nsIndex, key, data, dataSize) == ESP_OK) {
                return ESP_OK;
            }

            // Get the version of the previous index with same <ns,key>
            prevStart = item.blobIndex.chunkStart;
            NVS_ASSERT_OR_RETURN(prevStart == VerOffset::VER_0_OFFSET || prevStart == VerOffset::VER_1_OFFSET, ESP_FAIL);

            // Toggle the version by changing the offset
            nextStart
                = (prevStart == VerOffset::VER_1_OFFSET) ? VerOffset::VER_0_OFFSET : VerOffset::VER_1_OFFSET;
        }
        // Write the blob with new version
        err = writeMultiPageBlob(nsIndex, key, data, dataSize, nextStart, purgeAfterErase);

        if(err == ESP_ERR_NVS_PAGE_FULL) {
            return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
        }

        if(err != ESP_OK) {
            return err;
        }
    } else {
        // Do a check that the item in question is actually being modified.
        // If it isn't, it is cheaper to purposefully not write out new data.
        // since it may invoke an erasure of flash.
        if(matchedTypePageFound &&
                findPage->cmpItem(nsIndex, datatype, key, data, dataSize) == ESP_OK) {
            return ESP_OK;
        }

        Page& page = getCurrentPage();
        err = page.writeItem(nsIndex, datatype, key, data, dataSize);
        if(err == ESP_ERR_NVS_PAGE_FULL) {
            if(page.state() != Page::PageState::FULL) {
                err = page.markFull();
                if(err != ESP_OK) {
                    return err;
                }
            }
            err = mPageManager.requestNewPage();
            if(err != ESP_OK) {
                return err;
            }

            err = getCurrentPage().writeItem(nsIndex, datatype, key, data, dataSize);
            if(err == ESP_ERR_NVS_PAGE_FULL) {
                return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
            }
        }

        if(err != ESP_OK) {
            return err;
        }
    }

    // Delete previous value
    // Note: The old entry won't be deleted if the new value is the same as the old value - code won't reach here in that case.

    // If findPage is null then previous value was not present in NVS and nothig is to be deleted.
    if(findPage == nullptr) {
        return err;
    }

    if(item.datatype == ItemType::BLOB_IDX) {
        // If the item found was BLOB_INDEX, the eraseMultiPageBlob is used to erase the whole multi-page blob.
        // It is not necessary to check the potential page relocation as the function will find the blob again anyway.
        VerOffset prevStart = item.blobIndex.chunkStart;
        err = eraseMultiPageBlob(nsIndex, key, purgeAfterErase, prevStart);

        if(err == ESP_ERR_FLASH_OP_FAIL) {
            return ESP_ERR_NVS_REMOVE_FAILED;
        }
    } else {
        // For all other data types, we have to check the potential page relocation.

        // The findPage might have been relocated as a part of space reclaiming.
        // First indication is the page state. It might become the "spare" page thus changing the state from FULL or ACTIVE.
        bool wasRelocated = false;

        if( findPage->state() != Page::PageState::ACTIVE &&
            findPage->state() != Page::PageState::FULL) {
                wasRelocated = true;
        }

        // Other indication of the multi step relocation is the page sequence number. If the sequence number is different than page
        // sequence number at the moment initial item was found, the page was relocated.
        if(!wasRelocated) {
            uint32_t newPageSeqNumber;
            err = findPage->getSeqNumber(newPageSeqNumber);
            if(err != ESP_OK) {
                return err;
            }
            if(newPageSeqNumber != findPageSeqNumber) {
                wasRelocated = true;
            }
        }

        if(wasRelocated) {
            // The page was relocated. We have to find the old value again from the beginning.
            // As the item was already found before relocation, we can use the exact datatype from item.
            err = findItem(nsIndex, item.datatype, key, findPage, item, Page::CHUNK_ANY, VerOffset::VER_ANY, &itemIndex);
            if(err != ESP_OK) {
                return err;
            }
        }

        // Page containing the old value is now refreshed. We can erase the old value.
        err = findPage->eraseEntryAndSpan(itemIndex, purgeAfterErase);
        if(err == ESP_ERR_FLASH_OP_FAIL) {
            return ESP_ERR_NVS_REMOVE_FAILED;
        }
    }

#ifdef DEBUG_STORAGE
    debugCheck();
#endif
    return err;
}

esp_err_t Storage::createOrOpenNamespace(const char* nsName, bool canCreate, uint8_t& nsIndex)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }
    auto it = std::find_if(mNamespaces.begin(), mNamespaces.end(), [=] (const NamespaceEntry& e) -> bool {
        return strncmp(nsName, e.mName, sizeof(e.mName) - 1) == 0;
    });
    if(it == std::end(mNamespaces)) {
        if(!canCreate) {
            return ESP_ERR_NVS_NOT_FOUND;
        }

        uint8_t ns;
        bool ns_state;
        for(ns = 1; ns < 255; ++ns) {
            if(mNamespaceUsage.get(ns, &ns_state) != ESP_OK) {
                return ESP_FAIL;
            }
            if(!ns_state) {
                break;
            }
        }

        if(ns == 255) {
            return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
        }

        auto err = writeItem(Page::NS_INDEX, ItemType::U8, nsName, &ns, sizeof(ns), Page::DEFAULT_PURGE_AFTER_ERASE);
        if(err != ESP_OK) {
            return err;
        }
        if(mNamespaceUsage.set(ns, true) != ESP_OK) {
            return ESP_FAIL;
        }
        nsIndex = ns;

        NamespaceEntry* entry = new (std::nothrow) NamespaceEntry;
        if(!entry) {
            return ESP_ERR_NO_MEM;
        }

        entry->mIndex = ns;
        strncpy(entry->mName, nsName, sizeof(entry->mName) - 1);
        entry->mName[sizeof(entry->mName) - 1] = 0;
        mNamespaces.push_back(entry);

    } else {
        nsIndex = it->mIndex;
    }
    return ESP_OK;
}

esp_err_t Storage::readMultiPageBlob(uint8_t nsIndex, const char* key, void* data, size_t dataSize)
{
    Item item;
    Page* findPage = nullptr;
    size_t itemIndex = 0;


    // First read the blob index
    auto err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item);
    if(err != ESP_OK) {
        return err;
    }

    uint8_t chunkCount = item.blobIndex.chunkCount;
    VerOffset chunkStart = item.blobIndex.chunkStart;
    size_t offset = 0;

    NVS_ASSERT_OR_RETURN(dataSize == item.blobIndex.dataSize, ESP_FAIL);

    // Now read related blob data chunks
    // Remember the itemIndex as it is used to fast locate the entry in the page
    for(uint8_t chunkNum = 0; chunkNum < chunkCount; chunkNum++) {
        err = findItem(nsIndex, ItemType::BLOB_DATA, key, findPage, item, static_cast<uint8_t> (chunkStart) + chunkNum, nvs::VerOffset::VER_ANY, &itemIndex);
        if(err != ESP_OK) {
            if(err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            return err;
        }

        // Check if the blob data chunk length indicated for actual item still fits into the total length of the buffer
        if(item.varLength.dataSize > dataSize - offset) {
            err = ESP_ERR_NVS_INVALID_LENGTH;
            break;
        }

        err = findPage->readVariableLengthItemData(item, itemIndex, static_cast<uint8_t*>(data) + offset);
        if(err != ESP_OK) {
            return err;
        }

        NVS_ASSERT_OR_RETURN(static_cast<uint8_t> (chunkStart) + chunkNum == item.chunkIndex, ESP_FAIL);

        offset += item.varLength.dataSize;
    }

    if(err == ESP_ERR_NVS_NOT_FOUND || err == ESP_ERR_NVS_INVALID_LENGTH) {
        // cleanup if a chunk is not found or the size is inconsistent
        eraseMultiPageBlob(nsIndex, key, Page::DEFAULT_PURGE_AFTER_ERASE);
    }

    NVS_ASSERT_OR_RETURN(offset == dataSize, ESP_FAIL);

    return err;
}

esp_err_t Storage::cmpMultiPageBlob(uint8_t nsIndex, const char* key, const void* data, size_t dataSize)
{
    Item item;
    Page* findPage = nullptr;
    size_t itemIndex = 0;

    // First read the blob index
    auto err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item);
    if(err != ESP_OK) {
        return err;
    }

    uint8_t chunkCount = item.blobIndex.chunkCount;
    VerOffset chunkStart = item.blobIndex.chunkStart;
    size_t readSize = item.blobIndex.dataSize;
    size_t offset = 0;

    if(dataSize != readSize) {
        return ESP_ERR_NVS_CONTENT_DIFFERS;
    }

    // Now read corresponding chunks
    for(uint8_t chunkNum = 0; chunkNum < chunkCount; chunkNum++) {
        err = findItem(nsIndex, ItemType::BLOB_DATA, key, findPage, item, static_cast<uint8_t> (chunkStart) + chunkNum, nvs::VerOffset::VER_ANY, &itemIndex);
        if(err != ESP_OK) {
            if(err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            return err;
        }

        if(item.varLength.dataSize > dataSize - offset) {
            // The size of the entry in the index is bigger than the size of the remaining data to be compared
            return ESP_ERR_NVS_CONTENT_DIFFERS;
        }

        // calculate crc32 of the incoming data window related to the BLOB_DATA chunk and compare it with the crc32 from the BLOB_DATA metadata entry
        // Different crc32 indicates data mismatch.
        // If crc32 matches, we have to compare the data in the chunk with the buffer data to exclude crc32 collision.
        if (Item::calculateCrc32(reinterpret_cast<const uint8_t * >(data), item.varLength.dataSize) != item.varLength.dataCrc32) {
            return ESP_ERR_NVS_CONTENT_DIFFERS;
        }

        err = findPage->cmpItem(nsIndex, ItemType::BLOB_DATA, key, static_cast<const uint8_t*>(data) + offset, item.varLength.dataSize, static_cast<uint8_t> (chunkStart) + chunkNum);
        if(err != ESP_OK) {
            return err;
        }
        NVS_ASSERT_OR_RETURN(static_cast<uint8_t> (chunkStart) + chunkNum == item.chunkIndex, ESP_FAIL);

        offset += item.varLength.dataSize;
    }
    NVS_ASSERT_OR_RETURN(offset == dataSize, ESP_FAIL);

    return err;
}

esp_err_t Storage::readItem(uint8_t nsIndex, ItemType datatype, const char* key, void* data, size_t dataSize)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    if(datatype == ItemType::BLOB) {
        auto err = readMultiPageBlob(nsIndex, key, data, dataSize);
        if(err != ESP_ERR_NVS_NOT_FOUND) {
            return err;
        } // else check if the blob is stored with earlier version format without index
    }

    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if(err != ESP_OK) {
        return err;
    }
    return findPage->readItem(nsIndex, datatype, key, data, dataSize);

}

esp_err_t Storage::eraseMultiPageBlob(uint8_t nsIndex, const char* key, const bool purgeAfterErase, VerOffset chunkStart)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }
    Item item;
    Page* findPage = nullptr;
    size_t itemIndex = 0;
    uint8_t chunkCount = 0;

    auto err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item, Page::CHUNK_ANY, chunkStart, &itemIndex);
    if(err != ESP_OK) {
        return err;
    }

    chunkCount = item.blobIndex.chunkCount;

    // Erase the index first and make children blobs orphan
    err = findPage->eraseEntryAndSpan(itemIndex, purgeAfterErase);
    if(err != ESP_OK) {
        return err;
    }

    // If caller requires delete of VER_ANY
    // We may face dirty NVS partition and version duplicates can be there
    // Make second attempt to delete index and ignore eventual not found
    if(chunkStart == VerOffset::VER_ANY) {
        // Specific case called during initialisation of the storage
        // We need to delete all chunks with the same key and namespace index

        // If there exists another BLOB_IDX with the same key and namespace index, delete it
        // Ignore potential error if the item is not found
        err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item, Page::CHUNK_ANY, chunkStart, &itemIndex);
        if(err == ESP_OK) {
            err = findPage->eraseEntryAndSpan(itemIndex, purgeAfterErase);
            if(err != ESP_OK) {
                return err;
            }
        } else if(err != ESP_ERR_NVS_NOT_FOUND) {
            return err;
        }

        // To delete all chunks, we will visit every page and delete all chunks regardless of chunkIndex
        // This approach cannot use the hash list as the chunkIndex is not known.
        for(auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
            // reset itemIndex to zero for each page to search from the beginning
            itemIndex = 0;
            do {
                // (Re)Try to find the item at the position starting at the itemIndex
                err = it->findItem(nsIndex, ItemType::BLOB_DATA, key, itemIndex, item);

                // If the item is not found, we can break the actual loop and continue with the next page
                if(err == ESP_ERR_NVS_NOT_FOUND) {
                    break;
                } else if(err == ESP_OK) {
                    err = it->eraseEntryAndSpan(itemIndex, purgeAfterErase);

                    // advance itemIndex to the next potential entry on the page
                    // findItem checks the consistency of the entry metadata so we can safely assume the span is non-zero
                    itemIndex += item.span;
                }
                if(err != ESP_OK) {
                    return err;
                }
            // Continue the loop until all items on the current page are found and erased
            } while(err == ESP_OK && itemIndex < Page::ENTRY_COUNT);
        }

    } else {
        // Most common condition
        // The caller has specified the chunk version, delete all chunks within the chunk index range indicated by the BLOB_IDX entry
        // The loop will iterate the chunk index, page will be found and chunk index will be erased
        // This approach uses the hash list to find the item on the page, so it is efficient.
        uint8_t minChunkIndex = (uint8_t) VerOffset::VER_ANY;
        uint8_t maxChunkIndex = (uint8_t) VerOffset::VER_ANY;

        if(chunkStart == VerOffset::VER_0_OFFSET) {
            minChunkIndex = (uint8_t) VerOffset::VER_0_OFFSET;
            maxChunkIndex = minChunkIndex + chunkCount;
        } else if(chunkStart == VerOffset::VER_1_OFFSET) {
            minChunkIndex = (uint8_t) VerOffset::VER_1_OFFSET;
            maxChunkIndex = minChunkIndex + chunkCount;
        }

        for(uint8_t chunkIndex = minChunkIndex; chunkIndex < maxChunkIndex; chunkIndex++) {
            err = findItem(nsIndex, ItemType::BLOB_DATA, key, findPage, item, chunkIndex, nvs::VerOffset::VER_ANY, &itemIndex);
            if(err != ESP_OK) {
                return err;
            }

            // Erase the entry
            err = findPage->eraseEntryAndSpan(itemIndex, purgeAfterErase);
            if(err != ESP_OK) {
                return err;
            }
        }
    }

    return ESP_OK;
}

esp_err_t Storage::eraseItem(uint8_t nsIndex, ItemType datatype, const char* key, const bool purgeAfterErase)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    esp_err_t err = ESP_OK;
    size_t itemIndex = 0;

    err = findItem(nsIndex, datatype, key, findPage, item, Page::CHUNK_ANY, VerOffset::VER_ANY, &itemIndex);
    if(err != ESP_OK) {
        return err;
    }
    // If the item found is BLOB_IDX, the eraseMultiPageBlob is used to erase the whole multi-page blob.
    if (item.datatype == ItemType::BLOB_IDX) {
        return eraseMultiPageBlob(nsIndex, key, purgeAfterErase, item.blobIndex.chunkStart);
    }

    return findPage->eraseEntryAndSpan(itemIndex, purgeAfterErase);
}

esp_err_t Storage::eraseNamespace(uint8_t nsIndex, const bool purgeAfterErase)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    for(auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
        while(true) {
            auto err = it->eraseItem(nsIndex, ItemType::ANY, nullptr, purgeAfterErase);
            if(err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            else if(err != ESP_OK) {
                return err;
            }
        }
    }
    return ESP_OK;
}

esp_err_t Storage::purgeNamespace(uint8_t nsIndex){
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    for(auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
        auto err = it->purgeErasedItems(nsIndex);
        if(err != ESP_OK) {
            return err;
        }
    }
    return ESP_OK;
}

esp_err_t Storage::findKey(const uint8_t nsIndex, const char* key, ItemType* datatype)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    auto err = findItem(nsIndex, ItemType::ANY, key, findPage, item);
    if(err != ESP_OK) {
        return err;
    }

    if(datatype != nullptr) {
        *datatype = item.datatype;
    }

    return err;
}

esp_err_t Storage::getItemDataSize(uint8_t nsIndex, ItemType datatype, const char* key, size_t& dataSize)
{
    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    esp_err_t err = ESP_OK;

    // If requested datatype is BLOB, first try to find the item with datatype BLOB_IDX - new format
    // If not found, try to find the item with datatype BLOB - old format.
    if(datatype == ItemType::BLOB) {
        err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item);
        if(err == ESP_OK) {
            dataSize = item.blobIndex.dataSize;
            return err;
        } else if(err != ESP_ERR_NVS_NOT_FOUND) {
            return err;
        }
    }

    err = findItem(nsIndex, datatype, key, findPage, item);
    if(err != ESP_OK) {
        return err;
    }
    dataSize = item.varLength.dataSize;
    return ESP_OK;
}

void Storage::debugDump()
{
    for(auto p = mPageManager.begin(); p != mPageManager.end(); ++p) {
        p->debugDump();
    }
}

#ifdef DEBUG_STORAGE
void Storage::debugCheck()
{
    std::map<std::string, Page*> keys;

    for(auto p = mPageManager.begin(); p != mPageManager.end(); ++p) {
        size_t itemIndex = 0;
        size_t usedCount = 0;
        Item item;
        while(p->findItem(Page::NS_ANY, ItemType::ANY, nullptr, itemIndex, item) == ESP_OK) {
            std::stringstream keyrepr;
            keyrepr << static_cast<unsigned>(item.nsIndex) << "_" << static_cast<unsigned>(item.datatype) << "_" << item.key <<"_"<<static_cast<unsigned>(item.chunkIndex);
            std::string keystr = keyrepr.str();
            if(keys.find(keystr) != std::end(keys)) {
                printf("Duplicate key: %s\n", keystr.c_str());
                debugDump();
                assert(0);
            }
            keys.insert(std::make_pair(keystr, static_cast<Page*>(p)));
            itemIndex += item.span;
            usedCount += item.span;
        }
        assert(usedCount == p->getUsedEntryCount());
    }
}
#endif //DEBUG_STORAGE

esp_err_t Storage::fillStats(nvs_stats_t& nvsStats)
{
    nvsStats.namespace_count = mNamespaces.size();
    return mPageManager.fillStats(nvsStats);
}

esp_err_t Storage::calcEntriesInNamespace(uint8_t nsIndex, size_t& usedEntries)
{
    usedEntries = 0;

    if(mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    for(auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
        size_t itemIndex = 0;
        Item item;
        while(true) {
            auto err = it->findItem(nsIndex, ItemType::ANY, nullptr, itemIndex, item);
            if(err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            else if(err != ESP_OK) {
                return err;
            }
            usedEntries += item.span;
            itemIndex   += item.span;
            if(itemIndex >= it->ENTRY_COUNT) break;
        }
    }
    return ESP_OK;
}

void Storage::fillEntryInfo(Item &item, nvs_entry_info_t &info)
{
    info.type = static_cast<nvs_type_t>(item.datatype);
    strncpy(info.key, item.key, sizeof(info.key) - 1);
    info.key[sizeof(info.key) - 1] = '\0';

    for(auto &name : mNamespaces) {
        if(item.nsIndex == name.mIndex) {
            strlcpy(info.namespace_name, name.mName, sizeof(info.namespace_name));
            break;
        }
    }
}

bool Storage::findEntry(nvs_opaque_iterator_t* it, const char* namespace_name)
{
    it->entryIndex = 0;
    it->nsIndex = Page::NS_ANY;
    it->page = mPageManager.begin();

    if(namespace_name != nullptr) {
        if(createOrOpenNamespace(namespace_name, false, it->nsIndex) != ESP_OK) {
            return false;
        }
    }

    return nextEntry(it);
}

bool Storage::findEntryNs(nvs_opaque_iterator_t* it, uint8_t nsIndex)
{
    it->entryIndex = 0;
    it->nsIndex = nsIndex;
    it->page = mPageManager.begin();

    return nextEntry(it);
}

inline bool isIterableItem(Item& item)
{
    return (item.nsIndex != 0 &&
            item.datatype != ItemType::BLOB &&
            item.datatype != ItemType::BLOB_IDX);
}

inline bool isMultipageBlob(Item& item)
{
    return (item.datatype == ItemType::BLOB_DATA &&
            !(item.chunkIndex == static_cast<uint8_t>(VerOffset::VER_0_OFFSET)
                    || item.chunkIndex == static_cast<uint8_t>(VerOffset::VER_1_OFFSET)));
}

bool Storage::nextEntry(nvs_opaque_iterator_t* it)
{
    Item item;
    esp_err_t err;

    for(auto page = it->page; page != mPageManager.end(); ++page) {
        do {
            err = page->findItem(it->nsIndex, (ItemType)it->type, nullptr, it->entryIndex, item);
            it->entryIndex += item.span;
            if(err == ESP_OK && isIterableItem(item) && !isMultipageBlob(item)) {
                fillEntryInfo(item, it->entry_info);
                it->page = page;
                return true;
            }
        } while(err != ESP_ERR_NVS_NOT_FOUND);

        it->entryIndex = 0;
    }

    return false;
}


}

#if defined(SEGGER_H) && defined(GLOBAL_H)
NVS_GUARD_SYSVIEW_MACRO_EXPANSION_POP();
#endif
