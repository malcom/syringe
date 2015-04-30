/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 *
 * Released under the MIT License.
 */

#include <iostream>
#include <string>
#include <memory>
#include <functional>
#include <cstring>

#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>

struct CloseHandleDeleter {
	typedef HANDLE pointer;
	void operator()(HANDLE handle) const { ::CloseHandle(handle); }
};

typedef std::unique_ptr<HANDLE, CloseHandleDeleter> Handle;

inline size_t RoundToAlign(size_t size, size_t align) {
	return (size + align - 1) & ~(align - 1);
}

typedef std::function<int(const PROCESSENTRY32* pe)> EnumProcFunc;

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

int GetProcessId(const std::string& name) {

	int pid = 0;

	EnumProcesses([&](const PROCESSENTRY32* pe) {

		if (_stricmp(pe->szExeFile, name.c_str()) == 0) {
			pid = pe->th32ProcessID;
			return 1;
		}
		return 0;

	});

	return pid;
}

std::string GetProcessName(int pid) {

	std::string name;

	EnumProcesses([&](const PROCESSENTRY32* pe) {

		if (pe->th32ProcessID == static_cast<unsigned int>(pid)) {
			name.assign(pe->szExeFile);
			return 1;
		}
		return 0;

	});

	return name;
}

// Todo: rewrite to use boost-options

struct CmdOpt {
	unsigned int pid;
	std::string name;
	std::string dll;
};

int ParseCmdOpt(CmdOpt& opt, int argc, char* argv[]) {

	if (argc != 3) {
		std::cout
			<< "syringe v0.1\n"
			<< "usage: syringe <proc-pid-or-name> <dll-name>"
			<< std::endl;
		return 1;
	}

	char* end;
	opt.pid = std::strtoul(argv[1], &end, 10);
	if (argv[1] == end) {
		opt.pid = 0;
		opt.name = argv[1];
	}

	opt.dll = argv[2];

	return 0;
}


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


int main(int argc, char* argv[]) {

	try {

		std::cout << std::hex << std::uppercase;

		CmdOpt cmd;
		if (ParseCmdOpt(cmd, argc, argv) != 0)
			return 1;

		if (cmd.pid == 0)
			cmd.pid = GetProcessId(cmd.name);

		if (cmd.pid == 0)
			throw std::runtime_error("Error find specified process");

		if (cmd.name.empty())
			cmd.name = GetProcessName(cmd.pid);

		if (cmd.name.empty())
			throw std::runtime_error("Error find specified process");

		std::cout
			<< "PID:  " << std::dec << cmd.pid << std::hex << "\n"
			<< "Name: " << cmd.name << "\n"
			<< "DLL:  " << cmd.dll << "\n"
			<< std::endl;


		Handle proc(
			::OpenProcess(PROCESS_ALL_ACCESS, FALSE, cmd.pid)
		);

		if (!proc)
			throw std::runtime_error("Error open process");

		std::cout << "Process opened, handle: " << proc.get() << std::endl;


		const size_t sizeData = RoundToAlign(sizeof(LoadDllThreadData) + cmd.dll.size() + 1, 16);
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

		ret = ::WriteProcessMemory(proc.get(), memory + sizeof(LoadDllThreadData), cmd.dll.c_str(), cmd.dll.size() + 1, nullptr);
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

	} catch (std::exception& e) {
		std::cerr << e.what() << "\n" << "Error code: " << ::GetLastError() << std::endl;
		return 1;
	}

	return 0;
}
