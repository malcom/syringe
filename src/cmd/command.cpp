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
#include "cmd/command.h"

 namespace syringe {
 namespace cmd {

void Command::DoFormatHelp(std::ostream& stream, const std::string& usage, const OptionsDesc* opts) const {

	stream << usage << "\n\n";

	if (opts != nullptr) {
		stream << "Command " << Name() << " options:" << "\n";
		stream << *opts;
	}

}

Command* Commands::Get(const std::string& name) {

	auto iter = std::find_if(
		std::begin(Map()), std::end(Map()),
		[&](CmdMap::value_type cmd) { return cmd->Name() == name; }
	);

	return iter != std::end(Map()) ? *iter : nullptr;
}

} // namespace cmd
} // namespace syringe
