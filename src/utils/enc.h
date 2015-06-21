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
 * Convert hex digit to dec number.
 * Return converted value if successful or an error value (-1) on error.
 * Note: The function conver only one hex value - nibble (half-byte).
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
 * Decode shell-code from \xDE\xAD representation to binary 0xDE 0xAD data.
 * Returns zero if successful or an error otherwise. The error value could
 * be -1 for genereal error or position of the first invalid character in input buffer.
 * Note: The caller is responsible for ensuring that the out buffer has sufficient space.
 */
int DecodeShellcode(const char* in, size_t len, unsigned char* out);

} // namespace utils
} // namespace syringe

#endif // SYRINGE_UTILS_ENC_H