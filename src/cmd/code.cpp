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
#include "cmd/code.h"
#include "utils.h"
#include <iostream>

namespace syringe {
namespace cmd {

Code::Code() {

	ProcessOpts::AddOptions(m_optDesc);
	CodeOpts::AddOptions(m_optDesc);

}

void Code::Help(std::ostream& stream) const {
	DoFormatHelp(stream, "Usage: syringe code [options] <shellcode>", &m_optDesc);
}

void Code::Logo(std::ostream& stream) const {

	Command::Logo(stream);
	ProcessOpts::Logo(stream);
	CodeOpts::Logo(stream);

	stream << std::endl;
}

void Code::Parse(const OptionsList& opts) {

	po::positional_options_description pos;
	pos.add("code", 1);

	po::parsed_options parsed = po::command_line_parser(opts).
		options(m_optDesc).
		positional(pos).
		run();

	po::store(parsed, m_optMap);
	po::notify(m_optMap);

	ProcessOpts::NotifyOptions(m_optMap);
	CodeOpts::NotifyOptions(m_optMap);

}

int Code::Run() {

	std::cout << std::hex << std::uppercase;

	const CodeBuffer& code = CodeOpts::Code();

	Handle proc(
		::OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId())
	);

	if (!proc)
		throw std::runtime_error("Error open process");

	std::cout << "Process opened, handle: " << proc.get() << std::endl;


	std::shared_ptr<void> mem(
			::VirtualAllocEx(proc.get(), nullptr, utils::RoundToAlign(code.size(), 16), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE),
		[&](void* p) { ::VirtualFreeEx(proc.get(), p, 0, MEM_RELEASE); }
	);

	if (!mem)
		throw std::runtime_error("Error alocate memory in remote process");

	std::cout << "Memory in remote process allocated, address: " << mem.get() << std::endl;


	BOOL ret = ::WriteProcessMemory(proc.get(), mem.get(), code.data(), code.size(), nullptr);
	if (!ret)
		throw std::runtime_error("Error write to remote process memory");

	std::cout << "Code written in remote process memory" << std::endl;


	Handle thread(
		::CreateRemoteThread(proc.get(), nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(mem.get()), nullptr, 0, nullptr)
	);

	if (!thread)
		throw std::runtime_error("Error create remote thread");

	std::cout << "Remote thread created, handle: " << thread.get() << std::endl;


	std::cout << "Waiting for thread finish..." << std::endl;

	if (::WaitForSingleObject(thread.get(), INFINITE) == WAIT_FAILED)
		throw std::runtime_error("Error wait for thread");

	std::cout << "Thread finish" << std::endl;


	DWORD excode = 0;
	if (::GetExitCodeThread(thread.get(), &excode) != TRUE)
		throw std::runtime_error("Error get exit code for thread");

	std::cout << "Code successfully injected and executed, exit code: " << excode << std::endl;

	return 0;
}

SYRINGE_AUTO_REGISTER_COMMAND(Code);

} // namespace cmd
} // namespace syringe
