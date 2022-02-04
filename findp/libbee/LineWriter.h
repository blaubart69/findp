#pragma once

class LineWriter
{
public:
	LineWriter(HANDLE filehandle, DWORD initialSize, pfWin32Err WinErrFunc);
	~LineWriter();

	BOOL appendUTF8BOM();

	BOOL append(LPCWSTR text, DWORD cchWideChar);
	BOOL appendf(LPCWSTR format, ...);

	BOOL writeBuffer();
	BOOL writeBuffer_keepBuffer();
	BOOL writef(LPCWSTR format, ...);

	void  setByteLength(DWORD len)	{ _builder->setByteLength(len); }
	DWORD getByteLength()			{ return _builder->getByteLength(); }
	void  reset()					{ _builder->reset(); }

private:

	const pfWin32Err	_winErrFunc;
	const HANDLE 		_filehandle;
	StringBuilder*	    _builder;

	BOOL write_buffer_to_filestream();
};
