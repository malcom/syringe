/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_PATCH_APPLICATOR_H
#define SYRINGE_CMD_PATCH_APPLICATOR_H

#include <memory>
#include "cmd/patch/format.h"
#include "utils/file.h"

namespace syringe {
namespace cmd {
namespace patch {

struct Provider;

struct Applicator {

	virtual size_t Apply(Provider& patch) = 0;

};

typedef std::unique_ptr<Applicator> ApplicatorPtr;

ApplicatorPtr MakeApplicator(TargetFormat format, utils::File& file);

} // namespace patch
} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_PATCH_APPLICATOR_H