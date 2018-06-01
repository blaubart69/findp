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
	void inf(const WCHAR* format, ...) const;

private:
	static Log * _instance;

	Log() {};

public:
	Log(Log const&) = delete;
	void operator=(Log const&) = delete;
};
