#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Util/RStrParser.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

// TODO: More thorough testing of RStr specifically.
//
//  VOID  RStr::ToHex  (RStr &  strHexOut)
//  VOID  RStr::Replace

//------------------------------------------------------------------------------
TEST (RStrParser, BasicTest)
  {
  RStrParser  parser;

  RStr  strTest((const char *)NULL);
  ASSERT_STREQ (strTest.AsChar(), ""); // RStrParser.Set()
  ASSERT_EQ    (strTest.Length (), 0);           // RStr.StringLength

  parser.Set ("Hello World");
  ASSERT_STREQ (parser.AsChar(), "Hello World"); // RStrParser.Set()
  ASSERT_EQ    (parser.Length (), 11);           // RStr.StringLength
  ASSERT_EQ    (parser.GetAt(6), 'W');           // RStr.GetAt() );
  parser.SetCursorStart (-1);
  ASSERT_EQ (parser.GetCursorStart(), 0);        // RStrParser.SetCursorStart(-1)
  parser.SetCursorStart (2);
  ASSERT_EQ (parser.GetCursorStart(), 2);        // RStrParser.SetCursorStart(2)
  parser.SetCursorStart (100);
  ASSERT_EQ (parser.GetCursorStart(), 11);       // RStrParser.SetCursorStart(100)

  parser.Set ("Abcdefghi\nBjklmnopq\n");
  ASSERT_EQ (parser.GetLineEnd (), 9);           // RStrParser.FindLineEnd()
  parser.GotoNextLine ();
  ASSERT_EQ (parser.GetCursorStart(), 10);       // RStrParser.GotoNextLine()
  parser.GotoNextLine ();
  ASSERT_EQ (parser.GetCursorStart(), 20);       // RStrParser.GotoNextLine()


  parser.Set ("     Abcdefghi\nBjklmnopq\n");
  parser.SkipWhitespace ();
  ASSERT_EQ (parser.GetCursorStart(), 5);        // RStrParser.SkipWhitespace()
  ASSERT_TRUE (parser.IsWhitespace(' '));        // RStrParser.IsWhitespace()


  parser.Set ("Hello World");
  parser.Set ("");
  ASSERT_STREQ (parser.AsChar(), "");            // RStrParser.Set() with null
  parser.Set ("Hello World");
  parser.Set (NULL);
  ASSERT_STREQ (parser.AsChar(), "");            // RStrParser.Set() with null
  }

//------------------------------------------------------------------------------
TEST (RStrParser, CommentTest)
  {
  RStrParser  parser;

  // Note:  Should probably do more thorough comment function testing
  parser.Set ("Line 1\n// EOL comment Line 2\nLine 3 /* Block Comment Start \nLine 4*/\n");
  parser.SetSkipComments (RStrParser::kCStyle);
  parser.GotoNextLine();

  ASSERT_TRUE  (parser.IsEOLComment   (parser.GetCursorStart())); // RStrParser.IsEOLComment()
  ASSERT_FALSE (parser.IsBlockComment (parser.GetCursorStart())); // RStrParser.IsBlockComment() (neg)
  ASSERT_TRUE  (parser.IsComment      (parser.GetCursorStart())); // RStrParser.IsComment()
  parser.SkipComment();

  ASSERT_EQ (parser.GetCursorStart(), 28); // RStrParser.SkipComment()
  parser.SkipEOL();
  parser.SkipChars(7);
  ASSERT_EQ (parser.GetCursorStart(), 36); // RStrParser.SkipChars()
  ASSERT_TRUE (parser.IsBlockComment(parser.GetCursorStart())); // RStrParser.IsBlockComment()
  parser.SkipComment();
  ASSERT_EQ( parser.GetCursorStart(), 68); // RStrParser.SkipComment()
  }

//------------------------------------------------------------------------------
TEST (RStrParser, ElementParsingTest)
  {
  RStrParser  parser;
  RStr        strOut;

  // test element parsing
  parser.Set ("Apple Banana \"Cherry\" [Donut] [Eggs and Bacon]\n");
  parser.SetSkipComments (RStrParser::kNone);
  ASSERT_STREQ (parser.GetWord(&strOut), "Apple");        // RStrParser.GetWord()
  ASSERT_EQ    (parser.GetCursorStart(), 6);              // RStrParser.GetWord() (check cursor advance)

  ASSERT_STREQ (parser.GetWord(strOut, true, "n"), "Ba"); // RStrParser.GetWord()
  ASSERT_EQ    (parser.GetCursorStart(), 8);              // RStrParser.GetWord() (check cursor advance)
  ASSERT_STREQ (parser.GetWord(&strOut), "nana");         // RStrParser.GetWord() (midword start)

  ASSERT_STREQ (parser.GetQuoteString(&strOut), "Cherry");           // RStrParser.GetWord()
  ASSERT_STREQ (parser.GetBracketString(&strOut), "Donut");          // RStrParser.GetBracketString()
  ASSERT_STREQ (parser.GetBracketString(&strOut), "Eggs and Bacon"); // RStrParser.GetBracketString() (with spaces)

  parser.Set ("91");
  ASSERT_EQ (parser.GetInt(), 91);

  parser.Set ("0.911111");
  ASSERT_TRUE (FLT_APPROX_EQUAL(parser.GetFloat(), 0.911111f));


  }


//------------------------------------------------------------------------------
TEST (RStrParser, AddStringsTest)
  {
  RStrParser  parser;
  RStr        strOut;
  RStr        strIn;

  // test adding strings
  parser.Set ("");
  ASSERT_TRUE (parser.IsEmpty());                    // RStrParser.IsEmpty()
  parser.AppendString ("Hello");
  ASSERT_STREQ (parser.AsChar(), "Hello");           // RStrParser.AppendString() (to empty)
  parser.AppendString (" World");
  ASSERT_STREQ (parser.AsChar(), "Hello World");     // RStrParser.AppendString()
  parser.PrependString ("Yo, ");
  ASSERT_STREQ (parser.AsChar(), "Yo, Hello World"); // RStrParser.PrependString()


  parser.Set ("");
  ASSERT_TRUE (parser.IsEmpty());                    // RStrParser.IsEmpty()
  strIn.Set ("Hello");
  parser.AppendString (strIn);
  ASSERT_STREQ (parser.AsChar(), "Hello");           // RStrParser.AppendString() (to empty)
  strIn.Set (" World");
  parser.AppendString (strIn);
  ASSERT_STREQ (parser.AsChar(), "Hello World");     // RStrParser.AppendString()
  strIn.Set ("Yo, ");
  parser.PrependString (strIn);
  ASSERT_STREQ (parser.AsChar(), "Yo, Hello World"); // RStrParser.PrependString()
  };


//------------------------------------------------------------------------------
TEST (RStrParser, Searching)
  {
  RStrParser  parser;

  parser.Set ("Hello World");

  ASSERT_TRUE (parser.StartsWith ("Hello"));
  ASSERT_TRUE (parser.EndsWith   ("World"));

  ASSERT_TRUE (parser.StartsWith ((UINT32)'H'));
  ASSERT_TRUE (parser.EndsWith   (UINT32('d')));

  ASSERT_TRUE (parser.Contains   ("Wor"));


  parser.Set ("A\nB\n C \nD\n  E\n\nF\nG\nH  \nI\nJ\n\n\n");

  parser.SetCursorStart (parser.Find("C"));
  ASSERT_EQ (parser.GetLineNumber (), 3);

  parser.SetCursorStart (parser.Find("E"));
  ASSERT_EQ (parser.GetLineNumber (), 5);

  parser.SetCursorStart (parser.Find("H"));
  ASSERT_EQ (parser.GetLineNumber (), 9);


  // Test Identifiers
  ASSERT_TRUE (parser.IsIdentChar ('A'));
  ASSERT_TRUE (parser.IsIdentChar ('L'));
  ASSERT_TRUE (parser.IsIdentChar ('Z'));
  ASSERT_TRUE (parser.IsIdentChar ('a'));
  ASSERT_TRUE (parser.IsIdentChar ('k'));
  ASSERT_TRUE (parser.IsIdentChar ('z'));
  ASSERT_TRUE (parser.IsIdentChar ('0'));
  ASSERT_TRUE (parser.IsIdentChar ('3'));
  ASSERT_TRUE (parser.IsIdentChar ('9'));
  ASSERT_TRUE (parser.IsIdentChar ('.'));
  ASSERT_TRUE (parser.IsIdentChar ('_'));

  ASSERT_FALSE (parser.IsIdentChar ('?'));
  ASSERT_FALSE (parser.IsIdentChar ('%'));
  ASSERT_FALSE (parser.IsIdentChar ('{'));
  ASSERT_FALSE (parser.IsIdentChar (')'));
  ASSERT_FALSE (parser.IsIdentChar ('+'));

  parser.Set ("Identifier NotMatching");

  ASSERT_EQ (parser.FindIdentifier ("Identifier"), 0);
  ASSERT_EQ (parser.FindIdentifier ("Not"), -1);
  ASSERT_EQ (parser.FindIdentifier ("Matching"), -1);
  ASSERT_EQ (parser.FindIdentifier ("NotMatching"), 11);
  };
