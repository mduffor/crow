#include <gtest/gtest.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Script/InkScript.hpp"
#include "Script/InkParser.hpp"
#include "Script/InkExecute.hpp"
#include "ValueRegistry/ValueRegistrySimple.hpp"

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (InkScript, TextAndLinefeeds)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  registry.SetFloat ("four", 4.0f);


  // Test: Text and linefeeds
  exec.Parse ("Line One\nLine Two\nLine Three");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 4);

  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "Line Two");
  ASSERT_STREQ (exec.Continue (), "Line Three");
  };

//------------------------------------------------------------------------------
TEST (InkScript, KnotsAndDiverts)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);


  // Test:  Knots and diverts
  exec.Parse ("=== knot_one ===\nLine One B\n-> knot_two\nLine Two B\n=== knot_two ===\nLine Three B");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 7);
  ASSERT_STREQ (exec.Continue (), "Line One B");
  ASSERT_STREQ (exec.Continue (), "Line Three B");

  // Test:  Knots and inline diverts
  exec.Parse ("=== knot_one ===\nLine One B-> knot_two\nLine Two B\n=== knot_two ===\nLine Three B");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 6);
  ASSERT_STREQ (exec.Continue (), "Line One B");
  ASSERT_STREQ (exec.Continue (), "Line Three B");

  };

//------------------------------------------------------------------------------
TEST (InkScript, Lists)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: Sequence List
  exec.Parse ("Line {One|Two|Three}\n");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 2);

  ASSERT_STREQ (exec.Continue (), "Line One");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Two");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Three");  exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Three");  exec.Restart ();

  // Test: Cycle List
  exec.Parse ("Line {&One|Two|Three}\n");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 2);

  ASSERT_STREQ (exec.Continue (), "Line One");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Two");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Three");  exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line One");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Two");    exec.Restart ();

  // Test: Once-only List
  exec.Parse ("Line {!One|Two|Three}\n");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 2);

  ASSERT_STREQ (exec.Continue (), "Line One");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Two");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Three");  exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line ");       exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line ");       exec.Restart ();

  // test empty lists

  // Test: Sequence List with empty element
  exec.Parse ("Line {One||Three}\n");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 2);

  ASSERT_STREQ (exec.Continue (), "Line One");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line ");       exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Three");  exec.Restart ();

  // Test: Diverts in (sequence) lists
  exec.Parse ("=== knot_one ===\nLine {One|Two->knot_two}\nLine Three\n=== knot_two ===\nLine Four");

  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "Line Three");
  ASSERT_STREQ (exec.Continue (), "Line Four");
  exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Two");
  ASSERT_STREQ (exec.Continue (), "Line Four");


  // NOTE:  Not sure how to test list shuffles

  // Test: Nested lists
  exec.Parse ("Line {&{One|1}|{Two|2}|Three}\n");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 2);

  ASSERT_STREQ (exec.Continue (), "Line One");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Two");    exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line Three");  exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line 1");      exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line 2");      exec.Restart ();

  };

//------------------------------------------------------------------------------
TEST (InkScript, Expressions)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: Expressions
  exec.Parse (R"(
  Line One
  ~ myVar = 1 + 2
  Line Two
  )");

  ASSERT_TRUE (exec.GetScript()->GetNumElements () == 4);

  registry.SetFloat ("myVar", 1.0f);
  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "Line Two");
  ASSERT_EQ    (registry.GetFloat    ("myVar"),    3.0);

  // Test: Variable Expansion
  exec.Parse ("Line {myString}");
  registry.SetString ("myString", "One");
  ASSERT_STREQ (exec.Continue (), "Line One");

  // Test: Block expressions

  exec.Parse (R"(
  Line One
  ~~~
  myVar = 1 + 2;
  myVar += 3;
  ~~~
  Line Two
  )");

  registry.SetFloat ("myVar", 1.0f);
  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "Line Two");
  ASSERT_EQ    (registry.GetFloat    ("myVar"),    6.0);


  // TODO: Test: Function Call Expansion

  };

//------------------------------------------------------------------------------
TEST (InkScript, Conditionals)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);


  // Test: Conditional Text
  registry.SetFloat ("true", 1.0f);
  registry.SetFloat ("false", 0.0f);

  exec.Parse ("Line {true:One}");
  ASSERT_STREQ (exec.Continue (), "Line One");
  exec.Parse ("Line {false:One}");
  ASSERT_STREQ (exec.Continue (), "Line ");

  exec.Parse ("Line {true:One|Two}");
  ASSERT_STREQ (exec.Continue (), "Line One");
  exec.Parse ("Line {false:One|Two}");
  ASSERT_STREQ (exec.Continue (), "Line Two");

  // Test: Conditional Divert
  registry.SetFloat ("true", 1.0f);
  registry.SetFloat ("false", 0.0f);

  exec.Parse ("=== knot_one ===\nLine {true:One->knot_two}\nLine Two\n=== knot_two ===\nLine Three");
  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "Line Three");

  exec.Parse ("=== knot_one ===\nLine {true:->knot_two}\nLine Two\n=== knot_two ===\nLine Three");
  ASSERT_STREQ (exec.Continue (), "Line ");
  ASSERT_STREQ (exec.Continue (), "Line Three");

  exec.Parse ("=== knot_one ===\n{true:->knot_two}\nLine Two\n=== knot_two ===\nLine Three");
  ASSERT_STREQ (exec.Continue (), "Line Three");

  // Test: Variable expansion with local variables
  exec.Parse ("VAR test_var = 1\n Line {test_var}");
  ASSERT_STREQ (exec.Continue (), "Line 1");

  };

//------------------------------------------------------------------------------
TEST (InkScript, Choices)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: Choices
  exec.Parse ("=== knot_one ===\nLine One\n* Choice [One]is one.\n* Choice Two[] is chosen.\nLine after Choice Two\n=== knot_two ===\nLine Four");

  //script.DebugPrint ();

  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_TRUE (exec.GetNumChoices () > 0);

  ASSERT_STREQ (exec.GetChoiceText (0), "Choice One");
  ASSERT_STREQ (exec.GetChoiceText (1), "Choice Two");
  ASSERT_STREQ (exec.GetChoiceText (3), "");
  ASSERT_TRUE (exec.GetNumChoices () == 2);

  exec.ChooseChoiceIndex(0);
  ASSERT_STREQ (exec.Continue (), "Choice is one.");
  ASSERT_STREQ (exec.Continue (), "Line Four");


  exec.Restart ();
  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "");
  ASSERT_STREQ (exec.GetChoiceText (1), "Choice Two");
  exec.ChooseChoiceIndex(1);
  ASSERT_STREQ (exec.Continue (), "Choice Two is chosen.");
  ASSERT_STREQ (exec.Continue (), "Line after Choice Two");
  ASSERT_STREQ (exec.Continue (), "Line Four");

  };

//------------------------------------------------------------------------------
TEST (InkScript, Functions)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: Functions
  exec.Parse ("-> knot_after\n=== function myFn ===\nFunction\n=== knot_after ===\nKnot After A\n~ myFn()\nKnot After B\n");

  ASSERT_STREQ (exec.Continue (), "Knot After A");
  ASSERT_STREQ (exec.Continue (), "Function");
  ASSERT_STREQ (exec.Continue (), "Knot After B");

  //DBG_INFO ("------- Running test -------");


  };

//------------------------------------------------------------------------------
TEST (InkScript, Glue)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: Glue
  exec.Parse ("Line One<>\nLine Two\nLine Three");

  ASSERT_STREQ (exec.Continue (), "Line OneLine Two");
  ASSERT_STREQ (exec.Continue (), "Line Three");
  };

//------------------------------------------------------------------------------
TEST (InkScript, End)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: divert to END
  exec.Parse ("Line One\n->END\nLine Two\nLine Three");

  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "");
  ASSERT_EQ (exec.CanContinue (), FALSE);


  // Test: tail divert to END
  exec.Parse ("Line One->END\nLine Two\nLine Three");

  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_EQ (exec.CanContinue (), FALSE);

  };

//------------------------------------------------------------------------------
TEST (InkScript, Tags)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: To make sure characters and syntax we will use in tags makes it through
  exec.Parse ("Line : One\nLine ((Two))\nLine : Three ((Tags Live Here)) End\nLine # Four this is my tag");

  ASSERT_STREQ (exec.Continue (), "Line : One");
  ASSERT_STREQ (exec.Continue (), "Line ((Two))");
  ASSERT_STREQ (exec.Continue (), "Line : Three ((Tags Live Here)) End");
  ASSERT_STREQ (exec.Continue (), "Line # Four this is my tag");
  };

//------------------------------------------------------------------------------
TEST (InkScript, Comments)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // Test: Block comments
  exec.Parse ("Line One\nLine /*Two\nLine Three\nLine*/ Four");

  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "Line ");
  ASSERT_STREQ (exec.Continue (), "Four");


  // Test: Line comments
  exec.Parse ("Line One\nLine //Two\nLine Three\nLine// Four");

  ASSERT_STREQ (exec.Continue (), "Line One");
  ASSERT_STREQ (exec.Continue (), "Line ");
  ASSERT_STREQ (exec.Continue (), "Line Three");
  ASSERT_STREQ (exec.Continue (), "Line");

  };



//------------------------------------------------------------------------------
TEST (InkScript, Other)
  {
  ValueRegistrySimple  registry;
  InkExecute           exec;
  exec.SetRegistry (&registry);

  // TODO: Test comments


  // TODO: Test stitches

  /*
  DBG_INFO ("Showing results");

  while (script.HasContent ())
    {
    while (script.CanContinue ())
      {
      const char *  pszContent = script.Continue ();
      // check for null and empty strings to catch when we hit choices
      if ((pszContent != NULL) && (pszContent[0] != '\n'))
        {
        printf ("%s\n", pszContent);
        };
      };
    if (script.GetNumChoices () > 0)
      {
      for (INT  iIndex = 0; iIndex < script.GetNumChoices (); ++iIndex)
        {
        printf ("%d) %s\n", iIndex + 1, script.GetChoiceText (iIndex));
        };
      // get input (1 to NumChoices)
      INT  iInput = 1;
      script.ChooseChoiceIndex (iInput - 1) // convert back to 0-based index
      };
    };




  while (script.CanContinue ())
    {
    printf ("%s\n", script.Continue ());
    };



  */

  //printf ("%d\n", script.listElem[0]);
  //printf ("%d\n", script.listElem[1]);
  //printf ("%d\n", script.listElem[2]);

  //printf ("%s\n", script.Continue ());
  //printf ("%s\n", script.Continue ());
  //printf ("%s\n", script.Continue ());

/*
    ValueRegistry *         pRegistry;
    TArray<InkElem*>        listElem;  // parsed version of the text script
    RStrParser              parserFullScript;
    INT                     iCurrElem;
    RStrArray               arrayChoices;
    IntArray                arrayChoiceIndexes;
    RStr                    strTextOut;

    VOID          Parse              (const char *  szScriptIn);

    BOOL          CanContinue        (VOID); // True if there is more content, False if not

    const char *  Continue           (VOID); // return the curent line of content.

    INT           GetNumChoices      (VOID); // return current number of choices.  0 if not on a choice.

    const char *  GetChoiceText      (INT  iIndexIn); // return the text for a given choice.  0 <= iIndex < GetNumChioces ()

    VOID          ChooseChoiceIndex  (INT  iIndexIn); // go to the content of the choice.

    VOID          ClearChoices       (VOID);

*/






/*

  RStr  strResult;

  strResult = HTTP::HeaderBasicAuth ("Aladdin", "OpenSesame");
  ASSERT_STREQ (strResult.AsChar (), "Authorization: Basic QWxhZGRpbjpPcGVuU2VzYW1l\r\n");
*/
  };


