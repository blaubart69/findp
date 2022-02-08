#include "pch.h"
#include "CppUnitTest.h"

#include "../findp/libbee/beewstring.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestLibBee
{
	TEST_CLASS(Test_wstring)
	{
	public:
		
		TEST_METHOD(create_instance)
		{
			bee::wstring v;
			Assert::IsTrue(0 == v.length());
		}
		TEST_METHOD(c_str)
		{
			bee::wstring s;
			s.push_back('b');
			const WCHAR* cs = s.c_str();
			Assert::IsTrue(1 == s.length());
			Assert::AreEqual(L"b", cs);
		}
		TEST_METHOD(int_to_string)
		{
			bee::wstring s;
			s.append_ull(17);
			Assert::AreEqual(L"17", s.c_str());
		}
		/*
		TEST_METHOD(negative_int_to_string)
		{
			bee::wstring s;
			s.append_ll(-17);
			Assert::AreEqual(L"-17", s.c_str());
		}*/
		TEST_METHOD(Nui_schaumma_si_a_au)
		{
			bee::wstring s;
			s.append_ull(0);
			Assert::AreEqual(L"0", s.c_str());
		}
		TEST_METHOD(two_digit_number)
		{
			bee::wstring s;
			s.append_ull(17);
			Assert::IsTrue(s.length() == 2);
			Assert::AreEqual(L"17", s.c_str());
		}
		TEST_METHOD(ten_digit_number)
		{
			bee::wstring s;
			      s.append_ull(3363545088);
			Assert::AreEqual(L"3363545088", s.c_str());
		}
		TEST_METHOD(ull_UINT64_MAX)
		{
			bee::wstring s;

			s.append(L"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");
			s.resize(0);
			      s.append_ull(18446744073709551615U);
			Assert::AreEqual(L"18446744073709551615", s.c_str());
		}
		TEST_METHOD(ull_UINT64_MAX_hex)
		{
			bee::wstring s;
			s.append_ull(0xFFFFFFFFFFFFFFFF);
			Assert::AreEqual(L"18446744073709551615", s.c_str());
		}
		TEST_METHOD(ten_digit_number_aligned)
		{
			bee::wstring s;
			s.append_ull(3363545088,12);
			Assert::AreEqual(L"  3363545088", s.c_str());
		}
		TEST_METHOD(ten_digit_number_bigger_than_align)
		{
			bee::wstring s;
			s.append_ull(3363545088, 8);
			Assert::AreEqual(L"3363545088", s.c_str());
		}
	};
}
