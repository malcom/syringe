/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_BASE_PROCESSOPTS_H
#define SYRINGE_CMD_BASE_PROCESSOPTS_H

#include "cmd/command.h"
#include "utils.h"

namespace syringe {
namespace cmd {
namespace base {

class ProcessOpts {
public:

	void AddOptions(Command::OptionsDesc& desc) {
		m_pid = INVALID_PID;
		desc.add_options()
			("pid", po::value(&m_pid), "Process identifiator to inject into")
			("name", po::value(&m_name), "Process name to inject into")
			;
	}

	void NotifyOptions(Command::OptionsMap& map) {

		if (!map.count("pid") && !map.count("name"))
			throw po::error("the option '--pid' or '--name' is required but missing");


		if (m_pid == INVALID_PID)
			m_pid = utils::GetProcessId(m_name);

		if (m_pid == INVALID_PID)
			throw std::runtime_error("Error find specified process");

		if (m_name.empty())
			m_name = utils::GetProcessName(m_pid);

		if (m_name.empty())
			throw std::runtime_error("Error find specified process");

	}

	void Logo(std::ostream& stream) const {
		stream << "Target process: " << "pid: " << std::dec << ProcessId() << std::hex << " name: " << ProcessName() << "\n";
	}

	Pid ProcessId() const {
		return m_pid;
	}

	const std::string& ProcessName() const {
		return m_name;
	}

private:
	Pid m_pid;
	std::string m_name;

};

} // namespace base
} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_BASE_PROCESSOPTS_H