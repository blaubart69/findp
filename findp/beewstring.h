#pragma once

#include "nt.h"
#include "beeVector.h"

namespace bee
{
	class wstring
	{
	private:
		vector<wchar_t> _vec;

	public:
		wstring() {}
		wstring(const wchar_t* str)
		{
			appendW(str);

		}
		wstring(const wchar_t* str, const size_t len)
		{
			assign(str,len);

		}
		wchar_t* c_str()
		{
			// El Dörty
			_vec.ensureCapacity(_vec.size() + 1);
			_vec[_vec.size()] = L'\0';
			return _vec.data();
		}
		wchar_t* data() const
		{
			return _vec.data();
		}
		wstring& resize(const size_t newLen)
		{
			_vec.resize(newLen);
			return *this;
		}
		size_t length() const
		{
			return _vec.size();
		}
		wstring& push_back(const wchar_t c)
		{
			_vec.push_back(c);
			return *this;
		}
		wstring& assign(const wstring& str)
		{
			_vec.assign(str._vec);
			return *this;
		}
		wstring& assign(const wchar_t* str, const size_t len)
		{
			if (str != nullptr)
			{
				_vec.assign(str, len);
			}
			return *this;
		}
		wstring& append(const wstring& str)
		{
			_vec.append(str._vec);
			return *this;
		}
		wstring& append(const wchar_t* str, const size_t len)
		{
			_vec.append(str, len);
			return *this;
		}
		wstring& appendW(const wchar_t* str)
		{
			if (str != nullptr)
			{
				for (int i = 0; str[i] != L'\0'; ++i)
				{
					_vec.push_back(str[i]);
				}
			}
			return *this;
		}
		wstring& appendA(const char* str)
		{
			for (int i = 0; str[i] != '\0'; ++i)
			{
				wchar_t c = str[i];
				_vec.push_back(c);
			}
			return *this;
		}
		wstring& append_ull(ULONGLONG val) {

			_vec.reserve(_vec.size() + 32);

			nt::UNICODE_STRING ucs;
			ucs.Length			= 0;
			ucs.MaximumLength	= 32;
			ucs.Buffer			= _vec.data() + _vec.size();

			nt::NTSTATUS status = nt::RtlInt64ToUnicodeString(val, 10, &ucs);

			_vec.resize(_vec.size() + (ucs.Length / sizeof(WCHAR)));

			return *this;
		}
		wstring& append_ll(LONGLONG val) {

			if (val < 0)
			{
				_vec.push_back(L'-');
				val = -val;
			}

			return append_ull((ULONGLONG)val);
		}
		WCHAR& operator[](size_t idx) const
		{
			return _vec[idx];
		}
		bool ends_with(const wchar_t c) const
		{
			if (length() == 0)
			{
				return false;
			}

			return _vec[ length() -1 ] == c;
		}
		wstring& sprintf(const wchar_t* format, ...)
		{
			va_list args;
			va_start(args, format);

			if (_vec.size() == 0)
			{
				_vec.resize(32);
			}

			for (;;)
			{
				int charsWritten = nt::vswprintf_s(_vec.data(), _vec.size(), format, args);
				if (charsWritten == -1)
				{
					_vec.resize(_vec.size() * 4);
				}
				else
				{
					_vec.resize(charsWritten);
					break;
				}
			}
			va_end(args);

			return *this;
		}
		wstring& appendf(const wchar_t* format, ...)
		{
			va_list args;
			va_start(args, format);
			this->vappendf(format, args);
			va_end(args);

			return *this;
		}
		wstring& vappendf(const wchar_t* format, va_list argptr)
		{
			const size_t oldSize = _vec.size();
			if (_vec.size() == 0)
			{
				_vec.resize(32);
			}
			else
			{
				_vec.resize(oldSize + 64);
			}

			for (;;)
			{
				int charsWritten = nt::vswprintf_s(_vec.data() + oldSize, _vec.size() - oldSize, format, argptr);
				if (charsWritten == -1)
				{
					_vec.resize(_vec.size() * 2);
				}
				else
				{
					_vec.resize(oldSize + charsWritten);
					break;
				}
			}
			return *this;
		}
	};
}