/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#include "defs.h"
#include "utils/enc.h"

namespace syringe {
namespace utils {

int DecodeShellcode(const char* in, size_t len, unsigned char* out) {
	
	if ((len % 4)) {
		// invalid bytes?
		return -1;
	}

	const char* it = in;
	const char* end = in + len;

	while (it != end) {

		if (*it++ != '\\')
			goto error;

		char c = *it++;
		if (c != 'x' && c != 'X')
			goto error;

		char bh = HexToDec(*it++);
		if (bh == -1)
			goto error;

		char bl = HexToDec(*it++);
		if (bl == -1)
			goto error;

		*out++ = (bh & 0x0F) << 4 | bl & 0x0F;
	}

	return 0;

error:
	return it - in;
}

} // namespace utils
} // namespace syringe
