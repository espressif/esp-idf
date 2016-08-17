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
#ifndef intrusive_list_h
#define intrusive_list_h

#include <cassert>

template <typename T>
class intrusive_list;

template <typename T>
class intrusive_list_node
{
protected:
    friend class intrusive_list<T>;
    T* mPrev = nullptr;
    T* mNext = nullptr;
};

template <typename T>
class intrusive_list
{
    typedef intrusive_list_node<T> TNode;
    static_assert(std::is_base_of<TNode, T>::value, "");

public:

    class iterator : public std::iterator<std::forward_iterator_tag, T>
    {
    public:

        iterator() : mPos(nullptr) {}

        iterator(T* pos) : mPos(pos) {}

        iterator operator++(int)
        {
            auto result = *this;
            mPos = mPos->mNext;
            return result;
        }

        iterator operator--(int)
        {
            auto result = *this;
            mPos = mPos->mPrev;
            return result;
        }

        iterator& operator++()
        {
            mPos = mPos->mNext;
            return *this;
        }

        iterator& operator--()
        {
            mPos = mPos->mPrev;
            return *this;
        }


        bool operator==(const iterator& other) const
        {
            return mPos == other.mPos;
        }

        bool operator!=(const iterator& other) const
        {
            return !(*this == other);
        }

        T& operator*()
        {
            return *mPos;
        }

        const T& operator*() const
        {
            return *mPos;
        }

        T* operator->()
        {
            return mPos;
        }

        const T* operator->() const
        {
            return mPos;
        }

        operator T*()
        {
            return mPos;
        }

        operator const T*() const
        {
            return mPos;
        }


    protected:
        T* mPos;
    };

    void push_back(T* node)
    {
        if (mLast) {
            mLast->mNext = node;
        }
        node->mPrev = mLast;
        node->mNext = nullptr;
        mLast = node;
        if (mFirst == nullptr) {
            mFirst = node;
        }
        ++mSize;
    }

    void push_front(T* node)
    {
        node->mPrev = nullptr;
        node->mNext = mFirst;
        if (mFirst) {
            mFirst->mPrev = node;
        }
        mFirst = node;
        if (mLast == nullptr) {
            mLast = node;
        }
        ++mSize;
    }

    T& back()
    {
        return *mLast;
    }

    const T& back() const
    {
        return *mLast;
    }

    T& front()
    {
        return *mFirst;
    }

    const T& front() const
    {
        return *mFirst;
    }

    void pop_front()
    {
        erase(mFirst);
    }

    void pop_back()
    {
        erase(mLast);
    }

    void insert(iterator next, T* node)
    {
        if (static_cast<T*>(next) == nullptr) {
            push_back(node);
        } else {
            auto prev = next->mPrev;
            if (!prev) {
                push_front(node);
            } else {
                prev->mNext = node;
                next->mPrev = node;
                node->mNext = next;
                node->mPrev = &(*prev);
                ++mSize;
            }
        }
    }

    void erase(iterator it)
    {
        auto prev = it->mPrev;
        auto next = it->mNext;

        if (prev) {
            prev->mNext = next;
        } else {
            mFirst = next;
        }
        if (next) {
            next->mPrev = prev;
        } else {
            mLast = prev;
        }
        --mSize;
    }

    iterator begin()
    {
        return iterator(mFirst);
    }

    iterator end()
    {
        return iterator(nullptr);
    }

    size_t size() const
    {
        return mSize;
    }

    bool empty() const
    {
        return mSize == 0;
    }
    
    
    void clear()
    {
        while (mFirst) {
            erase(mFirst);
        }
    }

protected:
    T* mFirst = nullptr;
    T* mLast = nullptr;
    size_t mSize = 0;
};


#endif /* intrusive_list_h */
