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
		lastErr->set("SystemTimeToTzSpecificLocalTime");
	}

	return *lastErr;
}

static const wchar_t DASH = L'-';

void append_systemtime(const SYSTEMTIME& time, bee::wstring* str)
{
	str->append_ull(time.wYear,   4, L'0').push_back(DASH)
		.append_ull(time.wMonth,  2, L'0').push_back(DASH)
		.append_ull(time.wDay,    2, L'0').push_back(L' ')
		.append_ull(time.wHour,   2, L'0').push_back(L':')
		.append_ull(time.wMinute, 2, L'0').push_back(L':')
		.append_ull(time.wSecond, 2, L'0');
}

void append_attributes(const DWORD attrs, bee::wstring* str)
{
	str->push_back(((attrs & FILE_ATTRIBUTE_ARCHIVE)    != 0) ? L'A' : DASH)
		.push_back(((attrs & FILE_ATTRIBUTE_SYSTEM)     != 0) ? L'S' : DASH)
		.push_back(((attrs & FILE_ATTRIBUTE_HIDDEN)     != 0) ? L'H' : DASH)
		.push_back(((attrs & FILE_ATTRIBUTE_READONLY)   != 0) ? L'R' : DASH)
		.push_back(((attrs & FILE_ATTRIBUTE_DIRECTORY)  != 0) ? L'D' : DASH)
		.push_back(((attrs & FILE_ATTRIBUTE_ENCRYPTED)  != 0) ? L'E' : DASH)
		.push_back(((attrs & FILE_ATTRIBUTE_COMPRESSED) != 0) ? L'C' : DASH);
}

void Append_Time_Attributes_Size(nt::FILE_DIRECTORY_INFORMATION* finddata, bee::wstring* outBuffer, bee::LastError* lastErr)
{
	SYSTEMTIME localTime;

	if (ConvertFiletimeToLocalTime((FILETIME*)&(finddata->LastWriteTime), &localTime, lastErr).failed())
	{
		lastErr->print();
		//                  yyyy-MM-dd HH:mm:ss
		outBuffer->append(L"E cnv LastWriteTime");
	}
	else
	{
		append_systemtime(localTime, outBuffer);				outBuffer->push_back(L'\t');
	}
	append_attributes(finddata->FileAttributes, outBuffer);		outBuffer->push_back(L'\t');
	outBuffer->append_ull(finddata->EndOfFile.QuadPart, 12);	outBuffer->push_back(L'\t');
}

bee::LastError& PrintEntry(const bee::wstring& FullBaseDir, nt::FILE_DIRECTORY_INFORMATION* finddata, const std::wstring_view& filename, bee::wstring* outBuffer, bool printFull, bool printOwner, bool printQuoted, bee::LastError* lastErr)
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
			tmpFullfilename.append(filename.data(), filename.length());

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
	outBuffer->append(filename.data(), filename.length());
	outBuffer->append(L"\r\n");
	
	return *lastErr;
}

void ProcessEntry(const bee::wstring& FullBaseDir, nt::FILE_DIRECTORY_INFORMATION* finddata, std::wstring_view filename, Context* ctx, bee::wstring* outBuffer, bee::LastError* lastErr)
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
			//std::wstring_view filename(finddata->FileName, finddata->FileNameLength / sizeof(WCHAR));

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
			matched |= endsWith(filename.data(), filename.length(),
								 ctx->opts.extToSearch, ctx->opts.extToSearchLen);
		}
	}

	if (matched)
	{
		InterlockedIncrement64(&ctx->stats.filesMatched);
		InterlockedAdd64      (&ctx->stats.sumFileSizeMatched, finddata->EndOfFile.QuadPart);

		if (!ctx->opts.sum)
		{
			if ((   ctx->opts.emit == EmitType::Both)
				|| (ctx->opts.emit == EmitType::Files && isFile(finddata->FileAttributes))
				|| (ctx->opts.emit == EmitType::Dirs  && isDirectory(finddata->FileAttributes)))
			{
				PrintEntry(FullBaseDir, finddata, filename, outBuffer, ctx->opts.printFull, ctx->opts.printOwner, ctx->opts.quoteFilename, lastErr);
			}
		}
	}


	if (ctx->opts.GroupExtensions && isFile(finddata->FileAttributes) )
	{
		ProcessExtension(ctx->ext, filename, finddata->EndOfFile.QuadPart);
	}
}

