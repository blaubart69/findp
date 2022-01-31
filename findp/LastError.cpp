#include "stdafx.h"

#include "LastError.h"
#include "Write.h"

namespace bee
{
	void LastError::print(void)
	{
		wstring buf;
		buf.appendW(L"E ");
		buf.append_ll(_rc);
		buf.push_back(L'\t'); buf.appendA(_func);
		buf.push_back(L'\t'); buf.append(*_param);
		buf.push_back('\n');

		LastError err;
		if (Writer::Out().Write(buf, &err)->failed())
		{
			Beep(400, 2000);
		}
	}
}