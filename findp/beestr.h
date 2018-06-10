#pragma once

#include "stdafx.h"

typedef struct _BEESTR
{
	DWORD	len;
	WCHAR	str[1];
} BEESTR;