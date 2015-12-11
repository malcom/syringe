/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_PATCH_FORMAT_H
#define SYRINGE_CMD_PATCH_FORMAT_H

namespace syringe {
namespace cmd {
namespace patch {

enum class PatchFormat : int {
	Unknown,
	IDA,
	FormatMax
};

enum class TargetFormat : int {
	Unknown,
	Binary,
	IntelHEX,
	FormatMax
};

template<typename T>
inline T FormatValue(const char* value);

template<typename T>
inline const char* FormatString(T format);

template<typename T, typename F>
inline void EnumFormatString(F func) {

	// start after unknown!
	for (int i = enum_cast<T>(T::Unknown) + 1; i < enum_cast<T>(T::FormatMax); i++)
		func(FormatString<T>(underlying_cast<T>(i)));

}

template<typename T>
std::string FormatStringList(const char* sep = ", ") {

	std::string str;
	EnumFormatString<T>(
		[&](const char* value) { str.append(value); str.append(sep); }
	);
	str.erase(str.size() - std::strlen(sep));
	return str;
}


} // namespace patch
} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_PATCH_FORMAT_H