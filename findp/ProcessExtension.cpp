#include "stdafx.h"
#include "findp.h"

bool FindExtension(LPCWSTR filename, LPWSTR *extension);

void ProcessExtension(Extensions *ext, LPCWSTR filename, LONGLONG filesize)
{
	LPWSTR extension;

	if ( ! FindExtension(filename, &extension) )
	{
		InterlockedAdd64(&ext->noExtSum, filesize);
	}
	else
	{
		MikeHT_Insert(ext->extsHashtable, extension, filesize);
	}
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
		*extension = NULL;
		return false;
	}
	
	dotPos += 1;

	if (dotPos == p)
	{
		*extension = NULL;
		return false;
	}

	*extension = dotPos;
	return true;
}