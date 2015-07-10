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
#include "cmd/exec.h"
#include "utils.h"
#include <iostream>

namespace syringe {
namespace cmd {

Exec::Exec() {

	CodeOpts::AddOptions(m_optDesc, false);

}

void Exec::Help(std::ostream& stream) const {
	DoFormatHelp(stream, "Usage: syringe exec [options] <shellcode>", &m_optDesc);
}

void Exec::Logo(std::ostream& stream) const {

	Command::Logo(stream);
	CodeOpts::Logo(stream);

	stream << std::endl;
}

void Exec::Parse(const OptionsList& opts) {

	po::positional_options_description pos;
	pos.add("code", 1);

	po::parsed_options parsed = po::command_line_parser(opts).
		options(m_optDesc).
		positional(pos).
		run();

	po::store(parsed, m_optMap);
	po::notify(m_optMap);

	CodeOpts::NotifyOptions(m_optMap);

}

int Exec::Run() {

	std::cout << std::hex << std::uppercase;

	const CodeBuffer& code = CodeOpts::Code();

	std::shared_ptr<void> mem(
			::VirtualAlloc(nullptr, utils::RoundToAlign(code.size(), 16), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE),
		[&](void* p) { ::VirtualFree(p, 0, MEM_RELEASE); }
	);

	if (!mem)
		throw std::runtime_error("Error alocate memory in process");

	std::cout << "Memory in process allocated, address: " << mem.get() << std::endl;


	std::memcpy(mem.get(), code.data(), code.size());
	std::cout << "Code written in process memory" << std::endl;


	Handle thread(
		::CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(mem.get()), nullptr, 0, nullptr)
	);

	if (!thread)
		throw std::runtime_error("Error create thread");

	std::cout << "Thread created, handle: " << thread.get() << std::endl;


	std::cout << "Waiting for thread finish..." << std::endl;

	if (::WaitForSingleObject(thread.get(), INFINITE) == WAIT_FAILED)
		throw std::runtime_error("Error wait for thread");

	std::cout << "Thread finish" << std::endl;


	DWORD excode = 0;
	if (::GetExitCodeThread(thread.get(), &excode) != TRUE)
		throw std::runtime_error("Error get exit code for thread");

	std::cout << "Code successfully executed, exit code: " << excode << std::endl;

	return 0;
}

SYRINGE_AUTO_REGISTER_COMMAND(Exec);

} // namespace cmd
} // namespace syringe
