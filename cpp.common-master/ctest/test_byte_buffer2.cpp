#include "catch2/catch.hpp"

#include "byte_buffer2.hpp"
#include <string>
#include <iostream>

using namespace std;

TEST_CASE("ByteBuffer2 interface") {
  auto arr   = new uint8_t[10] { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
  auto minus = new uint8_t[10] { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00 };
  auto src   = new uint8_t[ 5] { 'l', 'e', 'e', 'c', 'h' };

  SECTION("set_owner") {
    ByteBuffer2 bb(arr, 1, 5, false);
    bb.set_owner();
    REQUIRE(bb.size() == 5);
    REQUIRE(bb.offset() == 0);
    REQUIRE(bb.begin() == 0);
    REQUIRE(bb[0] == 0x02);
  }

  SECTION("append 1") {
    ByteBuffer2 bb(arr, 0, 10, true);
    bb.append(src, 0, 6);
    REQUIRE(bb.size() == 16);
    REQUIRE(bb.limit() == 16);
    REQUIRE(bb.capacity() == 4096);
    REQUIRE(bb.offset() == 0);

    REQUIRE(bb[10] == 'l');
  }

  SECTION("append 2") {
    ByteBuffer2 bb0(arr, 0, 10, true);
    ByteBuffer2 bb1(src, 0, 5, true);

    bb0.append(bb1);
    REQUIRE(bb0.size() == 15);
    REQUIRE(bb0.offset() == 0);
    REQUIRE(bb0.limit() == 15);
    REQUIRE(bb0.capacity() == 4096);

    REQUIRE(bb0[14] == 'h');
  }

  SECTION("append to empty buffer") {
    ByteBuffer2 bb0(nullptr, 0, 0, true);
    ByteBuffer2 bb1(src, 0, 5, true);

    bb0.append(bb1);
    bb0.append(bb1);
    REQUIRE(bb0.size() == 10);
    REQUIRE(bb0.offset() == 0);
    REQUIRE(bb0.limit() == 10);
    REQUIRE(bb0.capacity() == 4096);
  }

  SECTION("append owned buffer to an owned buffer") {
    ByteBuffer2 bb0(arr, 0, 10, true);
    auto bb1 = new ByteBuffer2(src, 0, 5, true);

    bb0.append(bb1);
    REQUIRE(bb0.size() == 15);
    REQUIRE(bb0.offset() == 0);
    REQUIRE(bb0.limit() == 15);
    REQUIRE(bb0.capacity() == 4096);
  }

  SECTION("CtorWithInit") {
    ByteBuffer2 b = { 1, 2, 3 };

    REQUIRE(b.offset() == 0);
    REQUIRE(b.size() == 3);
    REQUIRE(b.get_hex_string(3) == "010203");
  }

  SECTION("CtorBeginEnd") {
    ByteBuffer2 b(src, 5);

    REQUIRE(b.to_s() == "leech");
  }

  SECTION("GetInt8") {
    ByteBuffer2 b = { 1, 2, 3 };

    REQUIRE(b.get_uint8(2) == 3);
    REQUIRE(b.get_int8(2) == 3);
    REQUIRE(b.size() == 3);
    REQUIRE(b[0] == 1);
  }

  SECTION("ToS") {
    ByteBuffer2 b(string("leech"));
    REQUIRE(b.to_s() == "leech");
    REQUIRE(b.offset() == 0);
  }

  SECTION("HasRemaining") {
    ByteBuffer2 b(arr, 0, 10, false);

    for (int i=0; i<10; i++)
    {
      REQUIRE(b.has_remaining());
      b.get_int8();
    }
  }

  SECTION("GetInt16BE") {
    ByteBuffer2 b(arr, 0, 5);

    REQUIRE(0x0102 == b.get_int16_be());
    REQUIRE(0x0304 == b.get_int16_be());
    REQUIRE_THROWS_AS(b.get_int16_be(), out_of_range);
    b.reset();
    REQUIRE(0x0102 == b.get_uint16_be());
    REQUIRE(0x0304 == b.get_uint16_be());
    REQUIRE_THROWS_AS(b.get_int16_be(), out_of_range);

    b.reset();
    ByteBuffer2 m(minus, 0, 5);
    REQUIRE(-1 == m.get_int16_be());
    REQUIRE(2 ==  m.offset());
  }

  SECTION("GetInt24LE") {
    ByteBuffer2 b(arr, 0, 6);

    REQUIRE(0x030201 == b.get_int24_le());
    REQUIRE(0x060504 == b.get_int24_le());
    b.reset();
    REQUIRE(0x030201 == b.get_uint24_le());
    REQUIRE(0x060504 == b.get_uint24_le());

    b.reset();
    ByteBuffer2 m(minus, 0, 6);
    REQUIRE(-1 == m.get_int24_be());
    REQUIRE(-1 == m.get_int24_be());
    REQUIRE_THROWS_AS(m.get_int24_be(), out_of_range);
  }

  SECTION("GetInt24BE") {
    ByteBuffer2 b(arr, 0, 6);

    REQUIRE(0x010203 == b.get_int24_be());
    REQUIRE(0x040506 == b.get_int24_be());
    b.reset();
    REQUIRE(0x010203 == b.get_uint24_be());
    REQUIRE(0x040506 == b.get_uint24_be());

    b.reset();
    ByteBuffer2 m(minus, 0, 6);
    REQUIRE(-1 == m.get_int24_be());
    REQUIRE(-1 == m.get_int24_be());
    REQUIRE_THROWS_AS(m.get_int24_be(), out_of_range);
  }

  SECTION("GetInt32BE") {
    ByteBuffer2 b(arr, 0, 8);

    REQUIRE(0x01020304 == b.get_int32_be());
    REQUIRE(0x05060708 == b.get_int32_be());
    REQUIRE_THROWS_AS(b.get_int16_be(), out_of_range);

    b.reset();
    REQUIRE(0x01020304 == b.get_uint32_be());
    REQUIRE(0x05060708 == b.get_uint32_be());
    REQUIRE_THROWS_AS(b.get_int16_be(), out_of_range);

    b.reset();
    ByteBuffer2 m(minus, 0, 5);
    REQUIRE(-1 == m.get_int32_be());
    REQUIRE( 4 == m.offset());
  }

  SECTION("GetInt40BE") {
    ByteBuffer2 b(arr, 0, 8);

    REQUIRE(0x0102030405 == b.get_int40_be());

    b.reset();
    REQUIRE(0x0102030405 == b.get_uint40_be());

    b.reset();
    REQUIRE(0x0504030201 == b.get_int40_le());

    b.reset();
    REQUIRE(0x0504030201 == b.get_uint40_le());

    b.reset();
    ByteBuffer2 m(minus, 0, 8);
    REQUIRE(-1 == m.get_int40_be());
    REQUIRE_THROWS_AS(m.get_int40_be(), out_of_range);
  }

  SECTION("GetInt56BE") {
    ByteBuffer2 b(arr, 0, 8);

    REQUIRE(0x01020304050607 == b.get_int56_be());

    b.reset();
    REQUIRE(0x01020304050607 == b.get_uint56_be());

    b.reset();
    REQUIRE(0x07060504030201 == b.get_int56_le());

    b.reset();
    REQUIRE(0x07060504030201 == b.get_uint56_le());

    b.reset();
    ByteBuffer2 m(minus, 0, 8);
    REQUIRE(-1 == m.get_int56_be());
    REQUIRE_THROWS_AS(m.get_int56_be(), out_of_range);
  }

  SECTION("GetInt64BE") {
    ByteBuffer2 b(arr, 0, 8);

    REQUIRE(0x0102030405060708 == b.get_int64_be());
    REQUIRE_THROWS_AS(b.get_int16_be(), out_of_range);

    b.reset();
    REQUIRE(0x0102030405060708 == b.get_uint64_be());
    REQUIRE_THROWS_AS(b.get_int16_be(), out_of_range);

    b.reset();
    ByteBuffer2 m(minus, 0, 8);
    REQUIRE(-1 == m.get_int64_be());
    REQUIRE(8 ==  m.offset());
  }

  SECTION("VarInt") {
    ByteBuffer2 b0 = { 0x81, 0x7f };

    auto [r0, s0] = b0.get_varint_with_size();
    REQUIRE(2 ==  s0);
    REQUIRE(0xff == r0);

    b0.reset({ 0x83, 0xff, 0x7f });
    auto [r1, s1] = b0.get_varint_with_size();
    REQUIRE(3 ==  s1);
    REQUIRE(0xffff == r1);

    b0.reset({ 0x87, 0xff, 0xff, 0x7f });
    auto [r2, s2] = b0.get_varint_with_size();
    REQUIRE(4 ==  s2);
    REQUIRE(0xffffff == r2);

    b0.reset({ 0x8f, 0xff, 0xff, 0xff, 0x7f });
    auto [r3, s3] = b0.get_varint_with_size();
    REQUIRE(5 ==  s3);
    REQUIRE(0xffffffff == r3);

    b0.reset({ 0x9f, 0xff, 0xff, 0xff, 0xff, 0x7f });
    auto [r4, s4] = b0.get_varint_with_size();
    REQUIRE(6 ==  s4);
    REQUIRE(0xffffffffff == r4);

    b0.reset({ 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f });
    auto [r5, s5] = b0.get_varint_with_size();
    REQUIRE(7 ==  s5);
    REQUIRE(0xffffffffffff == r5);

    b0.reset({ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f });
    auto [r6, s6] = b0.get_varint_with_size();
    REQUIRE(8 ==  s6);
    REQUIRE(0xffffffffffffff == r6);

    b0.reset({ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff });
    auto [r7, s7] = b0.get_varint_with_size();
    REQUIRE(9 ==  s7);
    REQUIRE(0xffffffffffffffff == r7);

    b0.reset({ 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff });
    auto r8 = b0.get_varint();
    REQUIRE(0xffffffffffffffff == r8);
  }

  SECTION("Skip") {
    ByteBuffer2 b(arr, 0, 10);
    REQUIRE(2 == b.skip(2).offset());
  }

  SECTION("Take") {
    ByteBuffer2 a("0123456789");
    REQUIRE(a.take(2).to_s() == string("01"));
    REQUIRE(a.take(2).to_s() == string("23"));
    REQUIRE(a.take(2).to_s() == string("45"));
    REQUIRE(a.offset() == 6);
  }

  SECTION("FirstN") {
    ByteBuffer2 b("leech");

    auto res1 = b.first(3);
    auto res2 = b.first(5);
    REQUIRE(res1.to_s() == string("lee"));
    REQUIRE(res2.to_s() == string("leech"));
  }

  SECTION("LastN") {
    ByteBuffer2 b("leech");

    REQUIRE(b.last() == uint8_t('h'));
    REQUIRE(b.last(3).to_s() == string("ech"));
    REQUIRE(b.last(5).to_s() == string("leech"));
  }

  SECTION("FromHexCode") {
    uint8_t arr[] = {
      0x92, 0xa7, 0x42, 0xab, 0x08, 0xc9, 0x69, 0xbf,
      0x00, 0x6c, 0x94, 0x12, 0xd3, 0xcc, 0x79, 0xa5 };

    auto b = ByteBuffer2::from_hexcode("92a742ab08c969bf006c9412d3cc79a5");
    for (int i=0; i<b.size(); i++)
      REQUIRE(arr[i]  == uint8_t(b[i]));
  }

  SECTION("Slice") {
    ByteBuffer2 a("0123456789");

    auto s0 = a.slice(4, 4);
    REQUIRE(s0.to_s() == string("4567"));
    REQUIRE(a.offset() == 0);

    auto s1 = a.slice(4, 0);
    REQUIRE(s1.to_s() == string(""));
    REQUIRE(a.offset() == 0);

    // deep copy test
    auto arr = new uint8_t[4];
    arr[0] = 1; arr[1] = 2; arr[2] = 3; arr[3] = 4;
    ByteBuffer2 b(arr, 0, 4, true);
    auto s2 = b.copy_slice(0, 2);

    //b.destroy();
    //REQUIRE(s2[0], 1);
    //REQUIRE(s2[1], 2);
  }

  SECTION("StartsWith") {
    ByteBuffer2 b("leech");
    REQUIRE(b.starts_with("lee"));
    REQUIRE_FALSE(b.starts_with("kee"));
    REQUIRE_THROWS_AS(b.starts_with("leeche"), out_of_range);
  }

  SECTION("GetAscii") {
    ByteBuffer2 b("leech");
    REQUIRE(b.get_ascii(3) == string("lee"));
    REQUIRE(b.get_ascii(2) == string("ch"));
    REQUIRE_FALSE(b.has_remaining());
  }

  // TODO
  // Waiting for compiler update
  //
  SECTION("GetUnicode16Le") {
    ByteBuffer2 b = { 0x53, 0x00, 0x79, 0x00, 0x73, 0x00 };
    REQUIRE(b.get_unicode16_le(3) == string("Sys"));
    //REQUIRE_THROWS_AS(b.get_unicode16_le(3), logic_error);
  }

  SECTION("GetDouble") {
    ByteBuffer2 bb = { 0x3f, 0xf3, 0xae, 0x14, 0x7a, 0xe1, 0x47, 0xae };
    REQUIRE(bb.get_double() == 1.23);
  }

  SECTION("GetHexString") {
    ByteBuffer2 bb = { 0x00, 0x01, 0x02, 0x03};
    REQUIRE(bb.get_hex_string(3) == "000102"s);
    REQUIRE(bb.offset() == 3);
  }

  SECTION("SliceWithAssign") {
    ByteBuffer2 bb = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

    ByteBuffer2 s;
    s = bb.slice(3, 2);

    REQUIRE(s.size() == 2);
    REQUIRE(s.offset() == 3);
    REQUIRE(s.limit() == 5);
    REQUIRE(s[0] == 0x03);
    REQUIRE(s[1] == 0x04);
  }

  SECTION("CopySliceWithAssign") {
    ByteBuffer2 bb = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

    ByteBuffer2 s;
    s = bb.copy_slice(3, 2);

    REQUIRE(s.size() == 2);
    REQUIRE(s.offset() == 0);
    REQUIRE(s.limit() == 2);
    REQUIRE(s[0] == 0x03);
    REQUIRE(s[1] == 0x04);
  }
}
