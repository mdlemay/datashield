//===----------------------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

// <string>

// template<> struct char_traits<char32_t>

// static size_t length(const char_type* s);

#include <string>
#include <cassert>

#include "test_macros.h"

int main()
{
#ifndef _LIBCPP_HAS_NO_UNICODE_CHARS
#if TEST_STD_VER >= 11
    assert(std::char_traits<char32_t>::length(U"") == 0);
    assert(std::char_traits<char32_t>::length(U"a") == 1);
    assert(std::char_traits<char32_t>::length(U"aa") == 2);
    assert(std::char_traits<char32_t>::length(U"aaa") == 3);
    assert(std::char_traits<char32_t>::length(U"aaaa") == 4);
#endif
#endif  // _LIBCPP_HAS_NO_UNICODE_CHARS
}
