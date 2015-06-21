/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_BASE_CODEOPTS_H
#define SYRINGE_CMD_BASE_CODEOPTS_H

#include "cmd/command.h"
#include "utils/enc.h"
#include "utils.h"

namespace syringe {

// Overload the boost.po 'validate' function for the CodeBuffer type validation and parsing
void validate(boost::any& v, const std::vector<std::string>& values, CodeBuffer*, int);

} // namespace syringe

namespace syringe {
namespace cmd {
namespace base {

class CodeOpts {
public:

	void AddOptions(Command::OptionsDesc& desc) {
		desc.add_options()
			("code", po::value<CodeBuffer>(&m_code), "Code to inject")
			;
	}

	void NotifyOptions(Command::OptionsMap& /*map*/) {}

	void Logo(std::ostream& stream) const {
		stream << "Shellcode: " << "size: " << std::dec << m_code.size() << "\n";
	}

	const CodeBuffer& Code() const {
		return m_code;
	}

private:

	CodeBuffer m_code;

};

} // namespace base
} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_BASE_CODEOPTS_H