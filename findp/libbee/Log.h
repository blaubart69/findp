#pragma once

class Log
{
public:
	static Log* Instance() 
	{
		if (Log::_instance == nullptr)
		{
			_instance = new Log(2, GetStdHandle(STD_ERROR_HANDLE), GetConsoleOutputCP());
		}
		return _instance;
	};

	void setLevel(int level) { _level = level; }

	void dbg(const WCHAR* format, ...);
	void inf(const WCHAR* format, ...);
	void wrn(const WCHAR* format, ...);
	void err(const WCHAR* format, ...);

	void win32err(LPCWSTR Apiname);
	void win32err(LPCWSTR Apiname, LPCWSTR param);

	void write    (const WCHAR * format, ...);
	void writeLine(const WCHAR * format, ...);

	static void win32errfunc(LPCWSTR Apiname, LPCWSTR param);

private:
	static Log * _instance;
	
		  int		_level;
	const HANDLE	_outHandle;
	const UINT		_codepage;

	Log(int level, HANDLE outHandle, UINT codepage);
	void writeLogLine(const WCHAR* format, va_list args, bool appendNewLine);
	void writeLogLine(WCHAR prefix, const WCHAR* format, va_list args, bool appendNewLine);
	BOOL writeTextCodepage(const WCHAR* text, const DWORD cchWideChar);

public:
	Log(Log const&) = delete;
	void operator=(Log const&) = delete;
};
