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
		const UINT   _codepage;

	public:
		Writer(HANDLE fp)               : _fp(fp), _codepage(GetConsoleOutputCP()) {}
		Writer(HANDLE fp, int codepage) : _fp(fp), _codepage(codepage)             {}
		~Writer() { CloseHandle(_fp); }
		LastError* Write(const wstring& str, LastError* err);
		void       Write(const wstring& str);
		void       WriteLine(const wstring& str);
		void	   WriteA(const char* str);
	};

	extern Writer* Out;
	extern Writer* Err;

}


