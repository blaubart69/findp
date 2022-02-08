#pragma once

#include "../nt.h"
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
			append(str);

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
		wstring& assign(const wchar_t* str)
		{
			_vec.resize(0);
			append(str);
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
		wstring& append(const wchar_t* str)
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
		//													          09876543210987654321	
		// A ULONGLONG is a 64-bit unsigned integer (range: 0 through 18446744073709551615)
		//
		int get_digits_count(const ULONGLONG val)
		{
			if (val < 10) return 1;
			if (val < 100) return 2;
			if (val < 1000) return 3;
			if (val < 10000) return 4;
			if (val < 100000) return 5;
			if (val < 1000000) return 6;
			if (val < 10000000) return 7;
			if (val < 100000000) return 8;
			if (val < 1000000000) return 9;
			if (val < 10000000000) return 10;
			if (val < 100000000000) return 11;
			if (val < 1000000000000) return 12;
			if (val < 10000000000000) return 13;
			if (val < 100000000000000) return 14;
			if (val < 1000000000000000) return 15;
			if (val < 10000000000000000) return 16;
			if (val < 100000000000000000) return 17;
			if (val < 1000000000000000000) return 18;
			if (val < 10000000000000000000) return 19;
			return 20;
		}
		//													          09876543210987654321	
		// A ULONGLONG is a 64-bit unsigned integer (range: 0 through 18446744073709551615)
		wstring& append_ull(ULONGLONG val, int align = 0, wchar_t fill = L' ')
		{
			if (align > 0)
			{
				int blanks_to_insert = align - get_digits_count(val);
				if (blanks_to_insert > 0)
				{
					_vec.append(fill, blanks_to_insert);
				}
			}

			_vec.reserve( 20 );

			nt::UNICODE_STRING ucs;
			ucs.Length			= 0;
			ucs.MaximumLength	= 20;
			ucs.Buffer			= _vec.data() + _vec.size();

			nt::NTSTATUS status = nt::RtlInt64ToUnicodeString(val, 10, &ucs);
			const USHORT charsWritten = ucs.Length / sizeof(WCHAR);
			_vec.resize( _vec.size() + charsWritten );

			if (align < 0)
			{
				const int blanks_to_insert = align - charsWritten;
				if (blanks_to_insert > 0)
				{
					_vec.append(fill, blanks_to_insert);
				}
			}

			return *this;
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
		/*
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
		}*/
	};
}