/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_COMMAND_H
#define SYRINGE_CMD_COMMAND_H

#include <string>
#include <algorithm>
#include <vector>
#include <set>

#include <boost/program_options.hpp>
namespace po = boost::program_options;

namespace syringe {
namespace cmd {

class Command {
public:

	typedef std::vector<std::string>	OptionsList;
	typedef po::options_description		OptionsDesc;
	typedef po::variables_map			OptionsMap;

	virtual std::string Name() const = 0;
	virtual std::string Desc() const = 0;

	virtual void Help(std::ostream& stream) const {
		stream << "Command " << Name() << " - " << Desc() << "\n";
	}

	virtual void Logo(std::ostream& stream) const {
		stream << "Syringe " << " - " << Desc() << "\n";
	}

	virtual void Parse(const OptionsList& /*opts*/) {}

	virtual int Run() = 0;

protected:
	void DoFormatHelp(std::ostream& stream, const std::string& usage, const OptionsDesc* opts) const;

};


class Commands {
private:

	static bool Compare(const Command* lhs, const Command* rhs) {
		return lhs->Name() < rhs->Name();
	}

	typedef std::set<Command*, bool(*)(const Command*, const Command*)> CmdMap;

	static CmdMap& Map() {
		// to be sure that the CmdMap is aviable before any other static
		// auto-register command object was initialized, so it can be
		// static in global/class scope, where static initialization
		// order depeneds of compiler
		static CmdMap map(Commands::Compare);
		return map;
	}

public:

	static void Register(Command* cmd) {
		Map().insert(cmd);
	}

	static void Unregister(Command* cmd) {
		Map().erase(cmd);
	}

	static Command* Get(const std::string& name);

	template<typename Func>
	static void Iterate(Func func) {
		std::for_each(std::begin(Map()), std::end(Map()), func);
	}

};


template<typename T>
struct AutoRegisterCommand {
	T Cmd;

	AutoRegisterCommand() {
		Commands::Register(&Cmd);
	}

	~AutoRegisterCommand() {
		Commands::Unregister(&Cmd);
	}
};

#define SYRINGE_AUTO_REGISTER_COMMAND(cmd) AutoRegisterCommand<cmd> auto_##cmd;

} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_COMMAND_H