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
#include "utils.h"

#include <tlhelp32.h>

namespace syringe {
namespace utils {

int EnumProcesses(EnumProcFunc func) {

	Handle snapshot(
		::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
	);

	if (snapshot.get() == INVALID_HANDLE_VALUE)
		throw std::logic_error("Error open process snapshot");

	PROCESSENTRY32 pe = { 0 };
	pe.dwSize = sizeof(PROCESSENTRY32);

	BOOL ret = ::Process32First(snapshot.get(), &pe);
	while (ret) {

		if (func(&pe) != 0)
			return 1;

		ret = ::Process32Next(snapshot.get(), &pe);
	}

	return 0;
}

Pid GetProcessId(const std::string& name) {

	int pid = INVALID_PID;

	EnumProcesses([&](const PROCESSENTRY32* pe) {

		if (_stricmp(pe->szExeFile, name.c_str()) == 0) {
			pid = pe->th32ProcessID;
			return 1;
		}
		return 0;

	});

	return pid;
}

std::string GetProcessName(Pid pid) {

	std::string name;

	EnumProcesses([&](const PROCESSENTRY32* pe) {

		if (pe->th32ProcessID == pid) {
			name.assign(pe->szExeFile);
			return 1;
		}
		return 0;

	});

	return name;
}

} // namespace utils
} // namespace syringe
