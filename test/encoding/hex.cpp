/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include "utils/enc.h"

namespace utils = syringe::utils;
namespace data = boost::unit_test::data::monomorphic;


struct BytesData {
	size_t blen;
	unsigned char bin[16];
	size_t hlen;
	char hex[16];
};

const BytesData Byte[] = {
	{ // common hex values
		16, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
		16, { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' }
	},
	{ // special for hex to dec only
		6, { 10, 11, 12, 13, 14, 15, },
		6, { 'a', 'b', 'c', 'd', 'e', 'f' }
	},
	{ // incorect values
		6, { 45, 88, 121, 91, 38, 310 },
		6, { '-', 'X', 'y', '[', '&', '\n' }
	}
};


struct ChunkData {
	size_t blen;
	unsigned char bin[16];
	size_t hlen;
	char hex[33];
};

const ChunkData Chunk[] = {
	{
		16, { 0x4F, 0xF2, 0xC2, 0x38, 0xDA, 0xCF, 0x68, 0x1D, 0xD4, 0xEA, 0xEF, 0xA2, 0xBA, 0xEE, 0xB6, 0xD0 },
		32, {  "4F"  "F2"  "C2"  "38"  "DA"  "CF"  "68"  "1D"  "D4"  "EA"  "EF"  "A2"  "BA"  "EE"  "B6"  "D0" }
	},
	{
		8,  { 0xE0, 0xFF, 0xEF, 0x52, 0x80, 0xC8, 0xDD, 0xA0 },
		16, {  "E0"  "FF"  "EF"  "52"  "80"  "C8"  "DD"  "A0" }
	},
	{
		12, { 0x68, 0x0C, 0xD5, 0x76, 0x0E, 0x89, 0xC2, 0x16, 0xEA, 0x00, 0xEE, 0xFF },
		24, {  "68"  "0C"  "D5"  "76"  "0E"  "XX"  "C2"  "16"  "EA"  "00"  "EE"  "FF" }
	}
};

struct VarData {

	union {
		uint32_t dummy;
		uint8_t uint8;
		uint16_t uint16;
		uint32_t uint32;
	} bin;

	union {
		char dummy[9];
		char uint8[3];
		char uint16[5];
		char uint32[9];
	} hex;

};

const VarData Var[] = {

	{ 0x1A, "1A" },
	{ 0x1A2B, "2B1A" },
	{ 0x1A2B3C4D, "4D3XYB1A" },

};


BOOST_AUTO_TEST_SUITE(HexConversionSuite)

BOOST_DATA_TEST_CASE(ByteTest1,
	data::array<char>(Byte[0].hex, Byte[0].hlen) + data::array<char>(Byte[1].hex, Byte[1].hlen) ^
	data::array<unsigned char>(Byte[0].bin, Byte[0].blen) + data::array<unsigned char>(Byte[1].bin, Byte[1].blen),
	in, out)
{
	BOOST_CHECK(utils::HexToDec(in) == out);
}

BOOST_DATA_TEST_CASE(ByteTest2, data::array<char>(Byte[2].hex, Byte[2].hlen), in) {

	BOOST_CHECK(utils::HexToDec(in) == static_cast<unsigned char>(-1));
}

BOOST_AUTO_TEST_CASE(ChunkTest) {

	unsigned char out[16];
	const ChunkData* data;

	data = &Chunk[0];
	BOOST_CHECK(utils::HexToDec(data->hex, data->hlen, out) == 0);
	BOOST_CHECK(std::memcmp(data->bin, out, data->blen) == 0);

	data = &Chunk[1];
	BOOST_CHECK(utils::HexToDec(data->hex, data->hlen, out) == 0);
	BOOST_CHECK(std::memcmp(data->bin, out, data->blen) == 0);

	data = &Chunk[2];
	BOOST_CHECK(utils::HexToDec(data->hex, data->hlen, out) == 11);
	BOOST_CHECK(std::memcmp(data->bin, out, data->blen) == -1);

}

BOOST_AUTO_TEST_CASE(VarTest) {

	const VarData* data;

	data = &Var[0];
	uint8_t out8;
	BOOST_CHECK_NO_THROW(out8 = utils::HexToDec<uint8_t>(data->hex.uint8));
	BOOST_CHECK(out8 == data->bin.uint8);

	data = &Var[1];
	uint16_t out16;
	BOOST_CHECK_NO_THROW(out16 = utils::HexToDec<uint16_t>(data->hex.uint16));
	BOOST_CHECK(out16 == data->bin.uint16);

	data = &Var[2];
	uint32_t out32 = 0;
	BOOST_CHECK_THROW(out32 = utils::HexToDec<uint32_t>(data->hex.uint32), std::runtime_error);
	BOOST_CHECK(out32 != data->bin.uint32);

}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(DecConversionSuite)

BOOST_DATA_TEST_CASE(ByteTest1,
	data::array<unsigned char>(Byte[0].bin, Byte[0].blen) ^
	data::array<char>(Byte[0].hex, Byte[0].hlen),
	in, out)
{
	BOOST_CHECK(utils::DecToHex(in) == out);
}

BOOST_DATA_TEST_CASE(ByteTest2, data::array<unsigned char>(Byte[2].bin, Byte[2].blen), in) {

	BOOST_CHECK(utils::DecToHex(in) == static_cast<char>(-1));
}

BOOST_AUTO_TEST_CASE(ChunkTest) {

	char out[32];
	const ChunkData* data;

	data = &Chunk[0];
	BOOST_CHECK(utils::DecToHex(data->bin, data->blen, out) == 0);
	BOOST_CHECK(std::memcmp(data->hex, out, data->hlen) == 0);

	data = &Chunk[1];
	BOOST_CHECK(utils::DecToHex(data->bin, data->blen, out) == 0);
	BOOST_CHECK(std::memcmp(data->hex, out, data->hlen) == 0);

	data = &Chunk[2];
	BOOST_CHECK(utils::DecToHex(data->bin, data->blen, out) == 0);
	BOOST_CHECK(std::memcmp(data->hex, out, data->hlen) == 1);

}

BOOST_AUTO_TEST_CASE(VarTest) {

	char out[8];
	const VarData* data;

	data = &Var[0];
	BOOST_CHECK_NO_THROW(utils::DecToHex(data->bin.uint8, out));
	BOOST_CHECK(std::memcmp(data->hex.uint8, out, sizeof(data->hex.uint8) - 1) == 0);

	data = &Var[1];
	BOOST_CHECK_NO_THROW(utils::DecToHex(data->bin.uint16, out));
	BOOST_CHECK(std::memcmp(data->hex.uint16, out, sizeof(data->hex.uint16) - 1) == 0);

	data = &Var[2];
	BOOST_CHECK_NO_THROW(utils::DecToHex(data->bin.uint32, out));
	BOOST_CHECK(std::memcmp(data->hex.uint32, out, sizeof(data->hex.uint32) - 1) == 1);

}

BOOST_AUTO_TEST_SUITE_END()
