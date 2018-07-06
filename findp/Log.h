#pragma once

class Log
{
public:
	static Log* Instance() 
	{
		if (Log::_instance == nullptr)
		{
			_instance = new Log(2);
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

	void resetBuffer();
	void append(LPCWSTR text, DWORD cchWideChar);
	void appendf(const WCHAR * format, ...);
	void writeBuffer();

	static void win32errfunc(LPCWSTR Apiname, LPCWSTR param);

private:
	static Log * _instance;
	
	int _level;
	LineWriter	*_lineWriter;

	Log(int level);
	void writeLogLine(WCHAR prefix, const WCHAR* format, va_list args);

public:
	Log(Log const&) = delete;
	void operator=(Log const&) = delete;
};
