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

namespace syringe {

// RAII for Windows HANDLE type
struct CloseHandleDeleter {
	typedef HANDLE pointer;
	void operator()(HANDLE handle) const { ::CloseHandle(handle); }
};

typedef std::unique_ptr<HANDLE, CloseHandleDeleter> Handle;


typedef unsigned int Pid;
const Pid INVALID_PID = static_cast<Pid>(-1);

} // namespace syringe

#endif // SYRINGE_TYPES_H