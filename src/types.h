/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_TYPES_H
#define SYRINGE_TYPES_H

#include <windows.h>
#include <memory>
#include <vector>

namespace syringe {

// RAII for Windows HANDLE type
struct CloseHandleDeleter {
	typedef HANDLE pointer;
	void operator()(HANDLE handle) const { ::CloseHandle(handle); }
};

typedef std::unique_ptr<HANDLE, CloseHandleDeleter> Handle;


typedef unsigned int Pid;
const Pid INVALID_PID = static_cast<Pid>(-1);


//typedef std::vector<unsigned char> CodeBuffer;
class CodeBuffer : public std::vector<unsigned char> {};

} // namespace syringe

// TODO: move to utils
// computes the count of any array
// C++ version of old-style macro
#define countof( array ) sizeof(array)/sizeof(array[0])

//template <typename T, std::size_t N>
//constexpr std::size_t countof(T const (&)[N]) /*noexcept*/ {
//	return N;
//}

// cast from enum type to underlying type
template<typename T, typename U = std::underlying_type<T>::type>
U enum_cast(T v) {
	return static_cast<U>(v);
}

// cast from underlying type to enum type
template<typename T, typename U = std::underlying_type<T>::type>
T underlying_cast(U v) {
	return static_cast<T>(v);
}

#endif // SYRINGE_TYPES_H