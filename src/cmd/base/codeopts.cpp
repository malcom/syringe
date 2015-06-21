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
#include "cmd/base/codeopts.h"
#include "utils/enc.h"

#include <boost/program_options.hpp>

namespace syringe {

void validate(boost::any& v, const std::vector<std::string>& values, CodeBuffer*, int) {

	namespace po = boost::program_options;

	po::validators::check_first_occurrence(v);

	const std::string& s = po::validators::get_single_string(values);

	syringe::CodeBuffer code;
	code.resize(s.size() / 4);

	int ret = syringe::utils::DecodeShellcode(s.data(), s.size(), code.data());
	if (ret != 0)
		throw po::validation_error(po::validation_error::invalid_option_value);

	v = boost::any(code);
}

} // namespace syringe
