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
#include "cmd/patch/provider.h"

namespace syringe {
namespace cmd {
namespace patch {

struct IDA_Provider : public Provider {

	utils::File& m_file;
	Provider::Entry m_entry;

	IDA_Provider(utils::File& file)
		: m_file(file) {
	}


	void Reset() {
		m_file.Seek(0, utils::File::SeekSet);

		char line[256];
		m_file.Gets(line, sizeof(line));	// eat intro line
		m_file.Gets(line, sizeof(line));	// eat blank line
		m_file.Gets(line, sizeof(line));	// eat input file name
	}

	bool Next() {
		unsigned int tmp[3];
		int ret = m_file.Scanf("%x: %x %x\n", &tmp[0], &tmp[1], &tmp[2]);

		if (ret != 3) {
			if (m_file.Eof())
				return false;
			throw std::runtime_error("Invalid data in file");
		}

		m_entry.offset = tmp[0];
		m_entry.orginal = static_cast<char>(tmp[1]);
		m_entry.replaced = static_cast<char>(tmp[2]);
		return true;
	}

	const Entry* GetPatchEntry() {
		return &m_entry;
	}

};


template <typename T>
inline ProviderPtr ProviderFactory(utils::File& file) {
	return ProviderPtr(new T(file));
}

typedef decltype(ProviderFactory<Provider>)* ProviderFactoryFunctor;

static const ProviderFactoryFunctor Functors[] = {
	nullptr,	// unknown
	ProviderFactory<IDA_Provider>,
};

static_assert(
	countof(Functors) == static_cast<int>(PatchFormat::FormatMax),
	"The provider functors array must contains functor for all format values"
);

ProviderPtr MakeProvider(PatchFormat format, utils::File& file) {

	ProviderFactoryFunctor factory = Functors[static_cast<int>(format)];
	if (factory == nullptr)
		std::runtime_error("Provider for specific format not available");

	return factory(file);
}

} // namespace patch
} // namespace cmd
} // namespace syringe
