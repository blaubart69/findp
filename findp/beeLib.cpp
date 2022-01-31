#ifndef _UNICODE
#define _UNICODE
#define  UNICODE
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>

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

//
// The _purecall function is a Microsoft-specific implementation detail of the Microsoft Visual C++ compiler. 
// This function is not intended to be called by your code directly, and it has no public header declaration. 
// It is documented here because it is a public export of the C Runtime Library.
// https://msdn.microsoft.com/en-us/library/ff798096.aspx
//
extern "C" int __cdecl _purecall() { return -1; }

extern "C" {
	//__declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn(void) {}
	
	//#undef RtlMoveMemory
	//__declspec(dllimport) void __stdcall RtlMoveMemory(void *dst, const void *src, size_t len);

	//	
//#undef memmove
//	void* memmove(void *dst, const void *src, size_t len) {
//		RtlMoveMemory(dst, src, len);
//		return dst;
//	}
//	
//	
//	#undef RtlCopyMemory
//	__declspec(dllimport) void __stdcall RtlCopyMemory(void *Destination, const void *Source, size_t Length);
//	
//	//#define RtlCopyMemory(Destination,Source,Length) memcpy((Destination),(Source),(Length))
//	//#define memcpy(Destination,Source,Length) RtlCopyMemory((Destination),(Source),(Length))
//
	//#undef memcpy
	//void* memcpy(void * destination, const void * source, size_t num);
	//void* memcpy(void * destination, const void * source, size_t num)
	//{
	//	RtlMoveMemory(destination, source, num);
	//	return destination;
	//}
}


void rawmain(void)
{
	int argc;
	LPTSTR* argv;
	argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	int Exitcode = beeMain(argc, argv);

	HeapFree(GetProcessHeap(), 0, argv);

	ExitProcess(Exitcode);
}