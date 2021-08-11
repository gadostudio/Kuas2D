#pragma once

#include <atomic>
#include <type_traits>

namespace kuas
{
    class RefCount
    {
    public:
        RefCount() :
            m_counter(1)
        {
        }

        virtual ~RefCount()
        {
        }

        void addRef() const
        {
            m_counter.fetch_add(1, std::memory_order_relaxed);
        }

        void release() const
        {
            if (m_counter.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                delete this;
            }
        }

        uint32_t getRefs() const
        {
            return m_counter.load(std::memory_order_relaxed);
        }

    private:
        mutable std::atomic<uint32_t> m_counter;
    };

    template<class T>
    static inline T* safeAddRef(T* ptr)
    {
        if (ptr != nullptr) {
            ptr->addRef();
        }

        return ptr;
    }

    template<class T>
    static inline T* safeRelease(T* ptr)
    {
        if (ptr != nullptr) {
            ptr->release();
        }

        return ptr;
    }

    template<class T>
    class Ref
    {
    public:
        Ref() :
            m_ptr(nullptr)
        {
        }

        explicit Ref(T* ptr) :
            m_ptr(ptr)
        {
        }

        Ref(const Ref<T>& other) :
            m_ptr(safeAddRef(other.m_ptr))
        {
        }

        template<class D, std::enable_if_t<std::is_convertible<D*, T*>::value, bool> = true>
        Ref(const Ref<D>& other) :
            m_ptr(safeAddRef(other.m_ptr))
        {
        }

        ~Ref()
        {
            safeRelease(m_ptr);
            m_ptr = nullptr;
        }

        Ref<T>& operator=(const Ref<T>& other)
        {
            if (&other != this) {
                reset(safeAddRef(other.m_ptr));
            }

            return *this;
        }

        template<class D, std::enable_if_t<std::is_convertible<D*, T*>::value, bool> = true>
        Ref<T>& operator=(const Ref<D>& other)
        {
            reset(safeAddRef(other.get()));
            return *this;
        }

        T* get() const
        {
            return m_ptr;
        }

        uint32_t getRefs() const
        {
            if (m_ptr == nullptr) {
                return 0;
            }

            return m_ptr->getRefs();
        }

        T& operator*()
        {
            return *m_ptr;
        }

        T* operator->() const
        {
            return m_ptr;
        }

        T** operator&()
        {
            return &m_ptr;
        }

        const T** operator&() const
        {
            return &m_ptr;
        }

        void reset(T* newPtr = nullptr)
        {
            T* tmp = m_ptr;
            m_ptr = newPtr;
            safeRelease(tmp);
        }

        T* release()
        {
            T* tmp = m_ptr;
            m_ptr = nullptr;
            return tmp;
        }

    private:
        T* m_ptr;
    };
}

#define KUAS_PTR_CAST(type, v) \
    static_cast<##type*>(##v)
