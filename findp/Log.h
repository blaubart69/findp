#pragma once

class Log
{
public:
	static Log* Instance() 
	{
		if (Log::_instance == nullptr)
		{
			_instance = new Log;
		}
		return _instance;
	};
	void dbg(const WCHAR* format, ...) const;
	void inf(const WCHAR* format, ...) const;
	void wrn(const WCHAR* format, ...) const;
	void err(const WCHAR* format, ...) const;

	void write(const WCHAR* format, ...) const;
	void writeLine(const WCHAR * format, ...) const;

	void win32err(LPCWSTR Apiname) const;
	void win32err(LPCWSTR Apiname, LPCWSTR param) const;

private:
	static Log * _instance;

	Log() {};

public:
	Log(Log const&) = delete;
	void operator=(Log const&) = delete;
};
