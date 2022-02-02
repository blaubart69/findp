#pragma once

#include "beevector.h"
#include "beewstring.h"
#include "LastError.h"

namespace bee 
{
	class Writer
	{
	private:
		vector<char> _array;
		const HANDLE _fp;

		static Writer* _out;
		static Writer* _err;

	public:
		Writer(HANDLE fp) : _fp(fp) {}
		~Writer() { CloseHandle(_fp); }
		LastError* Write(const wstring& str, LastError* err);
		void       Write(const wstring& str);
		void	   Write(const wchar_t* format, ...);
		void       WriteLine(const wstring& str);

		static Writer& Out()
		{
			if (_out == nullptr)
			{
				_out = new Writer (GetStdHandle(STD_OUTPUT_HANDLE));
			}
			return *_out;
		}
		static Writer& Err()
		{
			if (_err == nullptr)
			{
				_err = new Writer(GetStdHandle(STD_ERROR_HANDLE));
			}
			return *_err;
		}
	};

	

}

