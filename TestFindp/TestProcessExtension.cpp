#include "pch.h"


TEST(ProcessExtension, OneExtension) {

	Extensions ext;

	ProcessExtension(&ext, L"bumsti.txt", 1);

	LONGLONG val;
	bool found = MikeHT_Get(ext.extsHashtable, L"txt", &val);
	EXPECT_TRUE(found);
	EXPECT_EQ(1, val);
}

TEST(ProcessExtension, OnlyExtension) {

	Extensions ext;

	ProcessExtension(&ext, L".txt", 1);

	LONGLONG val;
	bool found = MikeHT_Get(ext.extsHashtable, L"txt", &val);
	EXPECT_TRUE(found);
	EXPECT_EQ(1, val);
}

TEST(ProcessExtension, NoExtension) {

	Extensions ext;

	ProcessExtension(&ext, L"NixIsDo", 11);

	LONGLONG noExtSum = -1;
	DWORD count = MikeHT_ForEach(ext.extsHashtable, 
		[](auto key, auto val, auto ctx) 
	{
		LONGLONG *sum = (LONGLONG*)ctx;
		if (lstrlen(key) == 0)
		{
			*sum = val;
		}
	}, NULL, &noExtSum);
	EXPECT_EQ(1, count);
	EXPECT_EQ(11, noExtSum);
}

TEST(ProcessExtension, DotAtEnd) {

	Extensions ext;

	ProcessExtension(&ext, L"kaungornedsei.", 12);

	DWORD count = MikeHT_ForEach(ext.extsHashtable, [](auto key, auto val, auto ctx) {}, NULL, NULL);
	EXPECT_EQ(1, count);

	LONGLONG noExtSum = -1;
	MikeHT_Get(ext.extsHashtable, L"", &noExtSum);

	EXPECT_EQ(12, noExtSum);
}

TEST(ProcessExtension, OnlyDot) {

	Extensions ext;

	ProcessExtension(&ext, L".", 22);

	DWORD count = MikeHT_ForEach(ext.extsHashtable, [](auto key, auto val, auto ctx) {}, NULL, NULL);
	EXPECT_EQ(1, count);

	LONGLONG noExtSum = -1;
	MikeHT_Get(ext.extsHashtable, L"", &noExtSum);
	EXPECT_EQ(22, noExtSum);
}
TEST(ProcessExtension, CaseInSensitive) {

	Extensions ext;

	ProcessExtension(&ext, L".txt", 1);
	ProcessExtension(&ext, L".Txt", 1);
	ProcessExtension(&ext, L".txT", 1);
	ProcessExtension(&ext, L".tXt", 1);
	ProcessExtension(&ext, L".TXT", 1);

	LONGLONG val;
	bool found = MikeHT_Get(ext.extsHashtable, L"txt", &val);
	EXPECT_TRUE(found);
	EXPECT_EQ(5, val);
}