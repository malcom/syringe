/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_CMD_PATCH_PROVIDER_H
#define SYRINGE_CMD_PATCH_PROVIDER_H

#include <memory>
#include "cmd/patch/format.h"
#include "utils/file.h"

namespace syringe {
namespace cmd {
namespace patch {

struct Provider {

	struct Entry {
		unsigned int offset;
		unsigned char orginal;
		unsigned char replaced;
	};

	virtual void Reset() = 0;
	virtual bool Next() = 0;

	virtual const Entry* GetPatchEntry() = 0;

};

typedef std::unique_ptr<Provider> ProviderPtr;

ProviderPtr MakeProvider(PatchFormat format, utils::File& file);

} // namespace patch
} // namespace cmd
} // namespace syringe

#endif // SYRINGE_CMD_PATCH_PROVIDER_H