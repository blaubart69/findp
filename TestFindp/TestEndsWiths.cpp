#include "pch.h"

TEST(TestendsWith, SameSame) {

	EXPECT_TRUE(endsWith(L"berni.txt", L".txt"));
}
TEST(TestendsWith, NotSameSame) {

	EXPECT_FALSE(endsWith(L"berni.txt", L".txtx"));
}
TEST(TestendsWith, endTooLong) {

	EXPECT_FALSE(endsWith(L"berni.txt", L".txtxaaaaaaaaaaaaaa"));
}
TEST(TestendsWith, kaEnd) {
	//
	// the .NET endsWith returns TRUE if the search string is an empty string
	//
	EXPECT_TRUE(endsWith(L"berni.txt", L""));
}
TEST(TestendsWith, shortestSame) {

	EXPECT_TRUE(endsWith(L".", L"."));
}
TEST(TestendsWith, shortestNotSame) {

	EXPECT_FALSE(endsWith(L"x", L"."));
}
