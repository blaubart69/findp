#include "LastError.h"
#include "Write.h"

namespace bee
{
	void LastError::print(void)
	{
		wstring buf;
		buf.append(L"E ");
		buf.append_ull(_rc);
		if (_func != nullptr)
		{
			buf.push_back(L'\t').appendA(_func);
		}
		if (_param != nullptr)
		{
			buf.push_back(L'\t').append(*_param);
		}
		buf.append(L"\r\n");

		LastError err;
		if (Err->Write(buf, &err).failed())
		{
			Beep(400, 2000);
		}
	}
}