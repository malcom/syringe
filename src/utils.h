/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_UTILS_H
#define SYRINGE_UTILS_H

#include <string>
#include <functional>

#include <tlhelp32.h>

namespace syringe {
namespace utils {

inline size_t RoundToAlign(size_t size, size_t align) {
	return (size + align - 1) & ~(align - 1);
}

typedef std::function<int(const PROCESSENTRY32* pe)> EnumProcFunc;

int EnumProcesses(EnumProcFunc func);

int GetProcessId(const std::string& name);

std::string GetProcessName(int pid);

} // namespace utils
} // namespace syringe

#endif // SYRINGE_UTILS_H