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
#include "utils/enc.h"

#include <algorithm>

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


//
// IntelHEX - Hexadecimal Object File Format
// spec: http://microsym.com/editor/assets/intelhex.pdf
//
// TODO:
// - add suport for Extended Segment/Linear Address records
// - for record with data len > 16 (max 255)
//
struct IntelHexApplicator : public Applicator {

	utils::File& m_file;

	const size_t PrologLength = 9;

	struct Record {
		fpos_t filepos;		// record position in file
		uint32_t offset;	// record load offset
		uint8_t length;		// record data length
		uint8_t updated;	// is record updated?
	};

	typedef std::vector<Record> Records;
	Records m_rec;

	uint8_t ReadByte() {
		char buf[2];
		buf[0] = m_file.Getc();
		buf[1] = m_file.Getc();
		return utils::HexToDec<uint8_t>(buf);
	}

	void WriteByte(uint8_t byte) {
		char buf[2];
		utils::DecToHex<uint8_t>(byte, buf);
		m_file.Putc(buf[0]);
		m_file.Putc(buf[1]);
	}


	void BuildVector() {

		m_rec.clear();

		char buffer[46];
		while (!m_file.Eof()) {

			fpos_t filepos = m_file.GetPos();
			m_file.Gets(buffer, sizeof(buffer));
			fpos_t len = m_file.GetPos() - filepos;

			if (len < PrologLength)
				throw std::runtime_error("Invalid data in file");

			// each record start with Record Mark
			if (buffer[0] != ':')
				throw std::runtime_error("Invalid data in file");

			uint8_t length = utils::HexToDec<uint8_t>(&buffer[1]);
			uint16_t offset = utils::HexToDec<uint16_t>(&buffer[3]);
			uint8_t type = utils::HexToDec<uint8_t>(&buffer[7]);

			// End of File Record
			if (type == 1)
				break;

			// only support Data Record
			if (type != 0)
				continue;

			// address in big endian
			offset = offset >> 8 | offset << 8;

			m_rec.emplace_back(Record{ filepos, offset, length, 0 });
		}

		// sort descending
		std::sort(m_rec.begin(), m_rec.end(), [](const Record& lhs, const Record& rhs) { return lhs.offset > rhs.offset; });
	}

	Record& GetRecord(uint32_t address) {

		// cache prev record
		static auto prev = m_rec.begin();

		size_t offset = address - prev->offset;
		if (offset < prev->length)
			return *prev;

		auto it = std::find_if(m_rec.begin(), m_rec.end(), [=](const Record& r) { return r.offset <= address; });
		if (it == m_rec.end())
			throw std::runtime_error("Record for specified address is not available");

		offset = address - it->offset;
		if (offset >= it->length)
			throw std::runtime_error("Record for specified address is not available");

		prev = it;
		return *it;
	}

	size_t UpdateChecksum() {

		size_t updated = 0;
		for (auto& rec : m_rec) {
			if (rec.updated == 1) {

				// set position after Record Mark (':')
				m_file.SetPos(rec.filepos + 1);

				size_t len = (PrologLength - 1) / 2 + rec.length;

				char crc = 0;
				for (size_t i = 0; i < len; i++)
					crc += ReadByte();
				crc = static_cast<char>(0x100 - crc);

				// re-pos position before write operation
				m_file.Seek(0, utils::File::SeekCur);
				WriteByte(crc);

				rec.updated = 0;
				updated++;
			}
		}

		return updated;
	}


	IntelHexApplicator(utils::File& file)
		: m_file(file) {

		BuildVector();
	}

	size_t Apply(Provider& patch) {

		size_t bytes = 0;
		patch.Reset();

		while (patch.Next()) {

			const Provider::Entry* entry = patch.GetPatchEntry();

			Record& rec = GetRecord(entry->offset);

			m_file.SetPos(rec.filepos + PrologLength + ((entry->offset - rec.offset) * 2));
			WriteByte(entry->replaced);

			rec.updated = 1;
			bytes++;
		}

		bytes += UpdateChecksum();

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
	ApplicatorFactory<IntelHexApplicator>,
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
