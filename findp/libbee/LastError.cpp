#include "LastError.h"
#include "Write.h"

namespace bee
{
	void LastError::print(void)
	{
		wstring buf;
		buf.appendW(L"E ");
		buf.append_ll(_rc);
		if (_func != nullptr)
		{
			buf.push_back(L'\t').appendA(_func);
		}
		if (_param != nullptr)
		{
			buf.push_back(L'\t').append(*_param);
		}
		buf.push_back('\n');

		LastError err;
		if (Out->Write(buf, &err)->failed())
		{
			Beep(400, 2000);
		}
	}
}