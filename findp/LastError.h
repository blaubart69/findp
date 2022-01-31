#pragma once

#include "beewstring.h"

namespace bee
{
	class LastError
	{
	private:

		LPCSTR   _func;
		wstring* _param;
		DWORD    _rc;

	public:

		LastError() : 
			  _rc(0)
			, _func(nullptr) 
			, _param(nullptr)
		{}
		~LastError()
		{
			if (_param != nullptr)
			{
				delete _param;
			}
		}
		LastError(LPCSTR errFunc)
		{
			this->set(errFunc);
		}

		bool failed() const { return _rc != 0; }
		bool ok()     const { return _rc == 0; }

		LastError* set(LPCSTR errFunc)
		{
			_rc = ::GetLastError();
			_func = errFunc;
			return this;
		}
		LastError* set(LPCSTR errFunc, DWORD lastError)
		{
			_rc = lastError;
			_func = errFunc;
			return this;
		}
		LastError* set(LPCSTR errFunc, const bee::wstring& funcParam)
		{
			this->set(errFunc);
			if (_param == nullptr)
			{
				_param = new wstring;
			}
			_param->assign(funcParam);
			return this;
		}

		DWORD code(void) { return _rc; }
		void print(void);
	};
}