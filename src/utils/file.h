/*
 * Syringe - A general purpose code injector tool
 * http://github.com/malcom/syringe
 *
 * Copyright (C) 2015 Marcin 'Malcom' Malich <me@malcom.pl>
 * Released under the MIT License.
 *
 * See README.md and LICENSE.md for more info.
 */

#ifndef SYRINGE_UTILS_FILE_H
#define SYRINGE_UTILS_FILE_H

#include "defs.h"
#include <cstdio>
#include <cstring>
#include <cstdarg>

namespace syringe {
namespace utils {

//
// Simple OOP interface for the most common C-lib file-IO functions
//
class File {
public:

	class Exception : public std::exception {
	public:
		Exception(const char* msg, int code)
			: m_msg(msg), m_code(code)
		{
			m_msg += ": ";
			m_msg += std::strerror(code);
		}

		const char* what() const {
			return m_msg.data();
		}

		int code() const {
			return m_code;
		}

	private:
		std::string m_msg;
		int m_code;
	};

	// TODO: change to inline function with _func_ C/C++11
	#define throw_file_error() throw File::Exception(__FUNCTION__, errno)
	#define throw_file_error_if_error() { int e = errno; if (Error()) { throw File::Exception(__FUNCTION__, e);} }


	File(FILE* file = nullptr) {
		m_file = file;
	}

	File(const char* filename, const char* mode) {
		Open(filename, mode);
	}

	~File() {
		if (IsOpened())
			Close();
	}


	void Open(const char* filename, const char* mode) {
		if (IsOpened())
			Close();
		m_file = std::fopen(filename, mode);
		if (m_file == nullptr)
			throw_file_error();
	}

	void Close() {
		if (std::fclose(m_file) != 0)
			throw_file_error();
		m_file = nullptr;
	}

	bool IsOpened() const {
		return m_file != nullptr;
	}


	unsigned char Getc() const {
		int ret = std::fgetc(m_file);
		if (ret == -1)
			throw_file_error();
		return static_cast<char>(ret);
	}

	void Gets(char* str, int num) const {
		if (std::fgets(str, num, m_file) == nullptr)
			throw_file_error();
	}

	void Putc(unsigned char character) {
		if (std::fputc(character, m_file) == -1)
			throw_file_error();
	}

	void Puts(const char* str) {
		if (std::fputs(str, m_file) == -1)
			throw_file_error();
	}


	size_t Read(void* ptr, size_t size, size_t count) const {
		size_t ret = std::fread(ptr, size, count, m_file);
		// ignore eof, only error
		throw_file_error_if_error();
		return ret;
	}

	size_t Write(const void* ptr, size_t size, size_t count) {
		size_t ret = std::fwrite(ptr, size, count, m_file);
		// ignore eof, only error
		throw_file_error_if_error();
		return ret;
	}


	int Scanf(const char* format, ...) const {
		va_list args;
		va_start(args, format);
		int ret = std::vfscanf(m_file, format, args);
		// ignore eof, only error
		throw_file_error_if_error();
		return ret;
	}

	int Printf(const char* format, ...) {
		va_list args;
		va_start(args, format);
		int ret = std::vfprintf(m_file, format, args);
		// ignore eof, only error
		throw_file_error_if_error();
		return ret;
	}


	long Tell() const {
		long pos = std::ftell(m_file);
		if (pos == -1)
			throw_file_error();
		return pos;
	}

	enum SeekOrigin {
		SeekSet = SEEK_SET,
		SeekCur = SEEK_CUR,
		SeekEnd = SEEK_END,
	};

	void Seek(long offset, SeekOrigin origin) {
		if (std::fseek(m_file, offset, origin) != 0)
			throw_file_error();
	}


	fpos_t GetPos() const {
		fpos_t pos;
		if (std::fgetpos(m_file, &pos) != 0)
			throw_file_error();
		return pos;
	}

	void SetPos(fpos_t pos) {
		if (std::fsetpos(m_file, &pos) != 0)
			throw_file_error();
	}


	bool Eof() const {
		return std::feof(m_file) != 0;
	}

	bool Error() const {
		return std::ferror(m_file) != 0;
	}


	void Flush() {
		if (std::fflush(m_file) != 0)
			throw_file_error();
	}


	void Setbuf(char* buffer) {
		std::setbuf(m_file, buffer);
	}

	void Setvbuf(char* buffer, int mode, size_t size) {
		if (std::setvbuf(m_file, buffer, mode, size) != 0)
			throw_file_error();
	}


	FILE* Handle() const {
		return m_file;
	}

	void Attach(FILE* file) {
		m_file = file;
	}

	void Detach() {
		m_file = nullptr;
	}

private:
	FILE* m_file;
};

} // namespace utils
} // namespace syringe

#endif // SYRINGE_UTILS_FILE_H