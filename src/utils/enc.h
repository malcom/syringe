/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_UTILS_ENC_H
#define SYRINGE_UTILS_ENC_H

#include "types.h"

namespace syringe {
namespace utils {

/*
 * Convert hexadecimal digit to decimal number.
 * Return converted value if successful or an error value (-1) otherwise.
 * Note: The function convert only nibble (half-byte) - from one hex-digit.
 */
inline unsigned char HexToDec(char digit) {

	if (digit >= '0' && digit <= '9')
		return digit - '0';
	if (digit >= 'a' && digit <= 'f')
		return digit - 'a' + 10;
	if (digit >= 'A' && digit <= 'F')
		return digit - 'A' + 10;

	return static_cast<unsigned char>(-1);
}

/*
 * Convert hexadecimal representation into binary data.
 * Returns zero if successful or an error otherwise. The error value could
 * be -1 for genereal error or position of the first invalid character in input buffer.
 * Note: The caller is responsible for ensuring that the out buffer has sufficient space.
 */
int HexToDec(const char* in, size_t len, unsigned char* out);

/*
 * Template function to convert hexadecimal representation into binary data of any types.
 */
template<typename T>
T HexToDec(const char* in, size_t len = sizeof(T) * 2) {

	T value;
	if (HexToDec(in, len, reinterpret_cast<unsigned char*>(&value)) != 0)
		throw std::runtime_error("Invalid format in input buffer");

	return value;
}


/*
 * Convert decimal number into hexadecimal digit.
 * Return converted value if successful or an error value (-1) otherwise.
 * Note: The function convert only nibble (half-byte) - to one hex-digit.
 */
inline char DecToHex(unsigned char nibble) {

#if 0
	if (nibble < 10)
		return nibble + '0';
	if (nibble < 17)
		return nibble + 'A';

	return static_cast<char>(-1);
#endif

	if (nibble > 16)
		return static_cast<char>(-1);

	static const char digits[] = "0123456789ABCDEF";

	return digits[nibble];
}

/*
 * Convert binary data into hexadecimal representation.
 * Returns zero if successful or an error otherwise. The error value could
 * be -1 for genereal error or position of the first invalid character in input buffer.
 * Note: The caller is responsible for ensuring that the out buffer has sufficient space.
 */
int DecToHex(const unsigned char* in, size_t len, char* out);

/*
 * Template function to convert binary data into hexadecimal representation of any types.
 */
template<typename T>
void DecToHex(T value, char* out, size_t len = sizeof(T)) {

	if (DecToHex(reinterpret_cast<unsigned char*>(&value), len, out) != 0)
		throw std::runtime_error("Invalid format in input buffer");

}


/*
 * Decode shell-code from \xDE\xAD representation to binary 0xDE 0xAD data.
 * Returns zero if successful or an error otherwise. The error value could
 * be -1 for genereal error or position of the first invalid character in input buffer.
 * Note: The caller is responsible for ensuring that the out buffer has sufficient space.
 */
int DecodeShellcode(const char* in, size_t len, unsigned char* out);

} // namespace utils
} // namespace syringe

#endif // SYRINGE_UTILS_ENC_H