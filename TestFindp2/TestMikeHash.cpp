#include "pch.h"
#include "CppUnitTest.h"

#include "../findp/MikeHash.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestFindp2
{
	TEST_CLASS(TestMikeHash)
	{
	public:
		
		TEST_METHOD(InsertAndCheckValues) {

			HT* ht = MikeHT_Init(10);

			MikeHT_Insert2(ht, L"doc", 3, 1);
			MikeHT_Insert2(ht, L"txt", 3, 2);
			MikeHT_Insert2(ht, L"doc", 3, 3);
			MikeHT_Insert2(ht, L"doc", 3, 4);

			LONGLONG val;
			MikeHT_Get(ht, L"doc", &val);
			Assert::IsTrue(8 == val);

			MikeHT_Get(ht, L"txt", &val);
			Assert::IsTrue(2 == val);

			MikeHT_Free(ht);
		}

		TEST_METHOD(InsertAndCheckValuesCaseInsensitive) {

			HT* ht = MikeHT_Init(10);

			MikeHT_Insert2(ht, L"doc", 3, 1);
			MikeHT_Insert2(ht, L"txt", 3, 2);
			MikeHT_Insert2(ht, L"doc", 3, 3);
			MikeHT_Insert2(ht, L"doc", 3, 4);
			MikeHT_Insert2(ht, L"DOC", 3, 4);

			LONGLONG val;
			MikeHT_Get(ht, L"doc", &val);
			Assert::IsTrue(12 == val);

			MikeHT_Get(ht, L"txt", &val);
			Assert::IsTrue(2 == val);

			MikeHT_Free(ht);
		}
		TEST_METHOD(TraverseTable) {


			HT* ht = MikeHT_Init(10);

			MikeHT_Insert2(ht, L"doc", 3, 1);
			MikeHT_Insert2(ht, L"txt", 3, 2);
			MikeHT_Insert2(ht, L"doc", 3, 3);
			MikeHT_Insert2(ht, L"doc", 3, 4);

			DWORD itemCount = MikeHT_ForEach(ht, [](LPWSTR Key, size_t KeyLen, LONGLONG Val, auto count, auto ctx) {}, NULL, NULL);
			Assert::IsTrue(2 == itemCount);

			MikeHT_Free(ht);
		}

		TEST_METHOD(TraverseTable_3_items) {


			HT* ht = MikeHT_Init(10);

			MikeHT_Insert2(ht, L"doc",   3, 1);
			MikeHT_Insert2(ht, L"txt",   3, 2);
			MikeHT_Insert2(ht, L"doc",   3, 3);
			MikeHT_Insert2(ht, L"doc",   3, 4);
			MikeHT_Insert2(ht, L"docxl", 5, 4);

			DWORD itemCount = MikeHT_ForEach(ht, [](LPWSTR Key, size_t KeyLen, LONGLONG Val, auto count, auto ctx) {}, NULL, NULL);
			Assert::IsTrue(3 == itemCount);

			MikeHT_Free(ht);
		}
	};
}
