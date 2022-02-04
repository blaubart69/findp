#include "stdafx.h"

#include "beewstring.h"
#include "utils.h"
#include "findp.h"

bee::LastError& GetOwner(LPCWSTR filename, bee::wstring* owner, bee::LastError* lastErr);

bee::LastError& ConvertFiletimeToLocalTime(const FILETIME* filetime, SYSTEMTIME* localTime, bee::LastError* lastErr)
{
	SYSTEMTIME UTCSysTime;
	if (!FileTimeToSystemTime(filetime, &UTCSysTime))
	{
		lastErr->set("FileTimeToSystemTime");
	}
	else if (!SystemTimeToTzSpecificLocalTime(NULL, &UTCSysTime, localTime))
	{
		lastErr->set("FileTimeToSystemTime");
	}

	return *lastErr;
}

void Append_Time_Attributes_Size(nt::FILE_DIRECTORY_INFORMATION* finddata, bee::wstring* outBuffer, bee::LastError* lastErr)
{
	SYSTEMTIME localTime;

	if (ConvertFiletimeToLocalTime((FILETIME*)&(finddata->LastWriteTime), &localTime, lastErr).failed())
	{
		lastErr->print();
	}
	else
	{
		DWORD attrs = finddata->FileAttributes;

		outBuffer->appendf(
			L"%04u-%02u-%02u %02u:%02u:%02u"
			L"\t%c%c%c%c%c%c%c"
			L"\t%12I64u"
			L"\t",
			localTime.wYear, localTime.wMonth, localTime.wDay
			, localTime.wHour, localTime.wMinute, localTime.wSecond
			, ((attrs & FILE_ATTRIBUTE_ARCHIVE) != 0) ? L'A' : L'-'
			, ((attrs & FILE_ATTRIBUTE_SYSTEM) != 0) ? L'S' : L'-'
			, ((attrs & FILE_ATTRIBUTE_HIDDEN) != 0) ? L'H' : L'-'
			, ((attrs & FILE_ATTRIBUTE_READONLY) != 0) ? L'R' : L'-'
			, ((attrs & FILE_ATTRIBUTE_DIRECTORY) != 0) ? L'D' : L'-'
			, ((attrs & FILE_ATTRIBUTE_ENCRYPTED) != 0) ? L'E' : L'-'
			, ((attrs & FILE_ATTRIBUTE_COMPRESSED) != 0) ? L'C' : L'-'
			, finddata->EndOfFile.QuadPart);
	}
}

bee::LastError& PrintEntry(const bee::wstring& FullBaseDir, nt::FILE_DIRECTORY_INFORMATION* finddata, bee::wstring* outBuffer, bool printFull, bool printOwner, bool printQuoted, bee::LastError* lastErr)
{
	if (printFull)
	{
		Append_Time_Attributes_Size(finddata, outBuffer, lastErr);

		if (printOwner)
		{
			bee::wstring owner;

			bee::wstring tmpFullfilename;
			tmpFullfilename.assign(FullBaseDir);
			tmpFullfilename.push_back(L'\\');
			tmpFullfilename.append(finddata->FileName, finddata->FileNameLength / sizeof(WCHAR));

			if (GetOwner(tmpFullfilename.c_str(), &owner, lastErr).failed())
			{
				owner.assign(L"n/a");
			}

			outBuffer->push_back(L'\t');
			outBuffer->append(owner);
			outBuffer->push_back(L'\t');
		}
	}

	outBuffer->append(FullBaseDir);
	outBuffer->push_back(L'\\');
	outBuffer->append(finddata->FileName, finddata->FileNameLength / sizeof(WCHAR));
	outBuffer->appendW(L"\r\n");
	
	return *lastErr;
}

void ProcessEntry(const bee::wstring& FullBaseDir, nt::FILE_DIRECTORY_INFORMATION* finddata, Context* ctx, bee::wstring* outBuffer, bee::LastError* lastErr)
{
	bool matched;
	if (   ctx->opts.FilenameSubstringPattern	== NULL
		&& ctx->opts.extToSearch				== NULL)
	{
		matched = true;
	}
	else
	{
		matched = false;

		if (ctx->opts.FilenameSubstringPattern != NULL)
		{
			std::wstring_view pattern(ctx->opts.FilenameSubstringPattern, lstrlenW(ctx->opts.FilenameSubstringPattern));
			std::wstring_view filename(finddata->FileName, finddata->FileNameLength / sizeof(WCHAR));

			auto found = std::search(
				filename.begin(), filename.end(),   // haystack
				pattern.begin(),  pattern.end(),	// needle
				[](const wchar_t a, const wchar_t b) {
				  return  
						 // If the high-order word of this parameter is zero, 
						 // the low-order word must contain a single character to be converted.
					     CharUpperW((LPWSTR)a)
					  == CharUpperW((LPWSTR)b);
				});
			
			matched = found != filename.end();
		}
		if (ctx->opts.extToSearch != NULL && ! matched)
		{
			matched |= endsWith(finddata->FileName,	finddata->FileNameLength / sizeof(WCHAR), 
								 ctx->opts.extToSearch, ctx->opts.extToSearchLen);
		}
	}

	if (matched)
	{
		InterlockedIncrement64(&ctx->stats.filesMatched);
		InterlockedAdd64      (&ctx->stats.sumFileSizeMatched, finddata->EndOfFile.QuadPart);
	}

	if (!ctx->opts.sum)
	{
		if ( matched )
		{
			if (   (ctx->opts.emit == EmitType::Both)
				|| (ctx->opts.emit == EmitType::Files && isFile     (finddata->FileAttributes))
				|| (ctx->opts.emit == EmitType::Dirs  && isDirectory(finddata->FileAttributes))  )
			{
				PrintEntry(FullBaseDir, finddata, outBuffer, ctx->opts.printFull, ctx->opts.printOwner, ctx->opts.quoteFilename, lastErr);
			}
		}
	}

	if (ctx->opts.GroupExtensions && isFile(finddata->FileAttributes) )
	{
		ProcessExtension(ctx->ext, finddata->FileName, finddata->EndOfFile.QuadPart);
	}
}

