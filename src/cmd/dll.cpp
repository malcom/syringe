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
#include "cmd/dll.h"
#include "utils.h"

#include <iostream>


typedef DWORD (WINAPI *GetLastErrorFunc)(VOID);
typedef HMODULE (WINAPI *LoadLibraryFunc)(LPCSTR lpLibFileName);

struct LoadDllThreadData {

	// input
	LoadLibraryFunc LoadLibrary;
	GetLastErrorFunc GetLastError;
	CHAR* DllName;

	// output
	HMODULE ModuleHandle;
	DWORD LastError;

};

// Hack!
// This is function to used in remote thread to load dll
// Todo: rewrite to opcode/shellcode
static void RemoteLoadDllFunction(LoadDllThreadData* data) {

	data->ModuleHandle = data->LoadLibrary(data->DllName);
	data->LastError = data->GetLastError();

}

// This is just a dummy function used to determine size of RemoteLoadDllFunction code
static void RemoteLoadDllFunctionEnd() {
	return;
}


namespace syringe {
namespace cmd {

Dll::Dll() {

	AddOptions(m_optDesc);

	m_optDesc.add_options()
		("dll-name", po::value<std::string>()->required(), "DLL filename or path to inject")
		;

}

void Dll::Parse(const OptionsList& opts) {

	po::positional_options_description pos;
	pos.add("dll-name", 1);

	po::parsed_options parsed = po::command_line_parser(opts).
		options(m_optDesc).
		positional(pos).
		run();

	po::store(parsed, m_optMap);
	po::notify(m_optMap);

	NotifyOptions(m_optMap);

}

int Dll::Run() {

	std::cout << std::hex << std::uppercase;

	const std::string& dll = m_optMap["dll-name"].as<std::string>();

		std::cout
			<< "PID:  " << std::dec << ProcessId() << std::hex << "\n"
			<< "Name: " << ProcessName() << "\n"
			<< "DLL:  " << dll << "\n"
			<< std::endl;

	Handle proc(
		::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId())
	);

	if (!proc)
		throw std::runtime_error("Error open process");

	std::cout << "Process opened, handle: " << proc.get() << std::endl;


	const size_t sizeData = utils::RoundToAlign(sizeof(LoadDllThreadData) + dll.size() + 1, 16);
	const size_t sizeCode = reinterpret_cast<size_t>(RemoteLoadDllFunctionEnd) - reinterpret_cast<size_t>(RemoteLoadDllFunction);

	std::shared_ptr<void> mem(
			::VirtualAllocEx(proc.get(), nullptr, sizeData + sizeCode, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE),
		[&](void* p) { ::VirtualFreeEx(proc.get(), p, 0, MEM_RELEASE); }
	);

	if (!mem)
		throw std::runtime_error("Error alocate memory in remote process");

	std::cout << "Memory in remote process allocated, address: " << mem.get() << std::endl;

	// for friendlly pointer aritmetic without casting ;)
	char* memory = reinterpret_cast<char*>(mem.get());

	LoadDllThreadData data;

	data.LoadLibrary = reinterpret_cast<LoadLibraryFunc>(
		::GetProcAddress(::GetModuleHandle("kernel32.dll"), "LoadLibraryA")
	);

	data.GetLastError = reinterpret_cast<GetLastErrorFunc>(
		::GetProcAddress(::GetModuleHandle("kernel32.dll"), "GetLastError")
	);

	data.DllName = memory + sizeof(LoadDllThreadData);

	data.ModuleHandle = nullptr;
	data.LastError = 0;

	BOOL ret = ::WriteProcessMemory(proc.get(), memory, &data, sizeof(data), nullptr);
	if (!ret)
		throw std::runtime_error("Error write to remote process memory");

	ret = ::WriteProcessMemory(proc.get(), memory + sizeof(LoadDllThreadData), dll.c_str(), dll.size() + 1, nullptr);
	if (!ret)
		throw std::runtime_error("Error write to remote process memory");

	std::cout << "Data written in remote process memory" << std::endl;

	ret = ::WriteProcessMemory(proc.get(), memory + sizeData, RemoteLoadDllFunction, sizeCode, nullptr);
	if (!ret)
		throw std::runtime_error("Error write to remote process memory");

	std::cout << "Code written in remote process memory" << std::endl;


	Handle thread(
		::CreateRemoteThread(proc.get(), nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(memory + sizeData), memory, 0, nullptr)
	);

	if (!thread)
		throw std::runtime_error("Error create remote thread");

	std::cout << "Remote thread created, handle: " << thread.get() << std::endl;


	std::cout << "Waiting for thread finish..." << std::endl;

	if (::WaitForSingleObject(thread.get(), INFINITE) == WAIT_FAILED)
		throw std::runtime_error("Error wait for thread");

	std::cout << "Thread finish" << std::endl;

	ret = ::ReadProcessMemory(proc.get(), memory, &data, sizeof(data), nullptr);
	if (!ret)
		throw std::runtime_error("Error read from remote process memory");

	if (!data.ModuleHandle)
		std::cout << "DLL not injected! LoadLibrary failed with code: " << data.LastError << std::endl;
	else
		std::cout << "DLL successfully injected, handle: " << data.ModuleHandle << std::endl;

	return 0;
}

SYRINGE_AUTO_REGISTER_COMMAND(Dll);

} // namespace cmd
} // namespace syringe
