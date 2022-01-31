#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//#include <type_traits>
#include <new>

namespace bee
{
	template<typename T>
	class vector
	{
	public:
		vector& push_back(const T& _Val) 
		{
			ensureCapacity(_len + 1);
			T* newObj = &(_array[_len]);
			new(newObj) T(_Val);
			_len += 1;
			return *this;
		}
		vector& append(const T* vec, const size_t len)
		{
			for (int i = 0; i < len; ++i)
			{
				this->push_back(vec[i]);
			}
			return *this;
		}
		vector& append(const vector& vec)
		{
			append(vec.data(), vec.size());
			return *this;
		}
		vector& assign(const T* other, const size_t len)
		{
			resize(0);
			ensureCapacity(len);
			append(other, len);
			return *this;
		}
		vector& assign(const vector& other)
		{
			assign(other.data(), other.size());
			return *this;
		}
		vector& clear()
		{
			resize(0);
			return *this;
		}
		size_t size() const
		{
			return _len;
		}
		vector& resize(const size_t newSize)
		{
			if (newSize > _len)
			{
				ensureCapacity(newSize);

				for (size_t i = _len; i < newSize; ++i)
				{
					T* newObj = &(_array[i]);
					new(newObj) T;
				}
			}
			else if (newSize < _len)
			{
				size_t objsToDelete = _len - newSize;
				for (size_t idx = _len - 1; objsToDelete > 0 ; --idx, --objsToDelete)
				{
					((T*)&(_array[idx]))->~T();
				}
			}

			_len = newSize;
			return *this;
		}
		vector& reserve(size_t newCapacity)
		{
			ensureCapacity(newCapacity);
			return *this;
		}
		T* data() const
		{
			return _array;
		}
		T& operator[](size_t idx) const
		{
			return _array[idx];
		}
		~vector()
		{
			resize(0);

			if (_array != nullptr)
			{
				HeapFree(GetProcessHeap(), 0, _array);
			}
		}
		vector() 
			: _array(nullptr), _len(0), _capacity(0) 
		{
		}
		vector(const size_t capacity) 
		{
			_array = nullptr;
			_capacity = 0;
			_len = 0;
			ensureCapacity(capacity);
		}

		size_t align_to_64(size_t length)
		{
			const size_t c = 64 - 1;
			return (length + c) & ~c;
		}

		void ensureCapacity(const size_t wantCapacity)
		{
			if (_capacity < wantCapacity)
			{
				const size_t newCapacity = align_to_64(wantCapacity);

				if (_array == nullptr) 
				{ 
					_array = (T*)HeapAlloc  (GetProcessHeap(), 0,       sizeof(T) * newCapacity); 
				}
				else				   
				{ 
					_array = (T*)HeapReAlloc(GetProcessHeap(), 0, _array, sizeof(T) * newCapacity); 
				}

				if (_array == nullptr)
				{
					ExitProcess(ERROR_NOT_ENOUGH_MEMORY);
				}

				_capacity = newCapacity;
			}
		}

	private:

		T* _array;
		size_t _capacity;
		size_t _len;

	};
}