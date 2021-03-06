/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_DEFS_H
#define SYRINGE_DEFS_H

// disable annoying warnings
#pragma warning(disable: 4512) // assignment operator could not be generated

// eliminate CRT deprecation warnings
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

// stringize macros
#define SYRINGE_STR(x)		#x
#define SYRINGE_STR_M(x)	SYRINGE_STR(x)

// some common spacial types
#include "types.h"

#endif // SYRINGE_DEFS_H