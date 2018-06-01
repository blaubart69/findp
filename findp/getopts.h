#pragma once

#include "stdafx.h"

struct Options
{
	std::wstring rootDir;
	bool sum;
};

int getopts(int argc, wchar_t *argv[], Options* opts);