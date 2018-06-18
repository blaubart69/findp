#include "stdafx.h"

bool FindExtension(LPCWSTR filename, LPWSTR *extension);

void ProcessExtension(Extensions *ext, LPCWSTR filename, LONGLONG filesize)
{
	LPWSTR extension;

	FindExtension(filename, &extension);
	MikeHT_Insert(ext->extsHashtable, extension, filesize);
}

bool FindExtension(LPCWSTR filename, LPWSTR *extension)
{
	WCHAR *dotPos = NULL;
	WCHAR *p = (WCHAR*)filename;

	for ( ; *p != L'\0'; p++ )
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