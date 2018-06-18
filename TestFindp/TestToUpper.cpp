#include "pch.h"

TEST(TestUpper, TestString) {

	WCHAR x[] = L"berni";

	LPWSTR res = CharUpperW(x);

	EXPECT_TRUE( lstrcmp(x,L"BERNI") == 0 );
}

TEST(TestUpper, UpperOneChar) {

	LPWSTR x = L"berni";
	WCHAR l = x[0];

	LPWSTR res = CharUpperW((LPWSTR)l);
	WCHAR u = LOWORD(res);

	EXPECT_EQ(L'B', u);
}