#include "stdafx.h"

#include "Write.h"

int       __cdecl atexit(void(__cdecl*bumsti)(void)) 
{
	return 0;
}

namespace bee
{
	Writer* Writer::_out = nullptr;
	Writer* Writer::_err = nullptr;

	LastError* Writer::Write(const wstring& str, LastError* err) 
	{
		if (DWORD written; GetFileType(_fp) == FILE_TYPE_CHAR)
		{
			if (!WriteConsoleW(_fp, str.data(), (DWORD)str.length(), &written, NULL))
			{
				err->set("WriteConsoleW");
			}
		}
		else
		{
			const size_t reserved_capacity = str.length() * 4;
			_array.reserve(reserved_capacity);

			if (int multiBytesWritten; (multiBytesWritten = WideCharToMultiByte(
				GetConsoleOutputCP()
				, 0						// dwFlags           [in]
				, str.data()			// lpWideCharStr     [in]
				, (int)str.length()		// cchWideChar       [in]
				, _array.data()			// lpMultiByteStr    [out, optional]
				, reserved_capacity		// cbMultiByte       [in]
				, NULL					// lpDefaultChar     [in, optional]
				, NULL					// lpUsedDefaultChar [out, optional]
			)) == 0)
			{
				err->set("WideCharToMultiByte");
			}
			else if (DWORD written; !WriteFile(
				_fp
				, _array.data()
				, multiBytesWritten
				, &written
				, NULL))
			{
				err->set("WriteFile");
			}
		}
		return err;
	}
	//-----------------------------------------------------------------------------
	void Writer::Write(const wstring& str) {
	//-----------------------------------------------------------------------------
		LastError err;
		this->Write(str, &err);
	}
	//-----------------------------------------------------------------------------
	void Writer::Write(const wchar_t* format, ...) {
	//-----------------------------------------------------------------------------
		va_list args;
		va_start(args, format);

		bee:wstring tmp;
		tmp.vappendf(format, args);
		va_end(args);

		Write(tmp);
	}
}

