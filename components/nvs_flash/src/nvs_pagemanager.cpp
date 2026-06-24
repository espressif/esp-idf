/*
 * SPDX-FileCopyrightText: 2015-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "nvs_pagemanager.hpp"

using namespace std;

namespace nvs
{
esp_err_t PageManager::load(Partition *partition, uint32_t baseSector, uint32_t sectorCount)
{
    if (partition == nullptr) {
        return ESP_ERR_INVALID_ARG;
    }

    mBaseSector = baseSector;
    mPageCount = sectorCount;
    mPageList.clear();
    mFreePageList.clear();
    mPages.reset(new (nothrow) Page[sectorCount]);

    if (!mPages) return ESP_ERR_NO_MEM;

    for (uint32_t i = 0; i < sectorCount; ++i) {
        auto err = mPages[i].load(partition, baseSector + i);
        if (err != ESP_OK) {
            return err;
        }
        uint32_t seqNumber;
        if (mPages[i].getSeqNumber(seqNumber) != ESP_OK) {
            mFreePageList.push_back(&mPages[i]);
        } else {
            auto pos = std::find_if(std::begin(mPageList), std::end(mPageList), [=](const Page& page) -> bool {
                uint32_t otherSeqNumber;
                return page.getSeqNumber(otherSeqNumber) == ESP_OK && otherSeqNumber > seqNumber;
            });
            if (pos == mPageList.end()) {
                mPageList.push_back(&mPages[i]);
            } else {
                mPageList.insert(pos, &mPages[i]);
            }
        }
    }

    if (mPageList.empty()) {
        mSeqNumber = 0;
        return activatePage();
    } else {
        uint32_t lastSeqNo;
        auto err = mPageList.back().getSeqNumber(lastSeqNo);
        if (err != ESP_OK) {
            return err;
        }
        mSeqNumber = lastSeqNo + 1;
    }

    // if power went out after a new item for the given key was written,
    // but before the old one was erased, we end up with a duplicate item
    if (!partition->get_readonly()) {
        Page& lastPage = back();
        size_t lastItemIndex = SIZE_MAX;
        Item item;
        size_t itemIndex = 0;
        while (lastPage.findItem(Page::NS_ANY, ItemType::ANY, nullptr, itemIndex, item) == ESP_OK) {
            itemIndex += item.span;
            lastItemIndex = itemIndex;
        }

        if (lastItemIndex != SIZE_MAX) {
            auto last = PageManager::TPageListIterator(&lastPage);
            TPageListIterator it;

            for (it = begin(); it != last; ++it) {

                if ((it->state() != Page::PageState::FREEING) &&
                        (it->eraseItem(item.nsIndex, item.datatype, item.key, Page::DEFAULT_PURGE_AFTER_ERASE, item.chunkIndex) == ESP_OK)) {
                    break;
                }
            }
            if ((it == last) && (item.datatype == ItemType::BLOB_IDX)) {
                /* Rare case in which the blob was stored using old format, but power went just after writing
                * blob index during modification. Loop again and delete the old version blob*/
                for (it = begin(); it != last; ++it) {

                    if ((it->state() != Page::PageState::FREEING) &&
                            (it->eraseItem(item.nsIndex, ItemType::BLOB, item.key, Page::DEFAULT_PURGE_AFTER_ERASE, item.chunkIndex) == ESP_OK)) {
                        break;
                    }
                }
            }
        }

        // check if power went out while page was being freed
        for (auto it = begin(); it!= end(); ++it) {
            if (it->state() == Page::PageState::FREEING) {
                Page* newPage = &mPageList.back();
                if (newPage->state() == Page::PageState::ACTIVE) {
                    auto err = newPage->erase();
                    if (err != ESP_OK) {
                        return err;
                    }
                    mPageList.erase(newPage);
                    mFreePageList.push_back(newPage);
                }
                auto err = activatePage();
                if (err != ESP_OK) {
                    return err;
                }
                newPage = &mPageList.back();

                err = it->copyItems(*newPage);
                if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
                    return err;
                }

                err = it->erase();
                if (err != ESP_OK) {
                    return err;
                }

                Page* p = static_cast<Page*>(it);
                mPageList.erase(it);
                mFreePageList.push_back(p);
                break;
            }
        }

        // partition should have at least one free page if it is not read-only
        if (mFreePageList.empty()) {
            return ESP_ERR_NVS_NO_FREE_PAGES;
        }
    }

    return ESP_OK;
}

esp_err_t PageManager::requestNewPage()
{
    if (mFreePageList.empty()) {
        return ESP_ERR_NVS_INVALID_STATE;
    }

    // do we have at least two free pages? in that case no erasing is required
    if (mFreePageList.size() >= 2) {
        return activatePage();
    }

    // find the page with the highest number of erased items
    TPageListIterator maxUnusedItemsPageIt;
    size_t maxUnusedItems = 0;
    for (auto it = begin(); it != end(); ++it) {

        auto unused =  Page::ENTRY_COUNT - it->getUsedEntryCount();
        if (unused > maxUnusedItems) {
            maxUnusedItemsPageIt = it;
            maxUnusedItems = unused;
        }
    }

    if (maxUnusedItems == 0) {
        return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
    }

    Page* erasedPage = maxUnusedItemsPageIt;

#ifndef NDEBUG
    size_t usedEntries = erasedPage->getUsedEntryCount();
#endif
    esp_err_t err = erasedPage->markFreeing();
    if (err != ESP_OK) {
        return err;
    }

    // Activating a new page first when markFreeing is done. The data recovery from power loss will be
    // driven by the existence of the (transitional) FREEING state of a page.
    err = activatePage();
    if (err != ESP_OK) {
        return err;
    }

    Page* newPage = &mPageList.back();

    err = erasedPage->copyItems(*newPage);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        return err;
    }

    err = erasedPage->erase();
    if (err != ESP_OK) {
        return err;
    }

#ifndef NDEBUG
    NVS_ASSERT_OR_RETURN(usedEntries == newPage->getUsedEntryCount(), ESP_FAIL);
#endif

    mPageList.erase(maxUnusedItemsPageIt);
    mFreePageList.push_back(erasedPage);

    return ESP_OK;
}

esp_err_t PageManager::activatePage()
{
    if (mFreePageList.empty()) {
        return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
    }
    Page* p = &mFreePageList.front();
    if (p->state() == Page::PageState::CORRUPT) {
        auto err = p->erase();
        if (err != ESP_OK) {
            return err;
        }
    }
    mFreePageList.pop_front();
    mPageList.push_back(p);
    p->setSeqNumber(mSeqNumber);
    ++mSeqNumber;
    return ESP_OK;
}

esp_err_t PageManager::fillStats(nvs_stats_t& nvsStats)
{
    nvsStats.used_entries      = 0;
    nvsStats.free_entries      = 0;
    nvsStats.available_entries = 0;
    nvsStats.total_entries     = 0;
    esp_err_t err = ESP_OK;

    // list of used pages
    for (auto p = mPageList.begin(); p != mPageList.end(); ++p) {
        err = p->calcEntries(nvsStats);
        if (err != ESP_OK) {
            return err;
        }
    }

    // add free pages
    nvsStats.total_entries += mFreePageList.size() * Page::ENTRY_COUNT;
    nvsStats.free_entries  += mFreePageList.size() * Page::ENTRY_COUNT;

    // calculate available entries from free entries by applying reserved page size
    // avoid overflow of size_t declared available_entries in case of free_entries being too low
    nvsStats.available_entries = (nvsStats.free_entries >= Page::ENTRY_COUNT) ? nvsStats.free_entries - Page::ENTRY_COUNT : 0;

    return err;
}

} // namespace nvs
