#include "stdafx.h"

#include "findp.h"

/*
bool FindExtension(LPCWSTR filename, LPWSTR* extension)
{
	WCHAR* dotPos = NULL;
	WCHAR* p = (WCHAR*)filename;

	for (; *p != L'\0'; p++)
	{
		if (*p == L'.')
		{
			dotPos = p;
		}
	}

	if (dotPos == NULL)
	{
		*extension = p;
		return false;
	}

	dotPos += 1;

	if (dotPos == p)
	{
		*extension = p;
		return false;
	}

	*extension = dotPos;
	return true;
}

void ProcessExtension(Extensions *ext, LPCWSTR filename, LONGLONG filesize)
{
	LPWSTR extension;

	FindExtension(filename, &extension);
	MikeHT_Insert(ext->extsHashtable, extension, filesize);
}
*/

std::wstring_view find_extension(const std::wstring_view& filename)
{
	std::wstring_view ext;

	for (auto iter = filename.crbegin(); iter != filename.crend(); ++iter)
	{
		//        iter
		//        |  crbegin()
		//        |  |
		//        v  v
		//  0123456789
		// "bumsti.txt"
		//
		//  01234
		// "a.txt"
		if (*iter == L'\.')
		{
			const ptrdiff_t length_extension = iter - filename.crbegin();
			const size_t    idx_begin_ext = filename.length() - length_extension;
			ext = std::wstring_view(filename.data() + idx_begin_ext, length_extension);
			break;
		}
	}

	return ext;
}

void ProcessExtension(Extensions* ext, const std::wstring_view& filename, LONGLONG filesize)
{
	//LPWSTR extension;
	//FindExtension(filename, &extension);
	//MikeHT_Insert(ext->extsHashtable, extension, filesize);
	std::wstring_view extension = find_extension(filename);
	MikeHT_Insert2(ext->extsHashtable, extension.data(), extension.length(), filesize);
}

