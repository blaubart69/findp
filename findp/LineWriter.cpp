#include "stdafx.h"

LineWriter::LineWriter(HANDLE filehandle, UINT codepage, DWORD initialSize, pfWin32Err WinErrFunc)
	:		_filehandle(filehandle)
		,	_winErrFunc(WinErrFunc)
{
	_builder = new StringBuilder(codepage, initialSize, WinErrFunc);
}

LineWriter::~LineWriter()
{
	delete _builder;
}

BOOL LineWriter::append(LPCWSTR text, DWORD cchWideChar)
{
	return _builder->append(text, cchWideChar);
}

BOOL LineWriter::appendf(LPCWSTR format, ...)
{
	va_list args;
	va_start(args, format);
	BOOL rc = _builder->appendv(format, args);
	va_end(args);

	return rc;
}

BOOL LineWriter::appendUTF8BOM()
{
	BOOL ok;
	ok = _builder->appendByte(0xEF);
	ok = _builder->appendByte(0xBB);
	ok = _builder->appendByte(0xBF);

	return ok;
}

BOOL LineWriter::writeBuffer()
{
	BOOL ok = write_buffer_to_filestream();
	if (ok)
	{
		_builder->reset();
	}
	return ok;
}

BOOL LineWriter::writeBuffer_keepBuffer()
{
	return write_buffer_to_filestream();
}

BOOL LineWriter::writef(LPCWSTR format, ...)
{
	va_list args;
	va_start(args, format);
	BOOL rc = _builder->appendv(format, args);
	va_end(args);

	if (rc)
	{
		rc = write_buffer_to_filestream();
	}
	

	return rc;
}

BOOL LineWriter::write_buffer_to_filestream()
{
	DWORD written;
	BOOL ok = WriteFile(
		_filehandle
		, _builder->getBuffer()
		, _builder->getByteLength()
		, &written
		, NULL);

	if (!ok)
	{
		if (_winErrFunc)
		{
			_winErrFunc(L"WriteFile", L"TextWriter::write_buffer()");
		}
	}

	return ok;
}


