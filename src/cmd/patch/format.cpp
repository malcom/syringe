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
#include "cmd/patch/format.h"

namespace syringe {
namespace cmd {
namespace patch {

template<typename T>
struct FormatTrait {};

template<typename T>
inline T FormatValue(const char* value) {

	typedef FormatTrait<T> Trait;

	// start after unknown!
	for (int i = enum_cast<T>(T::Unknown) + 1; i < enum_cast<T>(T::FormatMax); i++) {
		if (stricmp(value, Trait::Strings[i]) == 0)
			return underlying_cast<T>(i);
	}

	return T::Unknown;
}

template<typename T>
inline const char* FormatString(T format) {

	typedef FormatTrait<T> Trait;

	if (format >= T::FormatMax)
		format = T::Unknown;

	return Trait::Strings[enum_cast<T>(format)];
}


template<>
struct FormatTrait<PatchFormat> {

	// TODO: C++11 should be constexpr static and initialized here,
	// but VC2k13 not suppoerr it yet

	static const char* const Strings[];

};

const char* const FormatTrait<PatchFormat>::Strings[] = {
	"unknown",
	"IDA",
};

static_assert(
	countof(FormatTrait<PatchFormat>::Strings) == static_cast<int>(PatchFormat::FormatMax),
	"Strings must contains string for all values"
);

template PatchFormat FormatValue<PatchFormat>(const char*);
template const char* FormatString<PatchFormat>(PatchFormat);


template<>
struct FormatTrait<TargetFormat> {

	static const char* const Strings[];

};

const char* const FormatTrait<TargetFormat>::Strings[] = {
	"unknown",
	"binary",
	"IntelHEX",
};


static_assert(
	countof(FormatTrait<TargetFormat>::Strings) == static_cast<int>(TargetFormat::FormatMax),
	"Strings must contains string for all values"
);

template TargetFormat FormatValue<TargetFormat>(const char*);
template const char* FormatString<TargetFormat>(TargetFormat);

} // namespace patch
} // namespace cmd
} // namespace syringe
