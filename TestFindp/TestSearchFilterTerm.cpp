//#include "gtest/gtest.h"
#include "pch.h"

TEST(FilterTerm, EmptyFilter) {

	LPWSTR term = L"c:\bumsti";
	LPCWSTR filter;

	bool rc = GetSearchFilterFromDir(term, lstrlen(term), &filter);

	EXPECT_EQ(false, rc);
	EXPECT_EQ(nullptr, filter);
	ASSERT_STREQ(L"c:\bumsti", term);
}

TEST(FilterTerm, OnlyFilter) {

	LPWSTR term = L"*.txt";
	LPCWSTR filter;

	bool rc = GetSearchFilterFromDir(term, lstrlen(term), &filter);

	EXPECT_EQ(true, rc);
	EXPECT_EQ(term, filter);
	ASSERT_STREQ(L"*.txt", filter);
}

TEST(FilterTerm, FilterWithBackslashInFront) {

	WCHAR term[] = L"\\*.txt";
	LPCWSTR filter;

	bool rc = GetSearchFilterFromDir(term, lstrlen(term), &filter);

	EXPECT_EQ(true, rc);
	ASSERT_STREQ(L"", term);
	ASSERT_STREQ(L"*.txt", filter);
}
TEST(FilterTerm, DirectoryWithFilter) {

	WCHAR term[] = L"a\\*.xml";
	LPCWSTR filter;

	bool rc = GetSearchFilterFromDir(term, lstrlen(term), &filter);

	EXPECT_EQ(true, rc);
	ASSERT_STREQ(L"a", term);
	ASSERT_STREQ(L"*.xml", filter);
}