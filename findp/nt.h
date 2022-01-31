#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

//
// Generic test for success on any status value (non-negative numbers
// indicate success).
//

#ifndef NT_SUCCESS
#define NT_SUCCESS(Status) (((nt::NTSTATUS)(Status)) >= 0)
#endif

namespace nt
{
    #include <winternl.h>

    extern "C"
    {
        //typedef _Return_type_success_(return >= 0) LONG NTSTATUS;

        int swprintf_s(
            wchar_t* buffer,
            size_t sizeOfBuffer,
            const wchar_t* format,
            ...
        );

        int vswprintf_s(
            wchar_t* buffer,
            size_t numberOfElements,
            const wchar_t* format,
            va_list argptr
        );

        NTSYSAPI
            NTSTATUS
            NTAPI
            RtlInt64ToUnicodeString(
                _In_ ULONGLONG Value,
                _In_opt_ ULONG Base,
                _Inout_ PUNICODE_STRING String
            );

        NTSYSAPI NTSTATUS RtlUnicodeStringToInteger(
            PCUNICODE_STRING String,
            ULONG            Base,
            PULONG           Value
        );

        

#undef RtlMoveMemory
        __declspec(dllimport) void __stdcall RtlMoveMemory(void* dst, const void* src, size_t len);

#undef RtlFillMemory
        __declspec(dllimport) void __stdcall RtlFillMemory(void* Destination, size_t Length, int Fill);

        typedef struct _FILE_DIRECTORY_INFORMATION {
            ULONG NextEntryOffset;
            ULONG FileIndex;
            LARGE_INTEGER CreationTime;
            LARGE_INTEGER LastAccessTime;
            LARGE_INTEGER LastWriteTime;
            LARGE_INTEGER ChangeTime;
            LARGE_INTEGER EndOfFile;
            LARGE_INTEGER AllocationSize;
            ULONG FileAttributes;
            ULONG FileNameLength;
            _Field_size_bytes_(FileNameLength) WCHAR FileName[1];
        } FILE_DIRECTORY_INFORMATION, * PFILE_DIRECTORY_INFORMATION;

        __kernel_entry NTSYSCALLAPI
            NTSTATUS
            NTAPI
            NtQueryDirectoryFile(
                _In_ HANDLE FileHandle,
                _In_opt_ HANDLE Event,
                _In_opt_ PIO_APC_ROUTINE ApcRoutine,
                _In_opt_ PVOID ApcContext,
                _Out_ PIO_STATUS_BLOCK IoStatusBlock,
                _Out_writes_bytes_(Length) PVOID FileInformation,
                _In_ ULONG Length,
                _In_ FILE_INFORMATION_CLASS FileInformationClass,
                _In_ BOOLEAN ReturnSingleEntry,
                _In_opt_ PUNICODE_STRING FileName,
                _In_ BOOLEAN RestartScan
            );
    }
}