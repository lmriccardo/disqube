#include "ByteBuffer.hpp"

using namespace Lib::Network;

ByteBuffer::ByteBuffer(const std::size_t capacity)
{
    _capacity = capacity;
    _position = 0;

    // Reserve the given number of bytes for the buffer
    _buffer.reserve(_capacity);

    // Set Big Endian as the default byte order
    _order = ByteOrder::BigEndian;
}

ByteBuffer::ByteBuffer(const unsigned char *buffer, const std::size_t nofBytes)
    : ByteBuffer(nofBytes)
{
    put(buffer, nofBytes);
    position(0);
}

ByteBuffer &ByteBuffer::operator=(const ByteBuffer &other)
{
    if (this != &other)
    {
        _capacity = other._capacity;
        _position = other._position;
        _order = other._order;
                    
        // Copy the vector content
        _buffer.resize(other._buffer.size());
        std::copy(other._buffer.begin(), other._buffer.end(), _buffer.begin());
    }

    return *this;
}

void ByteBuffer::setByteOrder(const ByteOrder &newOrder)
{
    _order = newOrder;
}

const ByteBuffer::ByteOrder & ByteBuffer::getByteOrder()
{
    return _order;
}

std::size_t ByteBuffer::position() const
{
    return _position;
}

void ByteBuffer::position(std::size_t newPos)
{
    _position = newPos;
}

std::size_t ByteBuffer::getBufferSize() const
{
    return _buffer.size();
}

std::size_t ByteBuffer::getBufferCapacity() const
{
    return _capacity;
}

std::size_t ByteBuffer::getRemainingCapacity() const
{
    return getBufferCapacity() - getBufferSize();
}

std::size_t ByteBuffer::getRemainingSize() const
{
    return getBufferSize() - _position;
}

bool ByteBuffer::isEmpty() const
{
    return _buffer.empty();
}

void ByteBuffer::clear()
{
    _buffer.clear();
    position(0);
}

void ByteBuffer::spare()
{
    put(static_cast<unsigned char>(0.0));
}

void ByteBuffer::put(const unsigned char _data)
{
    ByteBuffer::checkForOutOfBound(_position, ByteBuffer::BYTE_SIZE, _capacity, "ByteBuffer::put");
    _buffer.insert(_buffer.begin() + _position, _data);
    position(_position + ByteBuffer::BYTE_SIZE);
}

void ByteBuffer::put(const unsigned short _data)
{
    ByteBuffer::checkForOutOfBound(_position, ByteBuffer::SHORT_SIZE, _capacity, "ByteBuffer::put");

    unsigned short x = _data;

    // Check if the byte order is big or little endian
    if (_order == ByteOrder::BigEndian) x = htobe16(x);

    unsigned char _datau = x >> 8;
    unsigned char _datal = x & ~(_datau << 8);

    put(_datau);
    put(_datal);
    
}

void ByteBuffer::put(const unsigned int _data)
{
    ByteBuffer::checkForOutOfBound(_position, ByteBuffer::INT_SIZE, _capacity, "ByteBuffer::put");

    unsigned int x = _data;

    // Check if the byte order is big or little endian
    if (_order == ByteOrder::BigEndian) x = htobe32(x);

    unsigned short _datau = x >> 16;
    unsigned short _datal = x & ~(_datau << 16);

    put(_datau);
    put(_datal);
}

void ByteBuffer::put(const unsigned char* _data, const int _start, const std::size_t _size)
{
    position(_start);
    ByteBuffer::checkForOutOfBound(_start, _size, _capacity, "ByteBuffer::put");

    for (std::size_t pos = 0; pos < _size; pos++)
    {
        put(_data[pos]);
    }
}

void ByteBuffer::put(const unsigned char* _data, const std::size_t _size)
{
    put(_data, _position, _size);
}

unsigned char ByteBuffer::get()
{
    ByteBuffer::checkForOutOfBound(_position, ByteBuffer::BYTE_SIZE, _capacity, "ByteBuffer::get");
    ByteBuffer::errorIfEmpty(this, "ByteBuffer::get");

    unsigned char ret = _buffer.at(_position);
    position(_position + ByteBuffer::BYTE_SIZE);
    return ret;
}

unsigned short ByteBuffer::getShort()
{
    ByteBuffer::checkForOutOfBound(_position, ByteBuffer::SHORT_SIZE, _capacity, "ByteBuffer::getShort");
    ByteBuffer::errorIfEmpty(this, "ByteBuffer::getShort");

    unsigned char ret1 = get();
    unsigned char ret2 = get();
    unsigned short ret = (ret1 << 8) + ret2;

    // Check the byte order
    if (_order == ByteOrder::BigEndian)
    {
        ret = be16toh(ret);
    }

    return ret;
}

unsigned int ByteBuffer::getInt()
{
    ByteBuffer::checkForOutOfBound(_position, ByteBuffer::INT_SIZE, _capacity, "ByteBuffer::getInt");
    ByteBuffer::errorIfEmpty(this, "ByteBuffer::getInt");

    unsigned short ret1 = getShort();
    unsigned short ret2 = getShort();
    unsigned int ret = (ret1 << 16) + ret2;

    // Check the byte order
    if (_order == ByteOrder::BigEndian)
    {
        ret = be32toh(ret);
    }

    return ret;
}

void ByteBuffer::getBuffer(unsigned char* _data, const int _start, const std::size_t _size)
{
    position(_start);
    ByteBuffer::checkForOutOfBound(_position, _size, _capacity, "ByteBuffer::getBuffer");
    ByteBuffer::errorIfEmpty(this, "ByteBuffer::getBuffer");

    for (std::size_t pos = 0; pos < _size; pos++)
    {
        _data[pos] = get();
    }
}

void ByteBuffer::getBuffer(unsigned char* _data, const std::size_t _size)
{
    getBuffer(_data, _position, _size);
}

const ByteBuffer::buffer_t& ByteBuffer::getBuffer() const
{
    return _buffer;
}

void ByteBuffer::checkForOutOfBound(
    const int _position, const std::size_t _size, const std::size_t _max, std::string _func
) {
    if (_position + _size > _max)
    {
        char message[200];
        snprintf(
            message, sizeof(message), "[%s] Index Out Of Bound: %ld > %ld\n", 
            _func.c_str(), _position + _size, _max
        );

        std::cerr << message << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ByteBuffer::errorIfEmpty(ByteBuffer* _buff, std::string _func)
{
    if (_buff->isEmpty())
    {
        char message[200];
        snprintf(message, sizeof(message), "[%s] The Buffer is Empty.\n",_func.c_str());
        std::string msg = message;
        std::cerr << msg << std::endl;
        exit(EXIT_FAILURE);
    }
}
