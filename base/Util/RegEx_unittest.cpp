#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/RegEx.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md
// NOTE: Tests drawn from https://hg.python.org/cpython/file/178075fbff3a/Lib/test/re_tests.py

//------------------------------------------------------------------------------
TEST (RegEx, matching)
  {

  //ASSERT_STREQ (strOut.AsChar (), szEncA);
  RegEx  rex;

  rex.Set ("abc"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("abc"); ASSERT_FALSE (rex.HasMatch ("xbc"));
  rex.Set ("abc"); ASSERT_FALSE (rex.HasMatch ("axc"));
  rex.Set ("abc"); ASSERT_FALSE (rex.HasMatch ("abx"));

  rex.Set ("abc"); ASSERT_STREQ (rex.Match ("xabcy").AsChar (), "abc");
  rex.Set ("abc"); ASSERT_STREQ (rex.Match ("ababc").AsChar (), "abc");
  rex.Set ("ab*c"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("ab*bc"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("ab*bc"); ASSERT_STREQ (rex.Match ("abbc").AsChar (), "abbc");
  rex.Set ("ab*bc"); ASSERT_STREQ (rex.Match ("abbbbc").AsChar (), "abbbbc");
  rex.Set ("ab+bc"); ASSERT_STREQ (rex.Match ("abbc").AsChar (), "abbc");
  rex.Set ("ab+bc"); ASSERT_FALSE (rex.HasMatch ("abc"));
  rex.Set ("ab+bc"); ASSERT_FALSE (rex.HasMatch ("abq"));
  rex.Set ("ab+bc"); ASSERT_STREQ (rex.Match ("abbbbc").AsChar (), "abbbbc");

  rex.Set ("ab?bc"); ASSERT_STREQ (rex.Match ("abbc").AsChar (), "abbc");
  rex.Set ("ab?bc"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("ab?bc"); ASSERT_FALSE (rex.HasMatch ("abbbbc"));
  rex.Set ("ab?c"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("ab?c"); ASSERT_STREQ (rex.Match ("ac").AsChar (), "ac");

  rex.Set ("^abc$"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("^abc$"); ASSERT_FALSE (rex.HasMatch ("abcc"));
  rex.Set ("^abc"); ASSERT_STREQ (rex.Match ("abcc").AsChar (), "abc");
  rex.Set ("^abc$"); ASSERT_FALSE (rex.HasMatch ("aabc"));
  rex.Set ("abc$"); ASSERT_STREQ (rex.Match ("aabc").AsChar (), "abc");

  rex.Set ("a.c"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("a.c"); ASSERT_STREQ (rex.Match ("axc").AsChar (), "axc");
  rex.Set ("a.*c"); ASSERT_STREQ (rex.Match ("axyzc").AsChar (), "axyzc");
  rex.Set ("a.*c"); ASSERT_FALSE (rex.HasMatch ("axyzd"));

  rex.Set ("a[bc]d"); ASSERT_FALSE (rex.HasMatch ("abc"));
  rex.Set ("a[bc]d"); ASSERT_STREQ (rex.Match ("abd").AsChar (), "abd");

  rex.Set ("a[b-d]e"); ASSERT_FALSE (rex.HasMatch ("abd"));
  rex.Set ("a[b-d]e"); ASSERT_STREQ (rex.Match ("ace").AsChar (), "ace");

  rex.Set ("a[b-d]"); ASSERT_STREQ (rex.Match ("aac").AsChar (), "ac");
  rex.Set ("a[-b]"); ASSERT_STREQ (rex.Match ("a-").AsChar (), "a-");
  rex.Set ("a[\\-b]"); ASSERT_STREQ (rex.Match ("a-").AsChar (), "a-");

  // NOTE:  Grouping still needs to be implemented.  For now, just test to make sure an overall match is made

  rex.Set ("(a)"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "a");
  rex.Set ("((a))"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "a");
  rex.Set ("(a)b(c)"); ASSERT_STREQ (rex.Match ("abc").AsChar (), "abc");
  rex.Set ("a+b+c"); ASSERT_STREQ (rex.Match ("aabbabc").AsChar (), "abc");

  rex.Set ("(a|b)*"); ASSERT_STREQ (rex.Match ("ab").AsChar (), "ab");

  //rex.Set ("(a+|b)*"); ASSERT_STREQ (rex.Match ("ab").AsChar (), "ab"); // Fail: infinite loop
  //rex.Set ("(a+|b)+"); ASSERT_STREQ (rex.Match ("ab").AsChar (), "ab"); // Fail: infinite loop
  //rex.Set ("(a+|b)?"); ASSERT_STREQ (rex.Match ("ab").AsChar (), "a"); // fail
  rex.Set ("[^ab]*"); ASSERT_STREQ (rex.Match ("cde").AsChar (), "cde");
  rex.Set ("abc"); ASSERT_FALSE (rex.HasMatch (""));
  //rex.Set ("a*"); ASSERT_TRUE (rex.HasMatch ("")); // Fail (need to debug... empty string may give false fail)
  rex.Set ("a|b|c|d|e"); ASSERT_STREQ (rex.Match ("e").AsChar (), "e");
  rex.Set ("(a|b|c|d|e)f"); ASSERT_STREQ (rex.Match ("ef").AsChar (), "ef");
  rex.Set ("abcd*efg"); ASSERT_STREQ (rex.Match ("abcdefg").AsChar (), "abcdefg");
  rex.Set ("ab*"); ASSERT_STREQ (rex.Match ("xabyabbbz").AsChar (), "ab");
  rex.Set ("ab*"); ASSERT_STREQ (rex.Match ("xayabbbz").AsChar (), "a");

  rex.Set ("(ab|cd)e"); ASSERT_STREQ (rex.Match ("abcde").AsChar (), "cde");
  rex.Set ("[abhgefdc]ij"); ASSERT_STREQ (rex.Match ("hij").AsChar (), "hij");
  rex.Set ("^(ab|cd)e"); ASSERT_FALSE (rex.HasMatch ("abcde"));

  // test
  rex.Set ("[ ]*"); ASSERT_STREQ (rex.Match ("a bc").AsChar (), " ");
  rex.Set ("[0-9]+"); ASSERT_STREQ (rex.Match (" 123 ").AsChar (), "123");

  rex.Set ("[0-9]+(\\.[1-9]+)?"); ASSERT_STREQ (rex.Match (" 123 ").AsChar (), "123");
  rex.Set ("[0-9]+(\\.[1-9]+)?"); ASSERT_STREQ (rex.Match (" 123.45 ").AsChar (), "123.45");


  rex.Set ("[ ]*([0-9]+(\\.[0-9]+)?|;)[ ]*"); ASSERT_STREQ (rex.Match (" 4").AsChar (), " 4");
  rex.Set ("(a|b|c|d|e)[ ]*"); ASSERT_STREQ (rex.Match (" b").AsChar (), "b");

/*
rex.Set ("abc*d");
rex.Set ("(a|b|c|d|e)");
rex.Set ("[ ]");
rex.Set ("[ab]");
rex.Set ("a*");
rex.Set ("[ ]*");
*/


/*

  RegEx  rexTokens ("[ ]*([0-9]+(\\.[0-9]+)?|;)[ ]*");


    ('^(ab|cd)e', 'abcde', FAIL, 'xg1y', 'xy'),
    ('(abc|)ef', 'abcdef', SUCCEED, 'found+"-"+g1', 'ef-'),
    ('(a|b)c*d', 'abcd', SUCCEED, 'found+"-"+g1', 'bcd-b'),
    ('(ab|ab*)bc', 'abc', SUCCEED, 'found+"-"+g1', 'abc-a'),
    ('a([bc]*)c*', 'abc', SUCCEED, 'found+"-"+g1', 'abc-bc'),
    ('a([bc]*)(c*d)', 'abcd', SUCCEED, 'found+"-"+g1+"-"+g2', 'abcd-bc-d'),
    ('a([bc]+)(c*d)', 'abcd', SUCCEED, 'found+"-"+g1+"-"+g2', 'abcd-bc-d'),
    ('a([bc]*)(c+d)', 'abcd', SUCCEED, 'found+"-"+g1+"-"+g2', 'abcd-b-cd'),
    ('a[bcd]*dcdcde', 'adcdcde', SUCCEED, 'found', 'adcdcde'),
    ('a[bcd]+dcdcde', 'adcdcde', FAIL),
    ('(ab|a)b*c', 'abc', SUCCEED, 'found+"-"+g1', 'abc-ab'),
    ('((a)(b)c)(d)', 'abcd', SUCCEED, 'g1+"-"+g2+"-"+g3+"-"+g4', 'abc-a-b-d'),

    ('^', 'abc', SUCCEED, 'found+"-"', '-'),
    ('$', 'abc', SUCCEED, 'found+"-"', '-'),
    (')(', '-', SYNTAX_ERROR),

*/
  };
