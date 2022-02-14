#include "Write.h"
#include "LastError.h"

int       __cdecl atexit(void(__cdecl*bumsti)(void)) 
{
	return 0;
}

namespace bee
{
	//Writer* Writer::_out = nullptr;
	//Writer* Writer::_err = nullptr;
	Writer* Out;
	Writer* Err;

	LastError& Writer::Write(const wstring& str, vector<char>& tmp, LastError* err) const
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
			tmp.reserve(reserved_capacity);

			if (int multiBytesWritten; (multiBytesWritten = WideCharToMultiByte(
				_codepage
				, 0						// dwFlags           [in]
				, str.data()			// lpWideCharStr     [in]
				, (int)str.length()		// cchWideChar       [in]
				, tmp.data()			// lpMultiByteStr    [out, optional]
				, reserved_capacity		// cbMultiByte       [in]
				, NULL					// lpDefaultChar     [in, optional]
				, NULL					// lpUsedDefaultChar [out, optional]
			)) == 0)
			{
				err->set("WideCharToMultiByte");
			}
			else if (DWORD written; !WriteFile(
				_fp
				, tmp.data()
				, multiBytesWritten
				, &written
				, NULL))
			{
				err->set("WriteFile");
			}
		}
		return *err;
	}

	LastError& Writer::Write(const wstring& str, LastError* err) 
	{
		return Write(str, _array, err);
	}
	//-----------------------------------------------------------------------------
	void Writer::Write(const wstring& str) {
	//-----------------------------------------------------------------------------
		LastError err;
		this->Write(str, &err);
		if (err.failed())
		{
			err.print();
		}
	}
	//-----------------------------------------------------------------------------
	void Writer::WriteLine(wstring& str) {
	//-----------------------------------------------------------------------------
		const size_t len_before = str.length();
		str.append(L"\r\n");
		this->Write(str);
		str.resize(len_before);
	}
	/*
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
	*/
	//-----------------------------------------------------------------------------
	void Writer::WriteA(const char* str) {
	//-----------------------------------------------------------------------------
		wstring tmp;
		tmp.appendA(str);
		Write(tmp);
	}

}

