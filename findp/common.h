#pragma once

struct Stats
{
	__int64 files = 0;
	__int64 dirs = 0;
	__int64 sumFileSize = 0;
};

struct Context
{
	Stats stats;
	bool sum;
};
