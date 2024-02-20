/*
 * SPDX-FileCopyrightText: 2015-2023 Espressif Systems (Shanghai) CO LTD
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
    for (auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;

        /* If the power went off just after writing a blob index, the duplicate detection
         * logic in pagemanager will remove the earlier index. So we should never find a
         * duplicate index at this point */

        while (p.findItem(Page::NS_ANY, ItemType::BLOB_IDX, nullptr, itemIndex, item) == ESP_OK) {
            BlobIndexNode* entry = new (std::nothrow) BlobIndexNode;

            if (!entry) return ESP_ERR_NO_MEM;

            item.getKey(entry->key, sizeof(entry->key));
            entry->nsIndex = item.nsIndex;
            entry->chunkStart = item.blobIndex.chunkStart;
            entry->chunkCount = item.blobIndex.chunkCount;

            blobIdxList.push_back(entry);
            itemIndex += item.span;
        }
    }

    return ESP_OK;
}

void Storage::eraseOrphanDataBlobs(TBlobIndexList& blobIdxList)
{
    for (auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;
        /* Chunks with same <ns,key> and with chunkIndex in the following ranges
         * belong to same family.
         * 1) VER_0_OFFSET <= chunkIndex < VER_1_OFFSET-1 => Version0 chunks
         * 2) VER_1_OFFSET <= chunkIndex < VER_ANY => Version1 chunks
         */
        while (p.findItem(Page::NS_ANY, ItemType::BLOB_DATA, nullptr, itemIndex, item) == ESP_OK) {

            auto iter = std::find_if(blobIdxList.begin(),
                    blobIdxList.end(),
                    [=] (const BlobIndexNode& e) -> bool
                    {return (strncmp(item.key, e.key, sizeof(e.key) - 1) == 0)
                            && (item.nsIndex == e.nsIndex)
                            && (item.chunkIndex >=  static_cast<uint8_t> (e.chunkStart))
                            && (item.chunkIndex < static_cast<uint8_t> (e.chunkStart) + e.chunkCount);});
            if (iter == std::end(blobIdxList)) {
                p.eraseItem(item.nsIndex, item.datatype, item.key, item.chunkIndex);
            }
            itemIndex += item.span;
        }
    }
}

esp_err_t Storage::init(uint32_t baseSector, uint32_t sectorCount)
{
    auto err = mPageManager.load(mPartition, baseSector, sectorCount);
    if (err != ESP_OK) {
        mState = StorageState::INVALID;
        return err;
    }

    // load namespaces list
    clearNamespaces();
    std::fill_n(mNamespaceUsage.data(), mNamespaceUsage.byteSize() / 4, 0);
    for (auto it = mPageManager.begin(); it != mPageManager.end(); ++it) {
        Page& p = *it;
        size_t itemIndex = 0;
        Item item;
        while (p.findItem(Page::NS_INDEX, ItemType::U8, nullptr, itemIndex, item) == ESP_OK) {
            NamespaceEntry* entry = new (std::nothrow) NamespaceEntry;

            if (!entry) {
                mState = StorageState::INVALID;
                return ESP_ERR_NO_MEM;
            }

            item.getKey(entry->mName, sizeof(entry->mName));
            err = item.getValue(entry->mIndex);
            if (err != ESP_OK) {
                delete entry;
                return err;
            }
            if (mNamespaceUsage.set(entry->mIndex, true) != ESP_OK) {
                delete entry;
                return ESP_FAIL;
            }
            mNamespaces.push_back(entry);
            itemIndex += item.span;
        }
    }
    if (mNamespaceUsage.set(0, true) != ESP_OK) {
        return ESP_FAIL;
    }
    if (mNamespaceUsage.set(255, true) != ESP_OK) {
        return ESP_FAIL;
    }
    mState = StorageState::ACTIVE;

    // Populate list of multi-page index entries.
    TBlobIndexList blobIdxList;
    err = populateBlobIndices(blobIdxList);
    if (err != ESP_OK) {
        mState = StorageState::INVALID;
        return ESP_ERR_NO_MEM;
    }

    // Remove the entries for which there is no parent multi-page index.
    eraseOrphanDataBlobs(blobIdxList);

    // Purge the blob index list
    blobIdxList.clearAndFreeNodes();

#ifdef DEBUG_STORAGE
    debugCheck();
#endif
    return ESP_OK;
}

bool Storage::isValid() const
{
    return mState == StorageState::ACTIVE;
}

esp_err_t Storage::findItem(uint8_t nsIndex, ItemType datatype, const char* key, Page* &page, Item& item, uint8_t chunkIdx, VerOffset chunkStart)
{
    for (auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
        size_t itemIndex = 0;
        auto err = it->findItem(nsIndex, datatype, key, itemIndex, item, chunkIdx, chunkStart);
        if (err == ESP_OK) {
            page = it;
            return ESP_OK;
        }
    }
    return ESP_ERR_NVS_NOT_FOUND;
}

esp_err_t Storage::writeMultiPageBlob(uint8_t nsIndex, const char* key, const void* data, size_t dataSize, VerOffset chunkStart)
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

    if (dataSize > max_pages * Page::CHUNK_MAX_SIZE) {
        return ESP_ERR_NVS_VALUE_TOO_LONG;
    }

    do {
        Page& page = getCurrentPage();
        size_t tailroom = page.getVarDataTailroom();
        size_t chunkSize = 0;
        if (chunkCount == 0U && ((tailroom < dataSize) || (tailroom == 0 && dataSize == 0)) && tailroom < Page::CHUNK_MAX_SIZE/10) {
            /** This is the first chunk and tailroom is too small ***/
            if (page.state() != Page::PageState::FULL) {
                err = page.markFull();
                if (err != ESP_OK) {
                    return err;
                }
            }
            err = mPageManager.requestNewPage();
            if (err != ESP_OK) {
                return err;
            } else if(getCurrentPage().getVarDataTailroom() == tailroom) {
                /* We got the same page or we are not improving.*/
                return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
            } else {
                continue;
            }
        } else if (!tailroom) {
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

        if (err != ESP_OK) {
            NVS_ASSERT_OR_RETURN(err != ESP_ERR_NVS_PAGE_FULL, err);
            break;
        } else {
            UsedPageNode* node = new (std::nothrow) UsedPageNode();
            if (!node) {
                err = ESP_ERR_NO_MEM;
                break;
            }
            node->mPage = &page;
            usedPages.push_back(node);
            if (remainingSize || (tailroom - chunkSize) < Page::ENTRY_SIZE) {
                if (page.state() != Page::PageState::FULL) {
                    err = page.markFull();
                    if (err != ESP_OK) {
                        break;
                    }
                }
                err = mPageManager.requestNewPage();
                if (err != ESP_OK) {
                    break;
                }
            }
        }
        offset += chunkSize;
        if (!remainingSize) {
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
    } while (1);

    if (err != ESP_OK) {
        /* Anything failed, then we should erase all the written chunks*/
        int ii=0;
        for (auto it = std::begin(usedPages); it != std::end(usedPages); it++) {
            it->mPage->eraseItem(nsIndex, ItemType::BLOB_DATA, key, ii++);
        }
    }
    usedPages.clearAndFreeNodes();
    return err;
}

esp_err_t Storage::writeItem(uint8_t nsIndex, ItemType datatype, const char* key, const void* data, size_t dataSize)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Page* findPage = nullptr;
    bool matchedTypePageFound = false;
    Item item;

    esp_err_t err;
    if (datatype == ItemType::BLOB) {
        err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item);
        if(err == ESP_OK) {
            matchedTypePageFound = true;
        }
    } else {
#ifdef CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY
        err = findItem(nsIndex, datatype, key, findPage, item);
        if(err == ESP_OK && findPage != nullptr) {
            matchedTypePageFound = true;
        }
#else
        err = findItem(nsIndex, ItemType::ANY, key, findPage, item);
        if(err == ESP_OK && datatype == item.datatype) {
            matchedTypePageFound = true;
        }
#endif
    }

    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        return err;
    }

    if (datatype == ItemType::BLOB) {
        VerOffset prevStart,  nextStart;
        prevStart = nextStart = VerOffset::VER_0_OFFSET;
        if (matchedTypePageFound) {
            // Do a sanity check that the item in question is actually being modified.
            // If it isn't, it is cheaper to purposefully not write out new data.
            // since it may invoke an erasure of flash.
            if (cmpMultiPageBlob(nsIndex, key, data, dataSize) == ESP_OK) {
                return ESP_OK;
            }

            if (findPage->state() == Page::PageState::UNINITIALIZED ||
                    findPage->state() == Page::PageState::INVALID) {
                err = findItem(nsIndex, datatype, key, findPage, item);
                if (err != ESP_OK) {
                    return err;
                }
            }
            /* Get the version of the previous index with same <ns,key> */
            prevStart = item.blobIndex.chunkStart;
            NVS_ASSERT_OR_RETURN(prevStart == VerOffset::VER_0_OFFSET || prevStart == VerOffset::VER_1_OFFSET, ESP_FAIL);


            /* Toggle the version by changing the offset */
            nextStart
                = (prevStart == VerOffset::VER_1_OFFSET) ? VerOffset::VER_0_OFFSET : VerOffset::VER_1_OFFSET;
        }
        /* Write the blob with new version*/
        err = writeMultiPageBlob(nsIndex, key, data, dataSize, nextStart);

        if (err == ESP_ERR_NVS_PAGE_FULL) {
            return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
        }
        if (err != ESP_OK) {
            return err;
        }

        if (matchedTypePageFound) {
            /* Erase the blob with earlier version*/
            err = eraseMultiPageBlob(nsIndex, key, prevStart);

            if (err == ESP_ERR_FLASH_OP_FAIL) {
                return ESP_ERR_NVS_REMOVE_FAILED;
            }
            if (err != ESP_OK) {
                return err;
            }

            findPage = nullptr;
        } else {
            /* Support for earlier versions where BLOBS were stored without index */
            err = findItem(nsIndex, datatype, key, findPage, item);
            if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
                return err;
            }
        }
    } else {
        // Do a sanity check that the item in question is actually being modified.
        // If it isn't, it is cheaper to purposefully not write out new data.
        // since it may invoke an erasure of flash.
        if (matchedTypePageFound &&
                findPage->cmpItem(nsIndex, datatype, key, data, dataSize) == ESP_OK) {
            return ESP_OK;
        }

        Page& page = getCurrentPage();
        err = page.writeItem(nsIndex, datatype, key, data, dataSize);
        if (err == ESP_ERR_NVS_PAGE_FULL) {
            if (page.state() != Page::PageState::FULL) {
                err = page.markFull();
                if (err != ESP_OK) {
                    return err;
                }
            }
            err = mPageManager.requestNewPage();
            if (err != ESP_OK) {
                return err;
            }

            err = getCurrentPage().writeItem(nsIndex, datatype, key, data, dataSize);
            if (err == ESP_ERR_NVS_PAGE_FULL) {
                return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
            }
            if (err != ESP_OK) {
                return err;
            }
        } else if (err != ESP_OK) {
            return err;
        }
    }

    if (findPage) {
        if (findPage->state() == Page::PageState::UNINITIALIZED ||
                findPage->state() == Page::PageState::INVALID) {
#ifdef CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY
            err = findItem(nsIndex, datatype, key, findPage, item);
#else
            err = findItem(nsIndex, ItemType::ANY, key, findPage, item);
#endif
            if (err != ESP_OK) {
                return err;
            }
        }
#ifdef CONFIG_NVS_LEGACY_DUP_KEYS_COMPATIBILITY
        err = findPage->eraseItem(nsIndex, datatype, key);
#else
        err = findPage->eraseItem(nsIndex, ItemType::ANY, key);
#endif
        if (err == ESP_ERR_FLASH_OP_FAIL) {
            return ESP_ERR_NVS_REMOVE_FAILED;
        }
        if (err != ESP_OK) {
            return err;
        }
    }
#ifdef DEBUG_STORAGE
    debugCheck();
#endif
    return ESP_OK;
}

esp_err_t Storage::createOrOpenNamespace(const char* nsName, bool canCreate, uint8_t& nsIndex)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }
    auto it = std::find_if(mNamespaces.begin(), mNamespaces.end(), [=] (const NamespaceEntry& e) -> bool {
        return strncmp(nsName, e.mName, sizeof(e.mName) - 1) == 0;
    });
    if (it == std::end(mNamespaces)) {
        if (!canCreate) {
            return ESP_ERR_NVS_NOT_FOUND;
        }

        uint8_t ns;
        bool ns_state;
        for (ns = 1; ns < 255; ++ns) {
            if (mNamespaceUsage.get(ns, &ns_state) != ESP_OK) {
                return ESP_FAIL;
            }
            if (!ns_state) {
                break;
            }
        }

        if (ns == 255) {
            return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
        }

        auto err = writeItem(Page::NS_INDEX, ItemType::U8, nsName, &ns, sizeof(ns));
        if (err != ESP_OK) {
            return err;
        }
        if (mNamespaceUsage.set(ns, true) != ESP_OK) {
            return ESP_FAIL;
        }
        nsIndex = ns;

        NamespaceEntry* entry = new (std::nothrow) NamespaceEntry;
        if (!entry) {
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

    /* First read the blob index */
    auto err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t chunkCount = item.blobIndex.chunkCount;
    VerOffset chunkStart = item.blobIndex.chunkStart;
    size_t offset = 0;

    NVS_ASSERT_OR_RETURN(dataSize == item.blobIndex.dataSize, ESP_FAIL);

    /* Now read corresponding chunks */
    for (uint8_t chunkNum = 0; chunkNum < chunkCount; chunkNum++) {
        err = findItem(nsIndex, ItemType::BLOB_DATA, key, findPage, item, static_cast<uint8_t> (chunkStart) + chunkNum);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            return err;
        }
        err = findPage->readItem(nsIndex, ItemType::BLOB_DATA, key, static_cast<uint8_t*>(data) + offset, item.varLength.dataSize, static_cast<uint8_t> (chunkStart) + chunkNum);
        if (err != ESP_OK) {
            return err;
        }
        NVS_ASSERT_OR_RETURN(static_cast<uint8_t> (chunkStart) + chunkNum == item.chunkIndex, ESP_FAIL);

        offset += item.varLength.dataSize;
    }
    NVS_ASSERT_OR_RETURN(offset == dataSize, ESP_FAIL);

    if (err == ESP_ERR_NVS_NOT_FOUND) {
        eraseMultiPageBlob(nsIndex, key); // cleanup if a chunk is not found
    }
    return err;
}

esp_err_t Storage::cmpMultiPageBlob(uint8_t nsIndex, const char* key, const void* data, size_t dataSize)
{
    Item item;
    Page* findPage = nullptr;

    /* First read the blob index */
    auto err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t chunkCount = item.blobIndex.chunkCount;
    VerOffset chunkStart = item.blobIndex.chunkStart;
    size_t readSize = item.blobIndex.dataSize;
    size_t offset = 0;

    if (dataSize != readSize) {
        return ESP_ERR_NVS_CONTENT_DIFFERS;
    }

    /* Now read corresponding chunks */
    for (uint8_t chunkNum = 0; chunkNum < chunkCount; chunkNum++) {
        err = findItem(nsIndex, ItemType::BLOB_DATA, key, findPage, item, static_cast<uint8_t> (chunkStart) + chunkNum);
        if (err != ESP_OK) {
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            return err;
        }
        err = findPage->cmpItem(nsIndex, ItemType::BLOB_DATA, key, static_cast<const uint8_t*>(data) + offset, item.varLength.dataSize, static_cast<uint8_t> (chunkStart) + chunkNum);
        if (err != ESP_OK) {
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
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    if (datatype == ItemType::BLOB) {
        auto err = readMultiPageBlob(nsIndex, key, data, dataSize);
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            return err;
        } // else check if the blob is stored with earlier version format without index
    }

    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }
    return findPage->readItem(nsIndex, datatype, key, data, dataSize);

}

esp_err_t Storage::eraseMultiPageBlob(uint8_t nsIndex, const char* key, VerOffset chunkStart)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }
    Item item;
    Page* findPage = nullptr;

    auto err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item, Page::CHUNK_ANY, chunkStart);
    if (err != ESP_OK) {
        return err;
    }
    /* Erase the index first and make children blobs orphan*/
    err = findPage->eraseItem(nsIndex, ItemType::BLOB_IDX, key, Page::CHUNK_ANY, chunkStart);
    if (err != ESP_OK) {
        return err;
    }

    uint8_t chunkCount = item.blobIndex.chunkCount;

    if (chunkStart == VerOffset::VER_ANY) {
        chunkStart = item.blobIndex.chunkStart;
    } else {
        NVS_ASSERT_OR_RETURN(chunkStart == item.blobIndex.chunkStart, ESP_FAIL);
    }

    /* Now erase corresponding chunks*/
    for (uint8_t chunkNum = 0; chunkNum < chunkCount; chunkNum++) {
        err = findItem(nsIndex, ItemType::BLOB_DATA, key, findPage, item, static_cast<uint8_t> (chunkStart) + chunkNum);

        if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
            return err;
        } else if (err == ESP_ERR_NVS_NOT_FOUND) {
            continue; // Keep erasing other chunks
        }
        err = findPage->eraseItem(nsIndex, ItemType::BLOB_DATA, key, static_cast<uint8_t> (chunkStart) + chunkNum);
        if (err != ESP_OK) {
            return err;
        }

    }

    return ESP_OK;
}

esp_err_t Storage::eraseItem(uint8_t nsIndex, ItemType datatype, const char* key)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    if (datatype == ItemType::BLOB) {
        return eraseMultiPageBlob(nsIndex, key);
    }

    Item item;
    Page* findPage = nullptr;
    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }

    if (item.datatype == ItemType::BLOB_DATA || item.datatype == ItemType::BLOB_IDX) {
        return eraseMultiPageBlob(nsIndex, key);
    }

    return findPage->eraseItem(nsIndex, datatype, key);
}

esp_err_t Storage::eraseNamespace(uint8_t nsIndex)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    for (auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
        while (true) {
            auto err = it->eraseItem(nsIndex, ItemType::ANY, nullptr);
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            else if (err != ESP_OK) {
                return err;
            }
        }
    }
    return ESP_OK;

}

esp_err_t Storage::findKey(const uint8_t nsIndex, const char* key, ItemType* datatype)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    auto err = findItem(nsIndex, ItemType::ANY, key, findPage, item);
    if (err != ESP_OK) {
        return err;
    }

    if(datatype != nullptr) {
        *datatype = item.datatype;
    }

    return err;
}

esp_err_t Storage::getItemDataSize(uint8_t nsIndex, ItemType datatype, const char* key, size_t& dataSize)
{
    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    Item item;
    Page* findPage = nullptr;
    auto err = findItem(nsIndex, datatype, key, findPage, item);
    if (err != ESP_OK) {
        if (datatype != ItemType::BLOB) {
            return err;
        }
        err = findItem(nsIndex, ItemType::BLOB_IDX, key, findPage, item);
        if (err != ESP_OK) {
            return err;
        }
        dataSize = item.blobIndex.dataSize;
        return ESP_OK;
    }

    dataSize = item.varLength.dataSize;
    return ESP_OK;
}

void Storage::debugDump()
{
    for (auto p = mPageManager.begin(); p != mPageManager.end(); ++p) {
        p->debugDump();
    }
}

#ifdef DEBUG_STORAGE
void Storage::debugCheck()
{
    std::map<std::string, Page*> keys;

    for (auto p = mPageManager.begin(); p != mPageManager.end(); ++p) {
        size_t itemIndex = 0;
        size_t usedCount = 0;
        Item item;
        while (p->findItem(Page::NS_ANY, ItemType::ANY, nullptr, itemIndex, item) == ESP_OK) {
            std::stringstream keyrepr;
            keyrepr << static_cast<unsigned>(item.nsIndex) << "_" << static_cast<unsigned>(item.datatype) << "_" << item.key <<"_"<<static_cast<unsigned>(item.chunkIndex);
            std::string keystr = keyrepr.str();
            if (keys.find(keystr) != std::end(keys)) {
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

    if (mState != StorageState::ACTIVE) {
        return ESP_ERR_NVS_NOT_INITIALIZED;
    }

    for (auto it = std::begin(mPageManager); it != std::end(mPageManager); ++it) {
        size_t itemIndex = 0;
        Item item;
        while (true) {
            auto err = it->findItem(nsIndex, ItemType::ANY, nullptr, itemIndex, item);
            if (err == ESP_ERR_NVS_NOT_FOUND) {
                break;
            }
            else if (err != ESP_OK) {
                return err;
            }
            usedEntries += item.span;
            itemIndex   += item.span;
            if (itemIndex >= it->ENTRY_COUNT) break;
        }
    }
    return ESP_OK;
}

void Storage::fillEntryInfo(Item &item, nvs_entry_info_t &info)
{
    info.type = static_cast<nvs_type_t>(item.datatype);
    strncpy(info.key, item.key, sizeof(info.key) - 1);
    info.key[sizeof(info.key) - 1] = '\0';

    for (auto &name : mNamespaces) {
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

    if (namespace_name != nullptr) {
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

    for (auto page = it->page; page != mPageManager.end(); ++page) {
        do {
            err = page->findItem(it->nsIndex, (ItemType)it->type, nullptr, it->entryIndex, item);
            it->entryIndex += item.span;
            if(err == ESP_OK && isIterableItem(item) && !isMultipageBlob(item)) {
                fillEntryInfo(item, it->entry_info);
                it->page = page;
                return true;
            }
        } while (err != ESP_ERR_NVS_NOT_FOUND);

        it->entryIndex = 0;
    }

    return false;
}


}
