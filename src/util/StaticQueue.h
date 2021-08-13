#pragma once

#include <cstdint>

namespace kuas
{
    template<class T, size_t Size>
    class StaticQueue
    {
    public:
        StaticQueue() :
            m_writePos(0)
        {
        }

        bool push(const T& d)
        {
            if ((m_writePos + 1) == Size) {
                return false;
            }

            m_data[m_writePos++] = d;
            return true;
        }

        void flush()
        {
            m_writePos = 0;
        }

        T* getData()
        {
            return m_data;
        }

        size_t getWritePos() const
        {
            return m_writePos;
        }

    private:
        T m_data[Size];
        size_t m_writePos;
    };
}
