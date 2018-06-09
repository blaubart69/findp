#include "pch.h"

#include "..\findp\MikeHash.h"

TEST(MikeHashTable, JustInsert) {
  

	HT *ht = MikeHT_Init(10);

	MikeHT_Insert(ht, L"doc", 1);
	MikeHT_Insert(ht, L"txt", 2);
	MikeHT_Insert(ht, L"doc", 3);
	MikeHT_Insert(ht, L"doc", 4);

	MikeHT_Free(ht);

	EXPECT_EQ(1, 1);
	EXPECT_TRUE(true);
}

TEST(MikeHashTable, InsertAndCheckValues) {


	HT *ht = MikeHT_Init(10);

	MikeHT_Insert(ht, L"doc", 1);
	MikeHT_Insert(ht, L"txt", 2);
	MikeHT_Insert(ht, L"doc", 3);
	MikeHT_Insert(ht, L"doc", 4);

	LONGLONG val;
	MikeHT_Get(ht, L"doc", &val);
	EXPECT_EQ(8, val);

	MikeHT_Get(ht, L"txt", &val);
	EXPECT_EQ(2, val);

	MikeHT_Free(ht);
}

TEST(MikeHashTable, TraverseTable) {


	HT *ht = MikeHT_Init(10);

	MikeHT_Insert(ht, L"doc", 1);
	MikeHT_Insert(ht, L"txt", 2);
	MikeHT_Insert(ht, L"doc", 3);
	MikeHT_Insert(ht, L"doc", 4);

	DWORD itemCount = MikeHT_ForEach(ht, [](LPWSTR Key, LONGLONG Val) {});
	EXPECT_EQ(2, itemCount);

	MikeHT_Free(ht);
}

TEST(MikeHashTable, TraverseTable_3_items) {


	HT *ht = MikeHT_Init(10);

	MikeHT_Insert(ht, L"doc", 1);
	MikeHT_Insert(ht, L"txt", 2);
	MikeHT_Insert(ht, L"doc", 3);
	MikeHT_Insert(ht, L"doc", 4);
	MikeHT_Insert(ht, L"docxl", 4);

	DWORD itemCount = MikeHT_ForEach(ht, [](LPWSTR Key, LONGLONG Val) {});
	EXPECT_EQ(3, itemCount);

	MikeHT_Free(ht);
}