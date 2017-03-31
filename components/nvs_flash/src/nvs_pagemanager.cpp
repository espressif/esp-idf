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
#include "nvs_pagemanager.hpp"

namespace nvs
{
esp_err_t PageManager::load(uint32_t baseSector, uint32_t sectorCount)
{
    mBaseSector = baseSector;
    mPageCount = sectorCount;
    mPageList.clear();
    mFreePageList.clear();
    mPages.reset(new Page[sectorCount]);

    for (uint32_t i = 0; i < sectorCount; ++i) {
        auto err = mPages[i].load(baseSector + i);
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
        ESP_ERROR_CHECK( mPageList.back().getSeqNumber(lastSeqNo) );
        mSeqNumber = lastSeqNo + 1;
    }

    // if power went out after a new item for the given key was written,
    // but before the old one was erased, we end up with a duplicate item
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
        for (auto it = begin(); it != last; ++it) {
            if (it->eraseItem(item.nsIndex, item.datatype, item.key) == ESP_OK) {
                break;
            }
        }
    }

    // check if power went out while page was being freed
    for (auto it = begin(); it!= end(); ++it) {
        if (it->state() == Page::PageState::FREEING) {
            Page* newPage = &mPageList.back();
            if (newPage->state() != Page::PageState::ACTIVE) {
                auto err = activatePage();
                if (err != ESP_OK) {
                    return err;
                }
                newPage = &mPageList.back();
            }
            while (true) {
                auto err = it->moveItem(*newPage);
                if (err == ESP_ERR_NVS_NOT_FOUND) {
                    break;
                } else if (err != ESP_OK) {
                    return err;
                }
            }

            auto err = it->erase();
            if (err != ESP_OK) {
                return err;
            }

            Page* p = static_cast<Page*>(it);
            mPageList.erase(it);
            mFreePageList.push_back(p);
            break;
        }
    }

    // partition should have at least one free page
    if (mFreePageList.size() == 0) {
        return ESP_ERR_NVS_NO_FREE_PAGES;
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

    // find the page with the higest number of erased items
    TPageListIterator maxErasedItemsPageIt;
    size_t maxErasedItems = 0;
    for (auto it = begin(); it != end(); ++it) {
        auto erased = it->getErasedEntryCount();
        if (erased > maxErasedItems) {
            maxErasedItemsPageIt = it;
            maxErasedItems = erased;
        }
    }

    if (maxErasedItems == 0) {
        return ESP_ERR_NVS_NOT_ENOUGH_SPACE;
    }

    esp_err_t err = activatePage();
    if (err != ESP_OK) {
        return err;
    }

    Page* newPage = &mPageList.back();

    Page* erasedPage = maxErasedItemsPageIt;
#ifndef NDEBUG
    size_t usedEntries = erasedPage->getUsedEntryCount();
#endif
    err = erasedPage->markFreeing();
    if (err != ESP_OK) {
        return err;
    }
    while (true) {
        err = erasedPage->moveItem(*newPage);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            break;
        } else if (err != ESP_OK) {
            return err;
        }
    }

    err = erasedPage->erase();
    if (err != ESP_OK) {
        return err;
    }

#ifndef NDEBUG
    assert(usedEntries == newPage->getUsedEntryCount());
#endif
    
    mPageList.erase(maxErasedItemsPageIt);
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

} // namespace nvs
