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
#include "cmd/patch/applicator.h"
#include "cmd/patch/provider.h"

namespace syringe {
namespace cmd {
namespace patch {

struct BinaryApplicator : public Applicator {

	utils::File& m_file;

	BinaryApplicator(utils::File& file)
		: m_file(file) {
	}

	size_t Apply(Provider& patch) {

		size_t bytes = 0;
		patch.Reset();

		while (patch.Next()) {
			const Provider::Entry* entry = patch.GetPatchEntry();

			m_file.Seek(entry->offset, utils::File::SeekSet);
			m_file.Putc(entry->replaced);
			bytes++;
		}

		return bytes;
	}

};


template <typename T>
inline ApplicatorPtr ApplicatorFactory(utils::File& file) {
	return ApplicatorPtr(new T(file));
}

typedef decltype(ApplicatorFactory<Applicator>)* ApplicatorFactoryFunctor;

static const ApplicatorFactoryFunctor Functors[] = {
	nullptr,	// unknown
	ApplicatorFactory<BinaryApplicator>,
};

static_assert(
	countof(Functors) == static_cast<int>(TargetFormat::FormatMax),
	"The applicator functors array must contains functor for all format values"
);

ApplicatorPtr MakeApplicator(TargetFormat format, utils::File& file) {

	ApplicatorFactoryFunctor factory = Functors[static_cast<int>(format)];
	if (factory == nullptr)
		std::runtime_error("Applicator for specific format not available");

	return factory(file);
}

} // namespace patch
} // namespace cmd
} // namespace syringe
