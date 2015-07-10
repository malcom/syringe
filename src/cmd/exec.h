/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_EXEC_H
#define SYRINGE_CMD_EXEC_H

#include "cmd/command.h"
#include "cmd/base/processopts.h"
#include "cmd/base/codeopts.h"

namespace syringe {
namespace cmd {

class Exec : public Command, private base::CodeOpts {
public:

	Exec();

	std::string Name() const {
		return "exec";
	}

	std::string Desc() const {
		return "Execute code";
	}

	void Help(std::ostream& stream) const;
	void Logo(std::ostream& stream) const;

	void Parse(const OptionsList& opts);
	int Run();

private:
	OptionsDesc m_optDesc;
	OptionsMap m_optMap;
};

} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_EXEC_H