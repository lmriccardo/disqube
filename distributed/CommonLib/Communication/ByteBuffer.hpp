#ifndef _BYTEBUFFER_HPP
#define _BYTEBUFFER_HPP

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>

namespace CommonLib::Communication
{
    using buffer_t = std::vector<unsigned char>;

    enum ByteOrder
    {
        BigEndian,
        LittleEndian
    };

    class ByteBuffer
    {
        private:
            static void checkForOutOfBound(const int, const std::size_t, const std::size_t, std::string);
            static void errorIfEmpty(ByteBuffer* _buff, std::string _func);

        protected:
            std::size_t _capacity; //!< The maximum capacity of the buffer
            std::size_t _position; //!< The current position into the buffer
            buffer_t    _buffer;   //!< The buffer
            ByteOrder   _order;    //!< Either Big/Little-Edian
            
        public:
            const static std::size_t BYTE_SIZE  = 1;
            const static std::size_t SHORT_SIZE = 2;
            const static std::size_t INT_SIZE   = 4;

            ByteBuffer(const std::size_t capacity);
            ByteBuffer(const ByteBuffer& other)
            {
                _order = other._order;
                _capacity = other._capacity;
                _position = other._position;
                
                // Copy the vector content
                _buffer.resize(other._buffer.size());
                std::copy(other._buffer.begin(), other._buffer.end(), _buffer.begin());
            }

            ~ByteBuffer() = default;

            ByteBuffer& operator=(const ByteBuffer& other);

            void setByteOrder(const ByteOrder& newOrder);
            const ByteOrder& getByteOrder();

            std::size_t position() const;
            void position(std::size_t newPos);

            std::size_t getBufferSize() const;
            std::size_t getBufferCapacity() const;
            std::size_t getRemainingCapacity() const;
            std::size_t getRemainingSize() const;
            bool isEmpty() const;
            void clear();

            void put(const unsigned char _data);
            void put(const unsigned short _data);
            void put(const unsigned int _data);
            void put(unsigned char* const& _data, const int _start, const std::size_t _size);
            void put(unsigned char* const& _data, const std::size_t _size);

            unsigned char get();
            unsigned short getShort();
            unsigned int getInt();
            void getBuffer(unsigned char* _data, const int _start, const std::size_t _size);
            void getBuffer(unsigned char* _data, const std::size_t _size);
            
            const buffer_t& getBuffer() const;
    };

    typedef std::shared_ptr<ByteBuffer> ByteBuffer_ptr;
}

#endif
