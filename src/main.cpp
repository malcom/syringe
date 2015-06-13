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

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>

#include <boost/program_options.hpp>

#include "version.h"
#include "cmd/command.h"

namespace {

using syringe::cmd::Command;
using syringe::cmd::Commands;

void ShowAvailableCommands(std::ostream& out) {

	out << "Available commands:\n";
	Commands::Iterate([&](const Command* cmd) {
		out << "  " << std::setw(8) << std::left << cmd->Name() << cmd->Desc() << "\n";
	});

}

class invalid_command : public po::error_with_option_name {
public:
	invalid_command(const std::string& name = "")
		: po::error_with_option_name("", "", name) {}
	~invalid_command() throw() {}
};

} // anonymous namespace


int main(int argc, const char* argv[]) {

	try {
		
		using namespace syringe;
		using syringe::cmd::Command;
		using syringe::cmd::Commands;

		namespace po = boost::program_options;

		po::options_description global("Global options");
		global.add_options()
			("help,h", "Display help message\n" "If 'command' is given, print the command specific help")
			("version,v", "Display the version number")
			("list", "Display available commands")
			("command", po::value<std::string>(), "Command to execute")
			("subargs", po::value<std::vector<std::string>>(), "Arguments for command");

		po::positional_options_description pos;
		pos.add("command", 1).
			add("subargs", -1);

		po::variables_map vm;

		po::parsed_options parsed = po::command_line_parser(argc, argv).
			options(global).
			positional(pos).
			allow_unregistered().
			run();

		po::store(parsed, vm);

		std::string cmd;
		if (vm.count("command"))
			cmd = vm["command"].as<std::string>();

		Command* command = Commands::Get(cmd);

		#define APPPROLOG	"Syringe - A general purpose code injector tool"

		if (vm.count("help")) {
			if (cmd.empty()) {
				// global help message

				std::cout << APPPROLOG << "\n";
				std::cout << "Usage: syringe [options] <command> [subargs...]" << "\n\n";
				std::cout << global << "\n";
				ShowAvailableCommands(std::cout);
				std::cout << "\n";

				std::cout << "Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>" << "\n";
				std::cout << "Released under the MIT License" << "\n";
				std::cout << "http://github.com/malcom/syringe" << "\n";

				return 0;

			} else {
				// command help message

				if (command == nullptr)
					throw invalid_command(cmd);
				std::cout << command->Help();

				return 0;
			}
		}

		if (vm.count("version")) {
			std::cout << APPPROLOG << "\n";
			std::cout << "Version: " << VersionString << "\n";
			return 0;
		}

		if (vm.count("list")) {
			ShowAvailableCommands(std::cout);
			return 0;
		}

		if (cmd.empty()) {
			std::cout << APPPROLOG << "\n";
			std::cout << "Type 'syringe --help' for more information" << "\n\n";
			ShowAvailableCommands(std::cout);
			return 0;
		}

		#undef APPPROLOG

		if (command == nullptr)
			throw invalid_command(cmd);

		Command::OptionsList opts = po::collect_unrecognized(parsed.options, po::include_positional);
		opts.erase(opts.begin());	// erase command name

		command->Parse(opts);
		return command->Run();

	} catch (invalid_command& e) {
		std::cerr << "Invalid command: " << e.get_option_name() << "\n\n";
		ShowAvailableCommands(std::cerr);
		return 1;

	} catch (std::runtime_error& e) {
		// TODO: this shoudl be changed
		std::cerr << e.what() << "\n" << "Error code: " << ::GetLastError() << std::endl;
		return 1;

	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		return 1;

	} catch (...) {
		std::cerr << "Undefined error" << std::endl;
		return 1;
	}

}
