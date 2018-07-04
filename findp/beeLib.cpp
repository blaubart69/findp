#ifndef _UNICODE
#define _UNICODE
#define  UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "beeLib.h"

void * __cdecl operator new (size_t size)
{
	return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
}

void __cdecl operator delete(void *ptrToRelease, size_t size)
{
	HeapFree(GetProcessHeap(), 0, ptrToRelease);
}

//void __cdecl std::_Xlength_error(char const *) {}

void rawmain(void)
{
	int argc;
	LPTSTR* argv;
	argv = CommandLineToArgv2(GetCommandLineW(), &argc);

	int Exitcode = beeMain(argc, argv);

	HeapFree(GetProcessHeap(), 0, argv);

	ExitProcess(Exitcode);
}