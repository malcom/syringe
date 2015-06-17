/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_DLL_H
#define SYRINGE_CMD_DLL_H

#include "cmd/command.h"
#include "cmd/base/processopts.h"

namespace syringe {
namespace cmd {

class Dll : public Command, private base::ProcessOpts {
public:

	Dll();

	std::string Name() const {
		return "dll";
	}

	std::string Desc() const {
		return "Inject (load) dll into process";
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

#endif // SYRINGE_CMD_DLL_H