#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

std::wstring_view find_extension(const std::wstring_view& filename)
{
	std::wstring_view ext;

	for (auto iter = filename.crbegin(); iter != filename.crend(); ++iter)
	{
		//        iter
		//        |  crbegin()
		//        |  |
		//        v  v
		//  0123456789
		// "bumsti.txt"
		//
		//  01234
		// "a.txt"
		if (*iter == L'\.')
		{
			const ptrdiff_t length_extension = iter - filename.crbegin();
			const size_t    idx_begin_ext    = filename.length() - length_extension;
			ext = std::wstring_view(filename.data() + idx_begin_ext, length_extension);
			break;
		}
	}

	return ext;
}

namespace TestFindp2
{
	TEST_CLASS(TestFindExtension)
	{
	public:

		TEST_METHOD(txt) {

			std::wstring s(L"bumsti.txt");
			auto e = find_extension(s);
			Assert::AreEqual(L"txt", e.data());
		}
		TEST_METHOD(noextension) {

			std::wstring s(L"bumsti");
			auto e = find_extension(s);
			Assert::IsTrue(0 == e.length());
		}
		TEST_METHOD(one_char_noextension) {

			std::wstring s(L"b");
			auto e = find_extension(s);
			Assert::IsTrue(0 == e.length());
		}
		TEST_METHOD(dot_in_the_beginning) {

			std::wstring s(L".koarl");
			auto e = find_extension(s);
			Assert::IsTrue(5 == e.length());
			Assert::AreEqual(L"koarl", e.data());
		}
		TEST_METHOD(dot_in_the_end) {

			std::wstring s(L"koarl.");
			auto e = find_extension(s);
			Assert::IsTrue(0 == e.length());
		}
	};
}