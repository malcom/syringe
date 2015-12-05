/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_PATCH_H
#define SYRINGE_CMD_PATCH_H

#include "cmd/command.h"
#include "cmd/patch/format.h"

namespace syringe {
namespace cmd {

class Patch : public Command {
public:

	Patch();

	std::string Name() const {
		return "patch";
	}

	std::string Desc() const {
		return "Patch file";
	}

	void Help(std::ostream& stream) const;
	void Logo(std::ostream& stream) const;

	void Parse(const OptionsList& opts);
	int Run();

private:
	OptionsDesc m_optDesc;

	std::string m_patchFile;
	std::string m_targetFile;

	patch::PatchFormat m_patchFormat;
	patch::TargetFormat m_targetFormat;
};

} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_PATCH_H