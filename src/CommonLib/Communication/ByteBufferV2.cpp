#include "ByteBufferV2.hpp"

Lib::Network::ByteBufferV2::ByteBufferV2(const size_t capacity)
    : m_Capacity(capacity), m_Order(ByteOrder::LittleEndian), m_Position(0), m_Size(0)
{
    m_Buffer = new unsigned char[capacity];
    memset(m_Buffer, 0, capacity * sizeof(unsigned char));
}

Lib::Network::ByteBufferV2::ByteBufferV2(const unsigned char *buffer, const size_t size)
    : ByteBufferV2(size)
{
    m_Size = size;

    // Copy the input buffer into the ByteBuffer buffer
    memcpy(m_Buffer, buffer, size * sizeof(unsigned char));
}

Lib::Network::ByteBufferV2::ByteBufferV2(const ByteBufferV2 &other)
{
    // Copy initial simple values
    m_Capacity = other.m_Capacity;
    m_Position = other.m_Position;
    m_Order = other.m_Order;
    m_Size = other.m_Size;

    // Copy the buffer
    m_Buffer = new unsigned char[m_Capacity];
    memcpy(m_Buffer, other.m_Buffer, m_Capacity * sizeof(unsigned char));
}

Lib::Network::ByteBufferV2::~ByteBufferV2()
{
    delete[] m_Buffer;
}

Lib::Network::ByteBufferV2 &Lib::Network::ByteBufferV2::operator=(const ByteBufferV2 &other)
{
    // We need to delete the previous buffer
    if (m_Buffer != nullptr) delete[] m_Buffer;

    // Copy initial simple values
    m_Capacity = other.m_Capacity;
    m_Position = other.m_Position;
    m_Order = other.m_Order;
    m_Size = other.m_Size;

    // Copy the buffer
    m_Buffer = new unsigned char[m_Capacity];
    memcpy(m_Buffer, other.m_Buffer, m_Capacity * sizeof(unsigned char));

    return *this;
}

void Lib::Network::ByteBufferV2::allocate(const size_t capacity)
{
    if (m_Capacity < capacity)
    {
        // We need to resize the memory location
        unsigned char* tmpbuffer = new unsigned char[capacity];

        // If the buffer is not empty we need to copy the current
        // content into the new buffer and then free the memory
        if (!(m_Buffer == nullptr))
        {
            memcpy(tmpbuffer, m_Buffer, m_Capacity*sizeof(unsigned char));
            delete[] m_Buffer;
        }

        m_Buffer = tmpbuffer;
    }
}

size_t Lib::Network::ByteBufferV2::position() const
{
    return m_Position;
}

void Lib::Network::ByteBufferV2::position(const size_t newpos)
{
    // Check if the new position is inside the range
    m_Position = newpos;
}

void Lib::Network::ByteBufferV2::setByteOrder(const ByteOrder &order)
{
    m_Order = order;
}

const Lib::Network::ByteBufferV2::ByteOrder &Lib::Network::ByteBufferV2::getByteOrder() const
{
    return m_Order;
}

size_t Lib::Network::ByteBufferV2::getBufferCapacity() const
{
    return m_Capacity;
}

size_t Lib::Network::ByteBufferV2::getBufferSize() const
{
    return m_Size;
}

size_t Lib::Network::ByteBufferV2::getRemainingCapacity() const
{
    return m_Capacity - m_Size;
}

size_t Lib::Network::ByteBufferV2::getRemainingSize() const
{
    return m_Size - m_Position;
}

bool Lib::Network::ByteBufferV2::isEmpty() const
{
    return m_Size == 0;
}

void Lib::Network::ByteBufferV2::clear()
{
    memset(m_Buffer, 0, m_Capacity*sizeof(unsigned char));
    m_Position = 0;
    m_Size = 0;
}

void Lib::Network::ByteBufferV2::put(const unsigned char data)
{
    checkForOutOfBound(position() + BYTE_SIZE, false);
    m_Buffer[m_Position++] = data;
    m_Size++;
}

void Lib::Network::ByteBufferV2::put(const unsigned short data)
{
    checkForOutOfBound(position() + SHORT_SIZE, false);

    // Convert into big endian or little endian if necessary
    unsigned short x = data;
    if (m_Order == ByteOrder::BigEndian) x = htobe16(data);
    if (m_Order == ByteOrder::LittleEndian) x = htole16(data);

    // Put it into the buffer
    memcpy(m_Buffer + m_Position, &x, sizeof(unsigned short));
    m_Position += SHORT_SIZE;
    m_Size = m_Position;
}

void Lib::Network::ByteBufferV2::put(const unsigned int data)
{
    checkForOutOfBound(position() + INT_SIZE, false);

    // Convert into big endian or little endian if necessary
    unsigned int x = data;
    if (m_Order == ByteOrder::BigEndian) x = htobe32(data);
    if (m_Order == ByteOrder::LittleEndian) x = htole32(data);

    // Put it into the buffer
    memcpy(m_Buffer + m_Position, &x, sizeof(unsigned int));
    m_Position += INT_SIZE;
    m_Size = m_Position;
}

void Lib::Network::ByteBufferV2::put(const uint64_t data)
{
    checkForOutOfBound(position() + INT_SIZE_64, false);

    // Convert into big endian or little endian if necessary
    uint64_t x = data;
    if (m_Order == ByteOrder::BigEndian) x = htobe64(data);
    if (m_Order == ByteOrder::LittleEndian) x = htole64(data);

    // Put it into the buffer
    memcpy(m_Buffer + m_Position, &x, sizeof(uint64_t));
    m_Position += INT_SIZE_64;
    m_Size = m_Position;
}

void Lib::Network::ByteBufferV2::put(const unsigned char *buff, const size_t start, const size_t size)
{
    position(start); // Change the current position to start
    checkForOutOfBound(position() + size, false);
    memcpy(m_Buffer + m_Position, buff, size * sizeof(unsigned char));
    position(start + size);
    m_Size = (m_Position > m_Size) ? m_Position : m_Size;
}

void Lib::Network::ByteBufferV2::put(const unsigned char *buff, const size_t size)
{
    put(buff, position(), size);
}

unsigned char Lib::Network::ByteBufferV2::get()
{
    errorIfEmpty();
    checkForOutOfBound(position() + BYTE_SIZE, true);

    unsigned char x = m_Buffer[m_Position++];

    return x;
}

unsigned short Lib::Network::ByteBufferV2::getUnsignedShort()
{
    errorIfEmpty();
    checkForOutOfBound(position() + SHORT_SIZE, true);

    // Get the element from the buffer
    unsigned short x;
    memcpy(&x, m_Buffer + m_Position, sizeof(unsigned short));

    // Converts if necessary
    if (m_Order == ByteOrder::BigEndian) x = be16toh(x);
    if (m_Order == ByteOrder::LittleEndian) x = le16toh(x);

    m_Position += SHORT_SIZE;
    return x;
}

short Lib::Network::ByteBufferV2::getShort()
{
    return static_cast<short>(getUnsignedShort());
}

unsigned int Lib::Network::ByteBufferV2::getUnsignedInt()
{
    errorIfEmpty();
    checkForOutOfBound(position() + INT_SIZE, true);

    // Get the element from the buffer
    unsigned int x;
    memcpy(&x, m_Buffer + m_Position, sizeof(unsigned int));

    // Converts if necessary
    if (m_Order == ByteOrder::BigEndian) x = be32toh(x);
    if (m_Order == ByteOrder::LittleEndian) x = le32toh(x);

    m_Position += INT_SIZE;
    return x;
}

int Lib::Network::ByteBufferV2::getInt()
{
    return static_cast<int>(getUnsignedInt());
}

unsigned long Lib::Network::ByteBufferV2::getUnsignedLong()
{
    errorIfEmpty();
    checkForOutOfBound(position() + INT_SIZE_64, true);

    // Get the element from the buffer
    unsigned long x;
    memcpy(&x, m_Buffer + m_Position, sizeof(unsigned long));

    // Converts if necessary
    if (m_Order == ByteOrder::BigEndian) x = be64toh(x);
    if (m_Order == ByteOrder::LittleEndian) x = le64toh(x);

    m_Position += INT_SIZE_64;
    return x;
}

long Lib::Network::ByteBufferV2::getLong()
{
    return static_cast<long>(getUnsignedLong());
}

void Lib::Network::ByteBufferV2::getBuffer(unsigned char *dst, const size_t start, const size_t size)
{
    errorIfEmpty();
    position(start);
    checkForOutOfBound(position() + size, true);

    memcpy(dst, m_Buffer + m_Position, size * sizeof(unsigned char));
    m_Position += size;
}

void Lib::Network::ByteBufferV2::getBuffer(unsigned char *dst, const size_t size)
{
    getBuffer(dst, position(), size);
}

void Lib::Network::ByteBufferV2::getBuffer(unsigned char *dst)
{
    getBuffer(dst, 0, m_Capacity);
}

unsigned char *Lib::Network::ByteBufferV2::getBuffer() const
{
    return m_Buffer;
}

void Lib::Network::ByteBufferV2::checkForOutOfBound(const size_t curr_pos, bool on_size)
{
    // If the current position is beyond the capacity error
    if (curr_pos > m_Capacity)
    {
        std::cerr << "[ByteBuffer:Error] Current position " << curr_pos
                  << " is beyond buffer capacity of " << m_Capacity
                  << std::endl;

        throw std::overflow_error("Error");
    }

    // Otherwise, if the current position is beyond the actual size
    if (curr_pos > m_Size && on_size)
    {
        std::cout << "[ByteBuffer:Warning] Current position " << curr_pos
                  << " is beyond the actual size of " << m_Size
                  << ". Undefined behaviour."
                  << std::endl;
    }
}

void Lib::Network::ByteBufferV2::errorIfEmpty()
{
    if (isEmpty())
    {
        std::cerr << "[ByteBuffer:Error] Trying to access an empty"
                  << " array of bytes. This lead to undefined behavior"
                  << std::endl;

        throw std::runtime_error("Error");
    }
}
