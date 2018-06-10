#include "pch.h"

#include "..\findp\findp.h"

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

	LONGLONG val;
	DWORD count = MikeHT_ForEach(ext.extsHashtable, [](auto key, auto val, auto ctx) {}, NULL, NULL);
	EXPECT_EQ(0, count);
	EXPECT_EQ(11, ext.noExtSum);
}

TEST(ProcessExtension, DotAtEnd) {

	Extensions ext;

	ProcessExtension(&ext, L"kaungornedsei.", 12);

	LONGLONG val;
	DWORD count = MikeHT_ForEach(ext.extsHashtable, [](auto key, auto val, auto ctx) {}, NULL, NULL);
	EXPECT_EQ(0, count);
	EXPECT_EQ(12, ext.noExtSum);
}

TEST(ProcessExtension, OnlyDot) {

	Extensions ext;

	ProcessExtension(&ext, L".", 22);

	LONGLONG val;
	DWORD count = MikeHT_ForEach(ext.extsHashtable, [](auto key, auto val, auto ctx) {}, NULL, NULL);
	EXPECT_EQ(0, count);
	EXPECT_EQ(22, ext.noExtSum);
}
