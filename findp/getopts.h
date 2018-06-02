#pragma once

#include "stdafx.h"

struct Options
{
	std::wstring rootDir;
	bool sum;
	bool progress;
};

int getopts(int argc, wchar_t *argv[], Options* opts);