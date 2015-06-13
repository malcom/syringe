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

std::string Command::FormatHelpMessage(const std::string& usage, const OptionsDesc* opts) const {

	std::stringstream ss;

	ss << "Command " << Name() << " - " << Desc() << "\n";
	ss << usage << "\n\n";

	if (opts != nullptr) {
		ss << "Command " << Name() << " options:" << "\n";
		ss << *opts;
	}

	return ss.str();
}

Commands::CmdMap Commands::Map(Commands::Compare);

Command* Commands::Get(const std::string& name) {

	auto iter = std::find_if(
		std::begin(Map), std::end(Map),
		[&](CmdMap::value_type cmd) { return cmd->Name() == name; }
	);

	return iter != std::end(Map) ? *iter : nullptr;
}

} // namespace cmd
} // namespace syringe
