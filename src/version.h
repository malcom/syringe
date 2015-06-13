/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_VERSION_H
#define SYRINGE_VERSION_H

#include "defs.h"

#define SYRINGE_VERSION_MAJOR	0
#define SYRINGE_VERSION_MINOR	1
#define SYRINGE_VERSION_RELEASE	0

namespace syringe {

const unsigned int VersionNumber =
	((SYRINGE_VERSION_MAJOR   & 0xFF)   << 24) |
	((SYRINGE_VERSION_MINOR   & 0x0FFF) << 12) |
	((SYRINGE_VERSION_RELEASE & 0x0FFF));

const char* VersionString =
	SYRINGE_STR_M(SYRINGE_VERSION_MAJOR) "." \
	SYRINGE_STR_M(SYRINGE_VERSION_MINOR) "." \
	SYRINGE_STR_M(SYRINGE_VERSION_RELEASE);

} // namespace syringe

#endif // SYRINGE_VERSION_H