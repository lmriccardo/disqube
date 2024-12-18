#include <iostream>
#include <string>
#include <string.h>

#include <CommonLib/Communication/ByteBufferV2.hpp>
#include "Test.hpp"

using ByteBuffer = Lib::Network::ByteBufferV2;
using ByteOrder = Lib::Network::ByteBufferV2::ByteOrder;

using namespace Test;

void set_fields(ByteBuffer& bb)
{
    // Put values into the bytebuffer
    bb.put((unsigned char)0);
    bb.put((unsigned char)1);
    bb.put((unsigned short)0x1234);
    bb.put((unsigned int)0x1234abcd);

    std::string msg = "Ciao";
    bb.put((unsigned char*)msg.c_str(), msg.length());
}

void test_values(ByteBuffer& bb)
{
    // Obtain values from the byte buffer
    bb.position(0);
    assert_eq<unsigned char>(bb.get(), 0);
    assert_eq<unsigned char>(bb.get(), 1);
    assert_eq<unsigned short>(bb.getUnsignedShort(), 0x1234);
    assert_eq<unsigned int>(bb.getUnsignedInt(), 0x1234abcd);

    char msg[5];
    bb.getBuffer((unsigned char*)msg, 4);
    assert_eq_str(msg, "Ciao", 5);
}

void test_be(ByteBuffer& bb)
{
    std::cout << "[TEST 1/3] Testing BigEndian Byte Order" << std::endl;
    bb.clear();
    bb.setByteOrder(ByteOrder::BigEndian);
    set_fields(bb);
    test_values(bb);
}

void test_le(ByteBuffer& bb)
{
    std::cout << "[TEST 2/3] Testing LittleEndian Byte Order" << std::endl;
    bb.clear();
    bb.setByteOrder(ByteOrder::LittleEndian);
    set_fields(bb);
    test_values(bb);
}

void test_copy_constr(ByteBuffer& bb)
{
    std::cout << "[TEST 3/3] Testing Copy Constructor" << std::endl;
    bb.position(0);

    ByteBuffer bb_c(bb);
    assert_eq<std::size_t>(bb_c.getBufferCapacity(), bb.getBufferCapacity());
    assert_eq<std::size_t>(bb_c.getBufferSize(), bb.getBufferSize());
    assert_eq<std::size_t>(bb_c.position(), bb.position());

    // Check that modifications do not affect the other one
    bb_c.position(1);
    bb_c.put((unsigned char)2);
    
    bb_c.position(1);
    bb.position(1);

    assert_neq<unsigned char>(bb_c.get(), bb.get());
}

int main()
{
    ByteBuffer bb(16);
    test_be(bb);
    test_le(bb);
    test_copy_constr(bb);

    return 0;
}