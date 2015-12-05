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
#include "cmd/patch.h"
#include "patch/format.h"
#include "patch/applicator.h"
#include "patch/provider.h"

#include <iostream>
#include <iomanip>

namespace syringe {
namespace cmd {

using namespace syringe::cmd::patch;

// boost_options validate function for format types must be pleaced in the real type namespace
namespace patch {

template<typename T>
void validate(boost::any& v, const std::vector<std::string>& values, T*, int) {

	namespace po = boost::program_options;

	po::validators::check_first_occurrence(v);

	const std::string& value = po::validators::get_single_string(values);
	T format = FormatValue<T>(value.c_str());
	if (format == T::Unknown)
		throw po::validation_error(po::validation_error::invalid_option_value);

	v = boost::any(format);
}

} // patch namespace

Patch::Patch() {

	m_optDesc.add_options()
		("format,f", po::value<PatchFormat>(&m_patchFormat)->required(), "Patch file format")
		("target", po::value<TargetFormat>(&m_targetFormat)->default_value(TargetFormat::Binary,
			std::string(FormatString(TargetFormat::Binary))), "Target file type (default: binary)")
		("patch-file,p", po::value<std::string>(&m_patchFile)->required(), "Patch/diff filename or path")
		("target-file,t", po::value<std::string>(&m_targetFile)->required(), "Target filename or path")
		;

}

void Patch::Help(std::ostream& stream) const {
	DoFormatHelp(stream, "Usage: syringe patch [options] <target-file> <patch-file>", &m_optDesc);

	stream << "\n";
	stream << "Supported patch file formats:\n";
	stream << "  " << FormatStringList<PatchFormat>() << "\n";

	stream << "\n";
	stream << "Supported target file types:\n";
	stream << "  " << FormatStringList<TargetFormat>() << "\n";
}

void Patch::Logo(std::ostream& stream) const {

	Command::Logo(stream);

	const char* patch = FormatString(m_patchFormat);
	const char* target = FormatString(m_targetFormat);
	size_t len = max(std::strlen(patch), std::strlen(target)) + 1;

	stream
		<< "Target: " << std::setw(len) << std::left << patch  << m_targetFile << "\n"
		<< "Patch:  " << std::setw(len) << std::left << target << m_patchFile  << "\n"
		<< std::endl;

}

void Patch::Parse(const OptionsList& opts) {

	po::positional_options_description pos;
	pos.add("target-file", 1);
	pos.add("patch-file", 2);

	po::parsed_options parsed = po::command_line_parser(opts).
		options(m_optDesc).
		positional(pos).
		run();

	OptionsMap map;
	po::store(parsed, map);
	po::notify(map);

}

int Patch::Run() {

	utils::File target(m_targetFile.c_str(), "rb+");
	utils::File patch(m_patchFile.c_str(), "r");

	std::cout << "Creating aplicator and provider for specified formats" << std::endl;

	ApplicatorPtr applicator = MakeApplicator(m_targetFormat, target);
	ProviderPtr provider = MakeProvider(m_patchFormat, patch);

	std::cout << "Patching target file..." << std::endl;

	size_t bytes = applicator->Apply(*provider);

	std::cout << "Target successfully patched, changed bytes: " << bytes << std::endl;

	return 0;
}

SYRINGE_AUTO_REGISTER_COMMAND(Patch);

} // namespace cmd
} // namespace syringe
