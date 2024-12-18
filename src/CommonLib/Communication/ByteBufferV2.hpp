#pragma once

#include <ctype.h>
#include <cstring>
#include <memory>
#include <endian.h>
#include <cstdlib>
#include <iostream>

namespace Lib::Network
{
    class ByteBufferV2
    {
    public:
        const static size_t BYTE_SIZE = 1;
        const static size_t SHORT_SIZE = 2;
        const static size_t INT_SIZE = 4;
        const static size_t INT_SIZE_64 = 8;

        // The two possible byte order
        enum class ByteOrder
        {
            BigEndian = __BIG_ENDIAN,
            LittleEndian = __LITTLE_ENDIAN
        };

        ByteBufferV2() : m_Capacity(0), m_Order(ByteOrder::LittleEndian),
                         m_Position(0), m_Size(0), m_Buffer(nullptr) {}

        ByteBufferV2(const size_t capacity);
        ByteBufferV2(const unsigned char *buffer, const size_t size);
        ByteBufferV2(const ByteBufferV2 &other);
        virtual ~ByteBufferV2();

        ByteBufferV2 &operator=(const ByteBufferV2 &other);

        void allocate(const size_t capacity); // Allocate new capacity
        size_t position() const;              // Returns the current position
        void position(const size_t newpos);   // Sets the new position

        void setByteOrder(const ByteOrder &order); // Set the byte order
        const ByteOrder &getByteOrder() const;     // Returns current byte order
        size_t getBufferCapacity() const;          // Returns the buffer capacity
        size_t getBufferSize() const;              // Returns the current buffer size
        size_t getRemainingCapacity() const;       // Returns the remaining capacity
        size_t getRemainingSize() const;           // Returns the remaining size (from position)

        bool isEmpty() const; // If the buffer is empty or not
        void clear();         // Clear the entire buffer

        void put(const unsigned char data);  // Put a single byte into the buffer
        void put(const unsigned short data); // Put a single short into the buffer
        void put(const unsigned int data);   // Put a single integer 32 into the buffer
        void put(const uint64_t data);       // Put a single integer 64 into the buffer

        // Copy the input buffer into the class buffer if possible
        void put(const unsigned char *buff, const size_t start, const size_t size);
        void put(const unsigned char *buff, const size_t size);

        unsigned char get();               // Returns a single byte from the buffer
        unsigned short getUnsignedShort(); // Returns a single short from the buffer
        unsigned int getUnsignedInt();     // Returns a single int from the buffer
        unsigned long getUnsignedLong();   // Returns a single long from the buffer

        short getShort(); // Returns a signed short from the buffer
        int getInt();     // Returns a signed int from the buffer
        long getLong();   // Returns a signed long from the buffer

        // Copy the content of the buffer into the destination
        void getBuffer(unsigned char *dst, const size_t start, const size_t size);
        void getBuffer(unsigned char *dst, const size_t size);

        void getBuffer(unsigned char *dst); // Copy the entire buffer
        unsigned char *getBuffer() const;   // Returns the actual pointer to the buffer

    private:
        void checkForOutOfBound(const size_t curr_pos, bool on_size);
        void errorIfEmpty();

        unsigned char *m_Buffer; // The actual byte buffer
        size_t m_Capacity;       // The maximum capacity of the buffer
        size_t m_Position;       // The current position into the buffer
        size_t m_Size;           // The total number of elements into the buffer
        ByteOrder m_Order;       // The byte order (host or network-order)
    };

    typedef std::shared_ptr<ByteBufferV2> ByteBufferV2_ptr;
    typedef std::unique_ptr<ByteBufferV2> ByteBufferV2_uptr;
}